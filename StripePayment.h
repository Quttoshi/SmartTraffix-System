#ifndef STRIPE_PAYMENT_H
#define STRIPE_PAYMENT_H

#include <string>

class StripePayment {
public:
    void processPayment(int challanId, int vehicleId, const std::string& vehicleType, float amountToPay);
};

#endif
