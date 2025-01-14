#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include "TrafficManager.h"

class Graphics {
public:
    Graphics(); // Constructor to initialize graphics resources

    // Main render function
    void render(sf::RenderWindow& window, TrafficManager& manager);

private:
    // Helper functions for drawing components
    void drawRoads(sf::RenderWindow& window);
    void drawLanes(sf::RenderWindow& window);
    void drawLights(sf::RenderWindow& window, TrafficManager& manager);
    void drawAnalytics(sf::RenderWindow& window, TrafficManager& manager); // Add this declaration


    // Shape objects to represent the roads
    sf::RectangleShape horizontalRoad;
    sf::RectangleShape verticalRoad;

    // Font for rendering analytics
    sf::Font font;
};

#endif // GRAPHICS_H
