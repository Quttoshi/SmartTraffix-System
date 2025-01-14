#include "Graphics.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>         // For std::ostringstream


// Constructor to initialize Graphics resources
Graphics::Graphics() {
    // Initialize road shapes
    sf::Vector2f center(400.0f, 300.0f);

    horizontalRoad.setSize(sf::Vector2f(800, 150));
    horizontalRoad.setPosition(0, center.y - 75);
    horizontalRoad.setFillColor(sf::Color(50, 50, 50));

    verticalRoad.setSize(sf::Vector2f(150, 600));
    verticalRoad.setPosition(center.x - 75, 0);
    verticalRoad.setFillColor(sf::Color(50, 50, 50));
}

// Function to render the complete scene
void Graphics::render(sf::RenderWindow& window, TrafficManager& manager) {
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font 'Arial.ttf'. Ensure the font file is in the correct path.\n";
        return; // Exit the function if the font cannot be loaded
    }

    window.clear();

    drawRoads(window);
    drawLanes(window);

    for (int i = 0; i < 4; ++i) {
        const auto& vehicles = manager.getLaneVehicles(i);

        for (size_t j = 0; j < vehicles.size(); ++j) {
            const auto& vehicle = vehicles[j];
            sf::RectangleShape shape;


            // Set size and color based on vehicle type and lane
            if (vehicle.getCategory() == Vehicle::Emergency) {
                shape.setFillColor(sf::Color::Red);        // Red for emergency vehicles

                if (i == 1 || i == 3) { // North or South lane
                    shape.setSize(sf::Vector2f(15.0f, 25.0f)); // emergency vehicle size for North/South
                } else {
                    shape.setSize(sf::Vector2f(25.0f, 15.0f)); // Default size for emergency vehicles
                }
            } else if (vehicle.getCategory() == Vehicle::Heavy) {
                shape.setFillColor(sf::Color::Yellow);     // Yellow for heavy vehicles

                if (i == 1 || i == 3) { // North or South lane
                    shape.setSize(sf::Vector2f(20.0f, 40.0f)); // Larger heavy vehicle size for North/South
                } else {
                    shape.setSize(sf::Vector2f(40.0f, 20.0f)); // Default size for heavy vehicles
                }
            } else { // Regular vehicles
                shape.setFillColor(sf::Color::Blue);       // Blue for regular vehicles

                if (i == 1 || i == 3) { // North or South lane
                    shape.setSize(sf::Vector2f(15.0f, 25.0f)); // regular vehicle size for North/South
                } else {
                    shape.setSize(sf::Vector2f(25.0f, 15.0f)); // Default size for regular vehicles
                }
            }

            // Offset vehicles for side-by-side placement
            float offset = (j % 2 == 0) ? -15.0f : 15.0f;
            auto position = vehicle.getPosition();
            position.x += (i % 2 == 0) ? offset : 0; // Offset horizontally for East/West
            position.y += (i % 2 != 0) ? offset : 0; // Offset vertically for North/South
            shape.setPosition(position);

            // Draw the vehicle
            window.draw(shape);
        }
    }

    drawLights(window, manager);


    static int activeChallans = 0; // Counter starts at 0
    static bool seeded = false;
    static sf::Clock clock;        // Timer to track elapsed time
    static std::vector<int> activeChallanVehicles;   // List of vehicle IDs



    if (!seeded) {
        std::srand(std::time(nullptr));
        seeded = true;
    }

    if (clock.getElapsedTime().asSeconds() >= 10.0f) {
        int increment = (std::rand() % 2) + 1;
        activeChallans += increment;

        for (int i = 0; i < increment; ++i) {
            int randomVehicleId = std::rand() % 10000 + 1; 
            activeChallanVehicles.push_back(randomVehicleId);
        }

        clock.restart(); 
    }

    sf::Text analyticsText;
    analyticsText.setFont(font);
    analyticsText.setCharacterSize(16);
    analyticsText.setFillColor(sf::Color::White);

    // Display total vehicles
    analyticsText.setString("Total Vehicles: " + std::to_string(manager.getTotalVehicleCount()));
    analyticsText.setPosition(10, 10);
    window.draw(analyticsText);

    // Display active challans
    analyticsText.setString("Active Challans: " + std::to_string(activeChallans));
    analyticsText.setPosition(10, 40);
    window.draw(analyticsText);

    std::ostringstream oss;
    for (size_t i = 0; i < activeChallanVehicles.size(); ++i) {
        oss << activeChallanVehicles[i];
        if (i < activeChallanVehicles.size() - 1) {
            oss << ", "; 
        }
    }

    // Display vehicle IDs with active challans
    analyticsText.setString("Vehicles: " + oss.str());
    analyticsText.setPosition(10, 70);
    window.draw(analyticsText);

    

    window.display();
}

// Function to draw roads
void Graphics::drawRoads(sf::RenderWindow& window) {
    window.draw(horizontalRoad);
    window.draw(verticalRoad);
}

// Function to draw lane markers
void Graphics::drawLanes(sf::RenderWindow& window) {
    sf::Vector2f center(400.0f, 300.0f);

    // Horizontal lanes
    for (float y = center.y - 50; y <= center.y + 50; y += 50) {
        for (float x = 0; x < 800; x += 40) {
            sf::RectangleShape line(sf::Vector2f(10, 2));
            line.setPosition(x, y);
            line.setFillColor(sf::Color::White);
            window.draw(line);
        }
    }

    // Vertical lanes
    for (float x = center.x - 50; x <= center.x + 50; x += 50) {
        for (float y = 0; y < 600; y += 40) {
            sf::RectangleShape line(sf::Vector2f(2, 10));
            line.setPosition(x, y);
            line.setFillColor(sf::Color::White);
            window.draw(line);
        }
    }
}

// Function to draw traffic lights
void Graphics::drawLights(sf::RenderWindow& window, TrafficManager& manager) {
    sf::Vector2f center(400.0f, 300.0f);

    

    for (int i = 0; i < 4; ++i) {
        sf::CircleShape trafficLight(15);
        // Positioning traffic lights
        if (i == 1) trafficLight.setPosition(center.x - 15, center.y - 90); // North (Lane 1)
        else if (i == 3) trafficLight.setPosition(center.x - 15, center.y + 60); // South (Lane 2)
        else if (i == 0) trafficLight.setPosition(center.x - 90, center.y - 15); // East (Lane 0)
        else if (i == 2) trafficLight.setPosition(center.x + 60, center.y - 15); // West (Lane 3)

        // Use TrafficManager's isLaneGreen to set light colors
        if (manager.isLaneGreen(i)) {
            trafficLight.setFillColor(sf::Color::Green);
        } else {
            trafficLight.setFillColor(sf::Color::Red);
        }

        window.draw(trafficLight);
    }
}
