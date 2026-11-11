#include "bank.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

Bank::Bank(std::string name) : name{name} {
    if(name.empty())
        throw std::runtime_error("b constructor name empty");
}

const std::map<unsigned, std::shared_ptr<Customer>>& Bank::get_customers() const {
    // Retourniert customers
    return customers;
}

void Bank::create_customer(std::string name, std::string acc_name, int dispo, int balance, Account_Type type, int fee) {
    // Erstellt ein Customer-Objekt mit Account-Objekt und fügt das Objekt in customers ein.
    auto c = std::make_shared<Customer>(name);
    c->create_account(acc_name, dispo, balance, type, fee);
    customers.emplace(c->get_id(), c);
}

std::ostream& operator<<(std::ostream& o, const Bank& b) {
    // Format: [name, {customer0, ..., customern}]
    o << "[" << b.name << ", {";
    bool first = true;
    for(const auto& c : b.customers) {
        if(!c.second)
            continue;
        if(!first)
            o << ", ";
        o << *c.second;
        first = false;
    }
    return o << "}]";
}