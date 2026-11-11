#include "bank_account.h"
#include "bank_customer.h"
#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

unsigned Account::next_id = 0;

Account::Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner) : name{name}, dispo{dispo}, balance{balance} {
    // owner soll in die Map owners hinzugefügt werden.
    if(this->name.empty())
        throw std::runtime_error("ba constructor name empty");
    if(this->dispo <= 0 || this->dispo >= 10000)
        throw std::runtime_error("ba constructor dispo not in range");
    if(this->balance < -dispo)
        throw std::runtime_error("ba constructor balance error");
    if(!owner)
        throw std::runtime_error("ba constructor owner == nullptr");
    id = next_id++;
    owners.emplace(owner->get_id(), owner);
}

int Account::withdraw(int x) {
    // balance soll um x reduziert werden sofern balance größer oder gleich -dispo danach wäre.
    // Falls dies nicht möglich ist oder x ≤ 0 ist, ist eine Exception vom Typ runtime_error zu werfen.
    // Es wird balance retourniert.
    if((balance-x) < -dispo)
        throw std::runtime_error("ba withdraw balance too small");
    if(x <= 0)
        throw std::runtime_error("ba withdraw x negative");
    balance -= x;
    return balance;
}

int Account::deposit(int x) {
    // Sollte x ≤ 0 sein ist eine Exception vom Typ runtime_error zu werfen,
    // ansonsten wird balance um x erhöht. Es wird balance retourniert.
    if(x <= 0)
        throw std::runtime_error("ba deposit x negative");
    balance += x;
    return balance;
}

unsigned Account::get_id() const {
    // Liefert id.
    return id;
}

int Account::get_balance() const {
    return balance;
}

std::string Account::get_name() const {
    return name;
}

unsigned Account::owner_count() const {
    // Liefert die Anzahl der nicht expired owners.
    unsigned count = 0;
    for(const auto& o : owners) {
        if(!o.second.expired())
            ++count;
    }
    return count;  
}

bool Account::share_account(std::shared_ptr<Customer> new_owner) {
    // Fügt new_owner zu owners hinzu, falls möglich und das this-Objekt zu den Accounts von new_owner.
    // Bei Erfolg wird true geliefert, ansonsten false.
    if(!new_owner)
        return false;
    if(owners.count(new_owner->get_id()) > 0)
        return false;
    owners.emplace(new_owner->get_id(), new_owner);
    new_owner->add_account(id, shared_from_this());
    return true;
}

bool Account::remove_owner(unsigned id) {
    // Entfernt owner mit der Id id aus owners, sofern möglich und das this-Objekt aus den Accounts von owner.
    // owners darf nie leer sein. Sollte es nicht möglich sein, ist false zu retournieren, ansonsten true.
    if(owner_count() <= 1)
        return false;
    auto it = owners.find(id);
    if(it == owners.end())
        return false;
    auto o_ptr = it->second.lock();
    if(!o_ptr) {
        owners.erase(it);
        return false;
    }
    if(!o_ptr->remove_account(this->id))
        return false;
    owners.erase(it);
    return true;
}

std::ostream& operator<<(std::ostream& o, const Account& p) {
    // Es soll nur [name, total_balance] von einem owner ausgegeben werden.
    // Format: [name, additional_output(), balance, dispo, {owner0, ..., ownern}, owner_count()]
    o << "[" << p.name << ", " << p.additional_output() << ", " << p.balance << ", " << p.dispo << ", {";
    bool first = true;
    for(const auto& own : p.owners) {
        auto own_sptr = own.second.lock();
        if(!own_sptr)
            continue;
        if(!first)
            o << ", ";
        o << "[" << own_sptr->get_name() << ", " << own_sptr->total_balance() << "]";
        first = false;
    }
    return o << "}, " << p.owner_count() << "]";
}

Standard_Account::Standard_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner) : Account(name, dispo, balance, owner) {}

std::string Standard_Account::additional_output() const {
    // Liefert “Standard”
    return "Standard";
}

Special_Account::Special_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner,int fee) : Account(name, dispo, balance, owner), fee{fee} {
    if(fee <= 0) {
        --next_id;
        throw std::runtime_error("ba sa constructor fee too low");
    }
}

int Special_Account::withdraw(int x) {
    // balance soll um x+fee reduziert werden sofern balance größer oder gleich -dispo danach wäre.
    // Falls dies nicht möglich ist oder x ≤ 0 ist, ist eine Exception vom Typ runtime_error zu werfen.
    // Es wird balance retourniert.
    if((balance-(x+fee)) < -dispo)
        throw std::runtime_error("ba withdraw balance too small");
    if(x <= 0)
        throw std::runtime_error("ba withdraw x negative");
    balance -= (x+fee);
    return balance;
}

std::string Special_Account::additional_output() const {
    // Liefert “Special, fee”
    return "Special, " + std::to_string(fee);
}

Foreign_Currency_Account::Foreign_Currency_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner, std::string currency,
                                std::string currency_symbol, double exchange_rate) : Account(name, dispo*exchange_rate, balance*exchange_rate, owner),
                                currency{currency}, currency_symbol{currency_symbol}, exchange_rate{exchange_rate} {
    if(exchange_rate <= 0) {
        throw std::runtime_error("Foreign_Currency_Account constructor exchange rate <= 0");
    }
}

std::string Foreign_Currency_Account::additional_output() const {
    return "Foreign, " + currency + ", " + currency_symbol + ", " + std::to_string(exchange_rate);
}

Business_Account::Business_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner, double percentage) : Account(name, dispo, balance, owner), percentage{percentage/100} {
    if(percentage < 0 || percentage > 1)
        throw std::runtime_error("Business_Account constructor percentage < 0 || > 1");
}

int Business_Account::withdraw(int x) {
    if((balance-(x * (1 + percentage))) < -dispo)
        throw std::runtime_error("business_acc withdraw balance too small");
    if(x <= 0)
        throw std::runtime_error("business_acc withdraw x negative");
    balance -= (x * (1 + percentage));
    return balance;
}

std::string Business_Account::additional_output() const {
    return "Business, " + std::to_string(percentage);
}

std::ostream& operator<<(std::ostream& o, Account_Type at) {
    // Die Ausgabe von at muss der üblichen Schreibweise von Nomen angepasst werden,
    // z.B. STANDARD sollte ausgegeben werden als Standard.
    if(at == Account_Type::STANDARD)
        return o << "Standard";
    if(at == Account_Type::SPECIAL)
        return o << "Special";
    return o;
}