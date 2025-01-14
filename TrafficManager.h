#ifndef TRAFFICMANAGER_H
#define TRAFFICMANAGER_H

#include <vector>
#include <queue>
#include <atomic>
#include <string>
#include "Vehicle.h"
#include "ChallanGenerator.h"

class TrafficManager {
public:
    TrafficManager();

    void simulateIntersection(float elapsedTime);
    void updateVehiclesInLane(int lane, float timeStep);
    bool isLaneGreen(int lane) const;

    int getTotalVehicleCount() const;
    int getVehicleCountByType(Vehicle::Type type) const;
    int getActiveChallanCount() const;
    std::vector<int> getVehiclesWithActiveChallans() const;

    const std::vector<Vehicle>& getLaneVehicles(int lane) const { return laneVehicles[lane]; }

    bool requestResources(int lane, const std::vector<int>& request);
    void releaseResources(int lane, const std::vector<int>& release);
    bool isSafeState() const;

private:
    int maxVehicles;
    int available;
    int trafficLightState;
    const float greenLightDuration = 10.0f;
    float lightTimer;

    int allocated[4];
    int max[4];
    int need[4];
    std::vector<std::queue<Vehicle>> waitingVehicles;
    std::vector<std::vector<Vehicle>> laneVehicles;

    ChallanGenerator challanGenerator;
    void sendChallanToUserPortal(const ChallanGenerator::Challan& challan);

    void generateVehicles(float timeStep);
    void addVehicleToLane(int lane, Vehicle::Type type);
    void handleBrokenVehicles();
    void manageLaneQueues();
    void monitorVehicleSpeeds();
    bool hasEmergencyVehicle(int lane) const;

    bool isPeakHour() const;
    int getEmergencyVehicleProbability(int lane) const;
    std::string vehicleTypeToString(Vehicle::Type type);

    bool challanStatusBlocked() const;
};

#endif
