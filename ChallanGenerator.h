#ifndef CHALLANGENERATOR_H
#define CHALLANGENERATOR_H

#include <vector>
#include <string>
#include <ctime>

class ChallanGenerator {
public:
    struct Challan {
        int challanId;
        int vehicleId;
        float speed;
        std::string vehicleType;
        float amount;
        float totalAmount;
        std::time_t issueDate;
        std::time_t dueDate;
        std::string paymentStatus;
    };

    ChallanGenerator();
    ~ChallanGenerator();

    Challan createChallan(int vehicleId, float speed, const std::string& vehicleType);
    void updatePaymentStatus(int challanId, const std::string& status);
    void displayChallanDetails(int challanId) const;
    void displayAllChallans() const;
    int getReadFd() const;
    void sendChallanToUserPortal(const Challan& challan);
    void processChallan(const Challan& challan);

private:
    std::vector<Challan> challanBuffer;
    int pipeFd[2];
    int currentId;

    int generateUniqueId();
    float calculateChallanAmount(float speed, const std::string& type);
};

#endif
