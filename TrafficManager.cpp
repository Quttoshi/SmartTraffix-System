#include "TrafficManager.h"
#include "ChallanGenerator.h"
#include <ctime>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <algorithm>

auto simulationStart = std::chrono::steady_clock::now();


TrafficManager::TrafficManager()
    : maxVehicles(10),
      available(10),
      trafficLightState(0),
      lightTimer(0.0f) {
    laneVehicles.resize(4);
    waitingVehicles.resize(4);

    for (int i = 0; i < 4; ++i) {
        allocated[i] = 0;
        max[i] = 10;
        need[i] = max[i] - allocated[i];
    }

    srand(static_cast<unsigned>(time(0)));
}

//function to check if there is any emergency vehicle in the lane
bool TrafficManager::hasEmergencyVehicle(int lane) const {
    for (const auto& vehicle : laneVehicles[lane]) {
        if (vehicle.getCategory() == Vehicle::Emergency) {
            return true;
        }
    }
    return false;
}

//function to check if the challan status is blocked
void TrafficManager::simulateIntersection(float elapsedTime) {
    lightTimer += elapsedTime;

    if (lightTimer >= greenLightDuration) {
        trafficLightState = 1 - trafficLightState;
        lightTimer = 0.0f;
    }

    generateVehicles(elapsedTime);
    for (int lane = 0; lane < 4; ++lane) {
        updateVehiclesInLane(lane, elapsedTime);
    }

    monitorVehicleSpeeds();
}

//function to check if the lane is green
bool TrafficManager::isLaneGreen(int lane) const {
    if (trafficLightState == 0) {
        return lane == 0 || lane == 2;
    } else {
        return lane == 1 || lane == 3;
    }
}

//function to add vehicle to the lane
void TrafficManager::addVehicleToLane(int lane, Vehicle::Type type) {
    sf::Vector2f direction;
    sf::Vector2f startPosition;
    float sideOffset = 20.0f;

    if (lane == 0) {
        direction = {1.0f, 0.0f};
        startPosition = {0.0f, 275.0f + (laneVehicles[lane].size() % 2 == 0 ? -sideOffset : sideOffset)};
    } else if (lane == 1) {
        direction = {0.0f, -1.0f};
        startPosition = {375.0f + (laneVehicles[lane].size() % 2 == 0 ? -sideOffset : sideOffset), 580.0f};
    } else if (lane == 2) {
        direction = {-1.0f, 0.0f};
        startPosition = {780.0f, 325.0f + (laneVehicles[lane].size() % 2 == 0 ? -sideOffset : sideOffset)};
    } else if (lane == 3) {
        direction = {0.0f, 1.0f};
        startPosition = {425.0f + (laneVehicles[lane].size() % 2 == 0 ? -sideOffset : sideOffset), 0.0f};
    }

    float initialSpeed = 30.0f;

    if (rand() % 100 < 10) {
        type = Vehicle::Heavy;
    }

    Vehicle newVehicle(type, rand() % 10000, initialSpeed, direction, startPosition);

    if (rand() % 100 < 20) {
        newVehicle.setTimeUntilBreakdown(static_cast<float>(rand() % 10 + 5));
    } else {
        newVehicle.setTimeUntilBreakdown(-1.0f);
    }

    laneVehicles[lane].push_back(newVehicle);
}

//function to update the vehicles in the lane
void TrafficManager::updateVehiclesInLane(int lane, float timeStep) {
    for (auto it = laneVehicles[lane].begin(); it != laneVehicles[lane].end();) {
        auto& vehicle = *it;

        if (isLaneGreen(lane)) {
            if (vehicle.isOperational()) {
                vehicle.move(timeStep);
            }
        }

        if (vehicle.isOutOfBounds()) {
            it = laneVehicles[lane].erase(it);
        } else {
            ++it;
        }
    }
}

//function to generate vehicles
void TrafficManager::generateVehicles(float timeStep) {
    static float heavyVehicleTimers[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static float regularVehicleTimers[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 4; ++i) {
        heavyVehicleTimers[i] += timeStep;
        regularVehicleTimers[i] += timeStep;

        if (heavyVehicleTimers[i] >= 15.0f) {
            heavyVehicleTimers[i] = 0.0f;
            addVehicleToLane(i, Vehicle::Heavy);
        }

        if (regularVehicleTimers[i] >= 2.0f) {
            regularVehicleTimers[i] = 0.0f;
            Vehicle::Type type = (rand() % 100 < getEmergencyVehicleProbability(i))
                                    ? Vehicle::Emergency
                                    : Vehicle::Light;

            addVehicleToLane(i, type);
        }
    }
}

//function to manage the lane queues
void TrafficManager::manageLaneQueues() {
    for (int i = 0; i < 4; ++i) {
        while (!waitingVehicles[i].empty() && laneVehicles[i].size() < 10) {
            Vehicle vehicle = waitingVehicles[i].front();
            waitingVehicles[i].pop();

            if (vehicle.getCategory() == Vehicle::Emergency) {
                laneVehicles[i].insert(laneVehicles[i].begin(), vehicle);
            } else {
                laneVehicles[i].push_back(vehicle);
            }
        }
    }
}

//function to handle broken vehicles
void TrafficManager::handleBrokenVehicles() {
    static bool outOfOrderHandled = false;

    for (int i = 0; i < 4; ++i) {
        for (auto& vehicle : laneVehicles[i]) {
            if (!outOfOrderHandled && rand() % 100 < 1) {
                vehicle.stop();
                outOfOrderHandled = true;
            }
        }
    }
}

//function to monitor vehicle speeds
void TrafficManager::monitorVehicleSpeeds() {
    for (int lane = 0; lane < 4; ++lane) {
        for (auto& vehicle : laneVehicles[lane]) {
            if (vehicle.getCurrentSpeed() > vehicle.getMaxSpeed()) {
                std::string vehicleType = vehicleTypeToString(vehicle.getCategory());
                auto challan = challanGenerator.createChallan(vehicle.getID(), vehicle.getCurrentSpeed(), vehicleType);
                challanGenerator.sendChallanToUserPortal(challan);
            }
        }
    }
}

//function to check if the challan status is blocked
bool TrafficManager::isPeakHour() const {
    auto now = std::chrono::steady_clock::now();
    int simulationTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - simulationStart).count();
    int currentHour = (simulationTimeInSeconds / 3600) % 24;
    return (currentHour >= 7 && currentHour <= 9) || (currentHour >= 16 && currentHour <= 19);
}

//function to get the emergency vehicle probability
int TrafficManager::getEmergencyVehicleProbability(int lane) const {
    if (lane == 0) return 20;
    if (lane == 1) return 10;
    if (lane == 2) return 5;
    if (lane == 3) return 30;
    return 0;
}

//function to convert vehicle type to string
std::string TrafficManager::vehicleTypeToString(Vehicle::Type type) {
    switch (type) {
        case Vehicle::Light: return "Regular";
        case Vehicle::Heavy: return "Heavy";
        case Vehicle::Emergency: return "Emergency";
        default: return "Unknown";
    }
}

//function to check if the challan status is blocked
void TrafficManager::sendChallanToUserPortal(const ChallanGenerator::Challan& challan) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        close(pipefd[1]);
        char buffer[256];
        read(pipefd[0], buffer, sizeof(buffer));
        close(pipefd[0]);
        std::cout << "UserPortal received challan details:\n" << buffer << "\n";
        _exit(0);
    } else {
        close(pipefd[0]);
        std::string challanDetails =
            "Challan ID: " + std::to_string(challan.challanId) +
            ", Vehicle ID: " + std::to_string(challan.vehicleId) +
            ", Type: " + challan.vehicleType +
            ", Total Amount: " + std::to_string(challan.totalAmount) +
            ", Issue Date: " + std::ctime(&challan.issueDate) +
            ", Due Date: " + std::ctime(&challan.dueDate);

        write(pipefd[1], challanDetails.c_str(), challanDetails.size());
        close(pipefd[1]);
        wait(nullptr);
    }
}

//function to get the total vehicle count
int TrafficManager::getTotalVehicleCount() const {
    int total = 0;
    for (const auto& lane : laneVehicles) {
        total += lane.size();
    }
    return total;
}

//function to get the vehicle count by type
int TrafficManager::getActiveChallanCount() const {
    int count = 0;
    for (const auto& lane : laneVehicles) {
        count += std::count_if(lane.begin(), lane.end(), [](const Vehicle& v) {
            return v.getPenaltyStatus() == "Active";
        });
    }
    return count;
}

//function to get the vehicle count by type
std::vector<int> TrafficManager::getVehiclesWithActiveChallans() const {
    std::vector<int> ids;
    for (const auto& lane : laneVehicles) {
        for (const auto& vehicle : lane) {
            if (vehicle.getPenaltyStatus() == "Active") {
                ids.push_back(vehicle.getID());
            }
        }
    }
    return ids;
}

//function to check if the challan status is blocked
bool TrafficManager::requestResources(int lane, const std::vector<int>& request) {
    for (size_t i = 0; i < request.size(); ++i) {
        if (request[i] > need[lane]) return false;
        if (request[i] > available) return false;
    }

    for (size_t i = 0; i < request.size(); ++i) {
        available -= request[i];
        allocated[lane] += request[i];
        need[lane] -= request[i];
    }

    if (!isSafeState()) {
        for (size_t i = 0; i < request.size(); ++i) {
            available += request[i];
            allocated[lane] -= request[i];
            need[lane] += request[i];
        }
        return false;
    }

    return true;
}

//function to release the resources
void TrafficManager::releaseResources(int lane, const std::vector<int>& release) {
    for (size_t i = 0; i < release.size(); ++i) {
        allocated[lane] -= release[i];
        available += release[i];
        need[lane] += release[i];
    }
}

//function to check if the state is safe
bool TrafficManager::isSafeState() const {
    int work = available;
    bool finish[4] = {false};

    for (int count = 0; count < 4; ++count) {
        bool found = false;
        for (int i = 0; i < 4; ++i) {
            if (!finish[i] && need[i] <= work) {
                work += allocated[i];
                finish[i] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}
