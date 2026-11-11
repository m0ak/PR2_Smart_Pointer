#ifndef BANK_H
#define BANK_H

#include "bank_customer.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

class Bank {
    private:
        std::string name; // Darf nicht leer sein.
        std::map<unsigned, std::shared_ptr<Customer>> customers; // Zu Beginn leer.
    public:
        Bank(std::string name);
        const std::map<unsigned, std::shared_ptr<Customer>>& get_customers() const;
        void create_customer(std::string name, std::string acc_name, int dispo, int balance, Account_Type = Account_Type::STANDARD, int fee = 0);
        friend std::ostream& operator<<(std::ostream& o, const Bank& b);
};

#endif