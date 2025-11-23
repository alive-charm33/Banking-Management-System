#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

// Base class for all accounts
class Account {
protected:
    int accNo;                 // account number
    string name;               // account holder name
    double balance;            // balance amount
    vector<string> history;    // transaction history

public:
    // constructor
    Account(int a = 0, string n = "", double b = 0) {
        accNo = a;
        name = n;
        balance = b;
    }

    int getAccNo() { return accNo; }

    // pure virtual function (forces child class to define)
    virtual string getType() = 0;

    // deposit money
    virtual void deposit(double amt) {
        balance += amt;
        history.push_back("Deposited: " + to_string(amt));
        cout << "Amount deposited.\n";
    }

    // withdraw money
    virtual void withdraw(double amt) {
        if (amt > balance) {
            cout << "Insufficient balance!\n";
            history.push_back("Failed Withdraw: " + to_string(amt));
        } else {
            balance -= amt;
            history.push_back("Withdrawn: " + to_string(amt));
            cout << "Amount withdrawn.\n";
        }
    }

    // show basic details
    virtual void showDetails() {
        cout << "----------------------\n";
        cout << "Account No : " << accNo << "\n";
        cout << "Name       : " << name << "\n";
        cout << "Balance    : " << balance << "\n";
    }

    // show transaction history
    void showHistory() {
        cout << "\nTransaction History:\n";
        if (history.empty()) {
            cout << "No transactions.\n";
            return;
        }
        for (string h : history) cout << " - " << h << "\n";
    }

    // interest only for savings
    virtual void applyInterest() {}

    // save to file
    virtual void saveToFile(ofstream &f) {
        f << getType() << " " << accNo << " " << name << " " << balance << "\n";
        f << history.size() << "\n";
        for (auto &h : history) f << h << "\n";
    }

    virtual ~Account() {}
};

// Savings account class
class SavingsAccount : public Account {
public:
    SavingsAccount(int a, string n, double b) : Account(a, n, b) {}

    // add 3% interest
    void applyInterest() override {
        double interest = balance * 0.03;
        balance += interest;
        history.push_back("Interest added: " + to_string(interest));
    }

    string getType() override { return "SAV"; }

    void showDetails() override {
        cout << "(Savings Account)\n";
        Account::showDetails();
    }
};

// Current account class
class CurrentAccount : public Account {
    double overdraft;     // overdraft limit

public:
    CurrentAccount(int a, string n, double b, double od)
        : Account(a, n, b), overdraft(od) {}

    // withdrawal with overdraft support
    void withdraw(double amt) override {
        if (amt > balance + overdraft) {
            cout << "Overdraft limit exceeded!\n";
            history.push_back("Failed Withdraw: " + to_string(amt));
        } else {
            balance -= amt;
            history.push_back("Withdrawn (OD): " + to_string(amt));
            cout << "Withdraw successful.\n";
        }
    }

    string getType() override { return "CUR"; }

    void showDetails() override {
        cout << "(Current Account)\n";
        Account::showDetails();
        cout << "Overdraft Limit : " << overdraft << "\n";
    }

    // save overdraft also
    void saveToFile(ofstream &f) override {
        f << getType() << " " << accNo << " " << name << " " << balance << " " << overdraft << "\n";
        f << history.size() << "\n";
        for (auto &h : history) f << h << "\n";
    }
};

// Bank class for managing all accounts
class Bank {
    vector<Account*> accList;    // stores all accounts

public:
    // load previous data
    Bank() {
        loadFromFile();
    }

    // open new account
    void openAccount() {
        int type, acc;
        string n;
        double bal;

        cout << "\n1. Savings\n2. Current\nChoose type: ";
        cin >> type;

        cout << "Account no: ";
        cin >> acc;

        cin.ignore();
        cout << "Name: ";
        getline(cin, n);

        cout << "Opening balance: ";
        cin >> bal;

        if (type == 1)
            accList.push_back(new SavingsAccount(acc, n, bal));
        else {
            double limit;
            cout << "Overdraft limit: ";
            cin >> limit;
            accList.push_back(new CurrentAccount(acc, n, bal, limit));
        }

        cout << "Account created.\n";
    }

    // search account
    Account* search(int acc) {
        for (auto a : accList)
            if (a->getAccNo() == acc) return a;
        return NULL;
    }

    // deposit
    void depositMoney() {
        int acc;
        cout << "Account number: ";
        cin >> acc;

        Account* a = search(acc);
        if (!a) { cout << "Not found!\n"; return; }

        double amt;
        cout << "Amount: ";
        cin >> amt;

        a->deposit(amt);
    }

    // withdraw
    void withdrawMoney() {
        int acc;
        cout << "Account number: ";
        cin >> acc;

        Account* a = search(acc);
        if (!a) { cout << "Not found!\n"; return; }

        double amt;
        cout << "Amount: ";
        cin >> amt;

        a->withdraw(amt);
    }

    // show details
    void showAccount() {
        int acc;
        cout << "Account number: ";
        cin >> acc;

        Account* a = search(acc);
        if (!a) { cout << "Not found!\n"; return; }

        a->showDetails();
    }

    // show history
    void showHistory() {
        int acc;
        cout << "Account number: ";
        cin >> acc;

        Account* a = search(acc);
        if (!a) { cout << "Not found!\n"; return; }

        a->showHistory();
    }

    // add interest to all savings accounts
    void applyInterestAll() {
        for (auto a : accList)
            a->applyInterest();
        cout << "Interest applied.\n";
    }

    // save all data
    void saveToFile() {
        ofstream f("bankdata.txt");
        if (!f) return;

        f << accList.size() << "\n";
        for (auto a : accList) a->saveToFile(f);

        cout << "Data saved.\n";
    }

    // load data
    void loadFromFile() {
        ifstream f("bankdata.txt");
        if (!f) return;

        int count;
        f >> count;

        for (int i = 0; i < count; i++) {
            string type, name;
            int acc;
            double bal;

            f >> type;

            if (type == "SAV") {
                f >> acc >> name >> bal;
                auto *s = new SavingsAccount(acc, name, bal);

                int hcount;
                f >> hcount;
                f.ignore();
                string line;
                while (hcount--) getline(f, line);

                accList.push_back(s);
            }

            else if (type == "CUR") {
                double od;
                f >> acc >> name >> bal >> od;
                auto *c = new CurrentAccount(acc, name, bal, od);

                int hcount;
                f >> hcount;
                f.ignore();
                string line;
                while (hcount--) getline(f, line);

                accList.push_back(c);
            }
        }

        cout << "Data loaded.\n";
    }

    // destructor saves automatically
    ~Bank() {
        saveToFile();
        for (auto a : accList) delete a;
    }
};

// main menu
int main() {
    Bank b;
    int ch;

    while (true) {
        cout << "\n====== BANK MENU ======\n";
        cout << "1. Open Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Show Account\n";
        cout << "5. Show History\n";
        cout << "6. Apply Interest\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        cin >> ch;

        switch (ch) {
            case 1: b.openAccount(); break;
            case 2: b.depositMoney(); break;
            case 3: b.withdrawMoney(); break;
            case 4: b.showAccount(); break;
            case 5: b.showHistory(); break;
            case 6: b.applyInterestAll(); break;
            case 7: cout << "Goodbye!\n"; return 0;
            default: cout << "Invalid choice!\n";
        }
    }
}
