#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "bank_account.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

class Customer : public std::enable_shared_from_this<Customer> {
    private:
        unsigned id;
        std::string name; // Darf nicht leer sein.
        std::map<unsigned, std::shared_ptr<Account>> accounts; // Diese Map darf leer sein.
        static unsigned next_id; // Benötigt für die laufende Nummerierung von id.
    public:
        Customer(std::string name);
        unsigned create_account(std::string acc_name, int dispo, int balance, Account_Type = Account_Type::STANDARD, int fee = 0);
        int total_balance() const;
        unsigned get_id() const;
        const std::string& get_name() const;
        const std::map<unsigned, std::shared_ptr<Account>>& get_accounts() const;
        bool share_account(unsigned aid, std::shared_ptr<Customer> new_owner);
        bool transfer(int amount, unsigned source_id, std::shared_ptr<Customer> target, unsigned target_id);
        void add_account(unsigned id, std::shared_ptr<Account> acc);
        bool remove_account(unsigned aid);
        friend std::ostream& operator<<(std::ostream& o, const Customer& p);
};

#endif