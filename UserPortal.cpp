#include "UserPortal.h"
#include <iostream>

void UserPortal::addChallan(const ChallanGenerator::Challan& challan) {
    challanBuffer.push_back(challan);
}

void UserPortal::displayChallanDetails(int challanId) const {
    for (const auto& challan : challanBuffer) {
        if (challan.challanId == challanId) {
            std::cout << "Challan ID: " << challan.challanId
                      << ", Vehicle ID: " << challan.vehicleId
                      << ", Type: " << challan.vehicleType
                      << ", Amount: " << challan.totalAmount
                      << ", Status: " << challan.paymentStatus << "\n";
            return;
        }
    }
    std::cout << "Challan not found.\n";
}
