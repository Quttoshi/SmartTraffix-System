#include "Vehicle.h"
#include <algorithm>
#include <iostream>

Vehicle::Vehicle(Type type, int id, float initSpeed, sf::Vector2f direction, sf::Vector2f startPosition)
    : vehicleType(type), id(id), speed(initSpeed), direction(direction), currentPosition(startPosition),
      operational(true), challanStatus("Inactive"), timeUntilBreakdown(-1.0f) {
    if (type == Light) maxAllowedSpeed = 60.0f;
    else if (type == Heavy) maxAllowedSpeed = 40.0f;
    else if (type == Emergency) maxAllowedSpeed = 80.0f;
}

void Vehicle::move(float deltaTime) {
    if (!isOperational()) return;

    if (rand() % 100 < 50) speed += 50.0f;

    sf::Vector2f movement = direction * speed * deltaTime;
    currentPosition += movement;

    speed = std::min(speed + (5.0f * deltaTime), maxAllowedSpeed);
}

void Vehicle::stop() {
    operational = false;
}

bool Vehicle::isOperational() const {
    return operational;
}

bool Vehicle::isOutOfBounds() const {
    return currentPosition.x < 0 || currentPosition.x > 800 || currentPosition.y < 0 || currentPosition.y > 600;
}

Vehicle::Type Vehicle::getCategory() const {
    return vehicleType;
}

sf::Vector2f Vehicle::getPosition() const {
    return currentPosition;
}

int Vehicle::getID() const {
    return id;
}

float Vehicle::getCurrentSpeed() const {
    return speed;
}

float Vehicle::getMaxSpeed() const {
    return maxAllowedSpeed;
}

const std::string& Vehicle::getPenaltyStatus() const {
    return challanStatus;
}

void Vehicle::setPenaltyStatus(const std::string& status) {
    challanStatus = status;
    std::cout << "Vehicle ID: " << id << " penalty status set to: " << challanStatus << "\n";
}

void Vehicle::setTimeUntilBreakdown(float time) {
    timeUntilBreakdown = time;
}

void Vehicle::reduceTimeUntilBreakdown(float deltaTime) {
    if (timeUntilBreakdown > 0) {
        timeUntilBreakdown -= deltaTime;
    }
}

bool Vehicle::shouldBreakDown() const {
    return timeUntilBreakdown <= 0;
}
