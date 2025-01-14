#include "ChallanGenerator.h"
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <cstring>
#include <iomanip>

ChallanGenerator::ChallanGenerator() : currentId(1) {
    if (pipe(pipeFd) == -1) {
        std::cerr << "Failed to create pipe: " << strerror(errno) << "\n";
    }
}

ChallanGenerator::~ChallanGenerator() {
    close(pipeFd[0]);
    close(pipeFd[1]);
}

int ChallanGenerator::generateUniqueId() {
    return currentId++;
}

float ChallanGenerator::calculateChallanAmount(float speed, const std::string& vehicleType) {
    float baseAmount = 500.0f;
    float overspeedFactor = (speed > 60) ? (speed - 60) * 10 : 0;
    float typeMultiplier = 1.0f;
    if (vehicleType == "Heavy") {
        typeMultiplier = 1.5f;
    } else if (vehicleType == "Emergency") {
        typeMultiplier = 2.0f;
    }
    return baseAmount + (overspeedFactor * typeMultiplier);
}

ChallanGenerator::Challan ChallanGenerator::createChallan(int vehicleId, float speed, const std::string& vehicleType) {
    Challan challan;
    challan.challanId = generateUniqueId();
    challan.vehicleId = vehicleId;
    challan.speed = speed;
    challan.vehicleType = vehicleType;
    challan.amount = calculateChallanAmount(speed, vehicleType);
    challan.totalAmount = challan.amount;
    challan.issueDate = std::time(nullptr);
    challan.dueDate = challan.issueDate + (7 * 24 * 60 * 60);
    challan.paymentStatus = "Unpaid";
    challanBuffer.push_back(challan);
    sendChallanToUserPortal(challan);
    return challan;
}

void ChallanGenerator::sendChallanToUserPortal(const Challan& challan) {
    std::string challanData = "Challan ID: " + std::to_string(challan.challanId) +
                              ", Vehicle ID: " + std::to_string(challan.vehicleId) +
                              ", Speed: " + std::to_string(challan.speed) +
                              ", Vehicle Type: " + challan.vehicleType +
                              ", Amount: " + std::to_string(challan.totalAmount) +
                              ", Issue Date: " + std::ctime(&challan.issueDate) +
                              ", Due Date: " + std::ctime(&challan.dueDate) +
                              ", Status: " + challan.paymentStatus + "\n";
    if (write(pipeFd[1], challanData.c_str(), challanData.size()) == -1) {
        std::cerr << "Failed to write to pipe: " << strerror(errno) << "\n";
    }
}

void ChallanGenerator::updatePaymentStatus(int challanId, const std::string& status) {
    for (auto& challan : challanBuffer) {
        if (challan.challanId == challanId) {
            challan.paymentStatus = status;
            return;
        }
    }
}

void ChallanGenerator::displayChallanDetails(int challanId) const {
    for (const auto& challan : challanBuffer) {
        if (challan.challanId == challanId) {
            std::cout << "Challan ID: " << challan.challanId
                      << ", Vehicle ID: " << challan.vehicleId
                      << ", Speed: " << challan.speed
                      << ", Vehicle Type: " << challan.vehicleType
                      << ", Amount: " << challan.totalAmount
                      << ", Status: " << challan.paymentStatus
                      << ", Issue Date: " << std::ctime(&challan.issueDate)
                      << ", Due Date: " << std::ctime(&challan.dueDate) << "\n";
            return;
        }
    }
}

void ChallanGenerator::displayAllChallans() const {
    if (challanBuffer.empty()) {
        std::cout << "No challans in the buffer.\n";
        return;
    }
    for (const auto& challan : challanBuffer) {
        std::cout << "Challan ID: " << challan.challanId
                  << ", Vehicle ID: " << challan.vehicleId
                  << ", Speed: " << challan.speed
                  << ", Vehicle Type: " << challan.vehicleType
                  << ", Amount: " << challan.totalAmount
                  << ", Status: " << challan.paymentStatus
                  << ", Issue Date: " << std::ctime(&challan.issueDate)
                  << ", Due Date: " << std::ctime(&challan.dueDate) << "\n";
    }
}

int ChallanGenerator::getReadFd() const {
    return pipeFd[0];
}

void ChallanGenerator::processChallan(const Challan& challan) {
    sendChallanToUserPortal(challan);
}
