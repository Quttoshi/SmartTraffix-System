#ifndef VEHICLE_H
#define VEHICLE_H

#include <SFML/Graphics.hpp>
#include <string>

class Vehicle {
public:
    enum Type { Light, Heavy, Emergency };

    Vehicle(Type type, int id, float initSpeed, sf::Vector2f direction, sf::Vector2f startPosition);

    void move(float deltaTime);
    void stop();
    bool isOperational() const;
    bool isOutOfBounds() const;

    Type getCategory() const;
    sf::Vector2f getPosition() const;
    int getID() const;
    float getCurrentSpeed() const;
    float getMaxSpeed() const;
    const std::string& getPenaltyStatus() const;

    void setPenaltyStatus(const std::string& status);
    void setTimeUntilBreakdown(float time);
    void reduceTimeUntilBreakdown(float deltaTime);
    bool shouldBreakDown() const;

private:
    Type vehicleType;
    int id;
    float speed;
    float maxAllowedSpeed;
    sf::Vector2f direction;
    sf::Vector2f currentPosition;
    bool operational;
    std::string challanStatus;
    float timeUntilBreakdown;
};

#endif
