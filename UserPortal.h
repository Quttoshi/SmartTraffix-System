#ifndef USER_PORTAL_H
#define USER_PORTAL_H

#include "ChallanGenerator.h"
#include <vector>

class UserPortal {
public:
    void addChallan(const ChallanGenerator::Challan& challan);
    void displayChallanDetails(int challanId) const;

private:
    std::vector<ChallanGenerator::Challan> challanBuffer;
};

#endif
