#include "bank_customer.h"
#include "bank_account.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

unsigned Customer::next_id = 0;

Customer::Customer(std::string name) : name{name} {
    // Erstellt Objekt.
    if(name.empty())
        throw std::runtime_error("bc constructor name empty");
    this->id = next_id++;
}

unsigned Customer::create_account(std::string acc_name, int dispo, int balance, Account_Type type, int fee) {
    // Es wird ein Konto mit passendem Typ für das Objekt angelegt und in accounts hinterlegt.
    // Retourniert wird die id des Account-Objekts.
    std::shared_ptr<Account> acc;
    if(type == Account_Type::STANDARD) {
        acc = std::make_shared<Standard_Account>(acc_name, dispo, balance, shared_from_this());
    } else if (type == Account_Type::SPECIAL) {
        acc = std::make_shared<Special_Account>(acc_name, dispo, balance, shared_from_this(), fee);
    } else {
        throw std::runtime_error("bc create_account invalid Account_Type");
    }
    unsigned acc_id = acc->get_id();
    accounts.emplace(acc_id, acc);
    return acc_id;
}

int Customer::total_balance() const {
    // Retourniert die Summe über alle accounts.
    int sum = 0;
    for(const auto& a : accounts)
        sum += a.second->get_balance();
    return sum;
}

unsigned Customer::get_id() const {
    // Retourniert id.
    return id;
}

const std::string& Customer::get_name() const {
    // Retourniert name.
    return name;
}

const std::map<unsigned, std::shared_ptr<Account>>& Customer::get_accounts() const {
    // Retourniert accounts.
    return accounts;
}

bool Customer::share_account(unsigned aid, std::shared_ptr<Customer> new_owner) {
    // Falls this über Account mit id aid verfügt, wird versucht, den Account mit new_owner zu teilen.
    // Bei Erfolg wird der Account auch bei new_owner eingetragen und true geliefert, ansonsten false.
    auto it = accounts.find(aid);
    if(it == accounts.end())
        return false;
    return it->second->share_account(new_owner);
}

bool Customer::transfer(int amount, unsigned source_id, std::shared_ptr<Customer> target, unsigned target_id) {
    // Falls this über Account mit id source_id verfügt und target über den Account mit id target_id verfügt,
    // wird versucht balance von source nach target zu überweisen. Sollten target_id und source_id gleich sein,
    // ist eine Exception vom Typ runtime_error zu werfen. Bei Erfolg wird true geliefert, ansonsten false.
    if(!target)
        return false;
    if(source_id == target_id)
        throw std::runtime_error("bc transfer source_it == target_it");
    auto it_s = this->accounts.find(source_id);
    auto it_t = target->accounts.find(target_id);
    if(it_s == this->accounts.end() || it_t == target->accounts.end())
        return false;
    if(amount <= 0)
        return false;
    try {
        it_s->second->withdraw(amount);
        it_t->second->deposit(amount);
    } catch(std::runtime_error&) {
        return false;
    }
    return true;
}

void Customer::add_account(unsigned aid, std::shared_ptr<Account> acc) {
    // Prüfung, ob Account bereits existiert erfolgt in Customer::share_account
    accounts.emplace(aid, acc);
}

bool Customer::remove_account(unsigned aid) {
    return accounts.erase(aid) > 0;
}

std::ostream& operator<<(std::ostream& o, const Customer& p) {
    // Es soll nur [name,owner_count()] von einem account ausgegeben werden.
    // Format: [name, {account0, ..., accountn}, total_balance()]
    o << "[" << p.get_name() << ", {";
    bool first = true;
    for(const auto& acc : p.accounts) {
        if(!acc.second)
            continue;
        if(!first)
            o << ", ";
        o << "[" << acc.second->get_name() << ", " << acc.second->owner_count() << "]";
        first = false;
    }
    return o << "}, " << p.total_balance() << "]";
}