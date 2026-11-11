#ifndef BANK_OWNER_H
#define BANK_OWNER_H

#include"bank.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

class Bank_Owner {
    private:
        std::string name; // Darf nicht leer sein.
        std::unique_ptr<Bank> bank;
    public:
        Bank_Owner(std::string name);
        bool create_bank(std::string name);
        bool transfer_bank(Bank_Owner& target);
        const std::map<unsigned, std::shared_ptr<Customer>>& get_customers() const;
        void create_customer(std::string name, std::string acc_name,int dispo, int balance, Account_Type = Account_Type::STANDARD, int fee = 0);
        friend std::ostream& operator<<(std::ostream& o, const Bank_Owner & bo);
};

#endif