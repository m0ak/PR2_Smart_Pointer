#include"bank_owner.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

Bank_Owner::Bank_Owner(std::string name) : name{name} {
    if(name.empty())
        throw std::runtime_error("bo constructor name empty");
}

bool Bank_Owner::create_bank(std::string name) {
    // Falls bank schon auf eine Bank verweist, ist false zu retournieren.
    // Ansonsten wird eine Bank erstellt, der Wert bank zugewiesen und true retourniert.
    if(bank)
        return false;
    bank = std::make_unique<Bank>(name);
    return true;
}

bool Bank_Owner::transfer_bank(Bank_Owner& target) {
    // Falls this über keine Bank verfügt oder target bereits über eine Bank verfügt, ist false zu retournieren.
    // Ansonsten wird bank zu target transferiert und true retourniert.
    if(!bank || target.bank)
        return false;
    target.bank = std::move(bank);
    return true;
}

const std::map<unsigned, std::shared_ptr<Customer>>& Bank_Owner::get_customers() const {
    // Retourniert customers der bank. Sollte es keine bank geben, ist eine Exception vom Typ runtime_error zu werfen.
    if(!bank)
        throw std::runtime_error("bo get_customers no bank");
    return bank->get_customers();
}

void Bank_Owner::create_customer(std::string name, std::string acc_name,int dispo, int balance, Account_Type type, int fee) {
    // Erstellt ein Customer-Objekt mit Account-Objekt und fügt das Objekt in customers von bank ein.
    // Sollte es keine bank geben, ist eine Exception vom Typ runtime_error zu werfen.
    if(!bank)
        throw std::runtime_error("bo create_customer bank == nullptr");
    bank->create_customer(name, acc_name, dispo, balance, type, fee);
}

std::ostream& operator<<(std::ostream& o, const Bank_Owner & bo) {
    // Format: [name, bank]
    o << "[" << bo.name;
    if(bo.bank)
        o << ", " << *bo.bank;
    return o << "]";
}