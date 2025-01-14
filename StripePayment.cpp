#include "StripePayment.h"
#include <iostream>

void StripePayment::processPayment(int challanId, int vehicleId, const std::string& vehicleType, float amountToPay) {
    std::cout << "Processing payment for Challan ID: " << challanId << "\n";
    std::cout << "Vehicle ID: " << vehicleId << ", Type: " << vehicleType << ", Amount: " << amountToPay << "\n";

    if (amountToPay > 0) {
        std::cout << "Payment successful for Vehicle ID: " << vehicleId << "\n";
    } else {
        std::cout << "Payment failed. Amount to pay: " << amountToPay << "\n";
    }
}
