#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <numeric>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <stdexcept>

class Customer;

enum class Account_Type {STANDARD, SPECIAL, FOREIGN, BUSINESS};

class Account : public std::enable_shared_from_this<Account> {
    protected:
        unsigned id;
        std::string name; // Darf nicht leer sein.
        int dispo; // Muss > 0 sein und < 10 000.
        int balance; // Muss ≥ -dispo sein.
        std::map<unsigned, std::weak_ptr<Customer>> owners; // Diese Map darf nicht leer sein.
        static unsigned next_id; // Benötigt für die laufende Nummerierung von id.
    public:
        Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner);
        virtual ~Account() = default; // Default Destructor.
        virtual int withdraw(int x);
        int deposit(int x);
        unsigned get_id() const;
        int get_balance() const;
        std::string get_name() const;
        unsigned owner_count() const;
        bool share_account(std::shared_ptr<Customer> new_owner);
        bool remove_owner(unsigned id);
        virtual std::string additional_output() const = 0;
        friend std::ostream& operator<<(std::ostream& o, const Account& p);
};

class Standard_Account : public Account {
    public:
        Standard_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner);
        std::string additional_output() const;
};

class Special_Account : public Account {
    private:
        int fee; // > 0
    public:
        Special_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner, int fee);
        int withdraw(int x);
        std::string additional_output() const;
};

class Foreign_Currency_Account : public Account {
    private:
        std::string currency;
        std::string currency_symbol;
        double exchange_rate; // wechselkurs relativ zu €
    public:
        Foreign_Currency_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner, std::string currency = "Euro",
                                std::string currency_symbol = "€", double exchange_rate = 0);
        std::string additional_output() const;
};

class Business_Account : public Account {
    private:
        double percentage; // transaktionsgebühr prozentual
    public:
        Business_Account(std::string name, int dispo, int balance, std::shared_ptr<Customer> owner, double percentage);
        int withdraw(int x);
        std::string additional_output() const;
};

std::ostream& operator<<(std::ostream& o, Account_Type at);

#endif