#include <SFML/Graphics.hpp>
#include <thread>
#include <atomic>
#include "TrafficManager.h"
#include "Graphics.h"
#include "UserPortal.h"
#include "StripePayment.h"
#include "ChallanGenerator.h"
#include <iostream>
#include <string>

std::atomic<bool> runningSimulation(true);

// Menu handling function
void menuThread(UserPortal& userPortal, StripePayment& stripePayment, ChallanGenerator& challanGenerator) {
    int menuChoice;
    while (runningSimulation) {
        std::cout << "\n--- Menu ---\n";
        std::cout << "1. View Challan Details\n";
        std::cout << "2. Pay Challan\n";
        std::cout << "3. Exit Program\n";
        std::cout << "Enter your choice: ";
        std::cin >> menuChoice;

        switch (menuChoice) {
            case 1: { // View Challan Details
                int challanId;
                std::cout << "Enter Challan ID to view details: ";
                std::cin >> challanId;

                userPortal.displayChallanDetails(challanId);
                break;
            }
            case 2: { // Pay Challan
                int challanId;
                float amount;

                std::cout << "Enter Challan ID to pay: ";
                std::cin >> challanId;
                std::cout << "Enter Amount to Pay: ";
                std::cin >> amount;

                stripePayment.processPayment(challanId, 0, "Regular", amount); // Adjust parameters as needed
                challanGenerator.updatePaymentStatus(challanId, "Paid");
                break;
            }
            case 3: // Exit Program
                runningSimulation = false; // Stop simulation
                std::cout << "Exiting program...\n";
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Traffic Light Simulation");

    // Initialize objects
    TrafficManager trafficManager;
    Graphics graphics;
    UserPortal userPortal;
    StripePayment stripePayment;
    ChallanGenerator challanGenerator;

    // Start the menu thread
    std::thread menu(menuThread, std::ref(userPortal), std::ref(stripePayment), std::ref(challanGenerator));

    // Main simulation loop starts immediately
    sf::Clock clock;
    while (window.isOpen() && runningSimulation) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                runningSimulation = false;
            }
        }

        // Update simulation
        float deltaTime = clock.restart().asSeconds();
        trafficManager.simulateIntersection(deltaTime);

        // Render the simulation
        window.clear(sf::Color::White);
        graphics.render(window, trafficManager);
        window.display();
    }

    // Ensure the menu thread finishes
    menu.join();

    return 0;
}
