#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>
#include <cstdio>

using namespace std;

class BankAccount
{
private:
    long long accountNumber;
    string name;
    double balance;

public:
    BankAccount() : accountNumber(0), balance(0.0) {}

    BankAccount(long long accNo, string accName, double accBalance)
        : accountNumber(accNo), name(accName), balance(accBalance) {}

    long long getAccountNumber() const
    {
        return accountNumber;
    }

    void input()
    {
        cout << "\n--------------------------------------------\n";
        cout << "           CREATE NEW ACCOUNT\n";
        cout << "--------------------------------------------\n";

        cout << "Enter Account Number : ";
        cin >> accountNumber;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Customer Name  : ";
        getline(cin, name);

        cout << "Enter Initial Balance: Rs. ";
        cin >> balance;

        if (balance < 0)
        {
            balance = 0;
            cout << "\n[!] Initial balance cannot be negative.\n";
            cout << "[!] Balance has been set to Rs. 0.00\n";
        }
    }

    void display() const
    {
        cout << left
             << setw(18) << accountNumber
             << setw(25) << name
             << "Rs. " << fixed << setprecision(2) << balance
             << endl;
    }

    void displayDetails() const
    {
        cout << "\n============================================\n";
        cout << "             ACCOUNT DETAILS\n";
        cout << "============================================\n";

        cout << left;
        cout << setw(20) << "Account Number" << ": " << accountNumber << endl;
        cout << setw(20) << "Customer Name" << ": " << name << endl;
        cout << setw(20) << "Current Balance" << ": Rs. "
             << fixed << setprecision(2) << balance << endl;

        cout << "============================================\n";
    }

    bool deposit(double amount)
    {
        if (amount <= 0)
            return false;

        balance += amount;
        return true;
    }

    bool withdraw(double amount)
    {
        if (amount <= 0 || amount > balance)
            return false;

        balance -= amount;
        return true;
    }

    void save(ofstream &file) const
    {
        file << accountNumber << '\n';
        file << name << '\n';
        file << balance << '\n';
    }

    bool load(ifstream &file)
    {
        if (!(file >> accountNumber))
            return false;

        file.ignore(numeric_limits<streamsize>::max(), '\n');

        getline(file, name);

        file >> balance;
        file.ignore(numeric_limits<streamsize>::max(), '\n');

        return true;
    }
};

const string FILE_NAME = "accounts.dat";

// --------------------------------------------------
// CREATE ACCOUNT
// --------------------------------------------------

void addAccount()
{
    BankAccount account;
    account.input();

    ifstream checkFile(FILE_NAME);
    BankAccount existing;

    while (existing.load(checkFile))
    {
        if (existing.getAccountNumber() == account.getAccountNumber())
        {
            cout << "\n[ERROR] An account with this number already exists.\n";
            return;
        }
    }

    checkFile.close();

    ofstream file(FILE_NAME, ios::app);

    if (!file)
    {
        cout << "\n[ERROR] Unable to open account file.\n";
        return;
    }

    account.save(file);
    file.close();

    cout << "\n[SUCCESS] Account created successfully!\n";
}

// --------------------------------------------------
// DISPLAY ACCOUNTS
// --------------------------------------------------

void displayAccounts()
{
    ifstream file(FILE_NAME);

    if (!file)
    {
        cout << "\n[INFO] No account records found.\n";
        return;
    }

    BankAccount account;
    bool found = false;

    cout << "\n============================================================\n";
    cout << "                    ALL BANK ACCOUNTS\n";
    cout << "============================================================\n";

    cout << left
         << setw(18) << "Account Number"
         << setw(25) << "Customer Name"
         << "Balance" << endl;

    cout << "------------------------------------------------------------\n";

    while (account.load(file))
    {
        account.display();
        found = true;
    }

    file.close();

    if (!found)
        cout << "No account records found.\n";

    cout << "============================================================\n";
}

// --------------------------------------------------
// FIND ACCOUNT
// --------------------------------------------------

bool findAccount(long long accountNumber, BankAccount &result)
{
    ifstream file(FILE_NAME);

    if (!file)
        return false;

    BankAccount account;

    while (account.load(file))
    {
        if (account.getAccountNumber() == accountNumber)
        {
            result = account;
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// --------------------------------------------------
// CHECK BALANCE
// --------------------------------------------------

void checkBalance()
{
    long long accountNumber;

    cout << "\n--------------------------------------------\n";
    cout << "              CHECK BALANCE\n";
    cout << "--------------------------------------------\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    BankAccount account;

    if (findAccount(accountNumber, account))
    {
        account.displayDetails();
    }
    else
    {
        cout << "\n[ERROR] Account not found.\n";
    }
}

// --------------------------------------------------
// DEPOSIT MONEY
// --------------------------------------------------

void depositMoney()
{
    long long accountNumber;
    double amount;

    cout << "\n--------------------------------------------\n";
    cout << "               DEPOSIT MONEY\n";
    cout << "--------------------------------------------\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    ifstream file(FILE_NAME);

    if (!file)
    {
        cout << "\n[ERROR] No account records found.\n";
        return;
    }

    ofstream tempFile("temp.dat");

    if (!tempFile)
    {
        cout << "\n[ERROR] Unable to create temporary file.\n";
        return;
    }

    BankAccount account;
    bool found = false;
    bool success = false;

    while (account.load(file))
    {
        if (account.getAccountNumber() == accountNumber)
        {
            found = true;

            cout << "Enter Deposit Amount : Rs. ";
            cin >> amount;

            if (account.deposit(amount))
            {
                success = true;
                cout << "\n[SUCCESS] Rs. "
                     << fixed << setprecision(2) << amount
                     << " deposited successfully!\n";
            }
            else
            {
                cout << "\n[ERROR] Invalid deposit amount.\n";
            }
        }

        account.save(tempFile);
    }

    file.close();
    tempFile.close();

    remove(FILE_NAME.c_str());
    rename("temp.dat", FILE_NAME.c_str());

    if (!found)
        cout << "\n[ERROR] Account not found.\n";

    if (success)
    {
        BankAccount updatedAccount;

        if (findAccount(accountNumber, updatedAccount))
        {
            cout << "New Balance          : Rs. "
                 << fixed << setprecision(2) << amount
                 << " added to account.\n";
        }
    }
}

// --------------------------------------------------
// WITHDRAW MONEY
// --------------------------------------------------

void withdrawMoney()
{
    long long accountNumber;
    double amount;

    cout << "\n--------------------------------------------\n";
    cout << "              WITHDRAW MONEY\n";
    cout << "--------------------------------------------\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    ifstream file(FILE_NAME);

    if (!file)
    {
        cout << "\n[ERROR] No account records found.\n";
        return;
    }

    ofstream tempFile("temp.dat");

    if (!tempFile)
    {
        cout << "\n[ERROR] Unable to create temporary file.\n";
        return;
    }

    BankAccount account;
    bool found = false;
    bool success = false;

    while (account.load(file))
    {
        if (account.getAccountNumber() == accountNumber)
        {
            found = true;

            cout << "Enter Withdrawal Amount: Rs. ";
            cin >> amount;

            if (account.withdraw(amount))
            {
                success = true;

                cout << "\n[SUCCESS] Rs. "
                     << fixed << setprecision(2) << amount
                     << " withdrawn successfully!\n";
            }
            else
            {
                cout << "\n[ERROR] Invalid amount or insufficient balance.\n";
            }
        }

        account.save(tempFile);
    }

    file.close();
    tempFile.close();

    remove(FILE_NAME.c_str());
    rename("temp.dat", FILE_NAME.c_str());

    if (!found)
        cout << "\n[ERROR] Account not found.\n";
}

// --------------------------------------------------
// MAIN MENU
// --------------------------------------------------

void displayMenu()
{
    cout << "\n\n";
    cout << "============================================\n";
    cout << "           BANK MANAGEMENT SYSTEM\n";
    cout << "============================================\n";
    cout << "  1. Create Account\n";
    cout << "  2. Display All Accounts\n";
    cout << "  3. Check Balance\n";
    cout << "  4. Deposit Money\n";
    cout << "  5. Withdraw Money\n";
    cout << "  6. Exit\n";
    cout << "============================================\n";
    cout << "  Enter your choice: ";
}

// --------------------------------------------------
// MAIN FUNCTION
// --------------------------------------------------

int main()
{
    int choice;

    cout << "\n";
    cout << "============================================\n";
    cout << "        WELCOME TO BANK MANAGEMENT\n";
    cout << "============================================\n";

    do
    {
        displayMenu();
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "\n[ERROR] Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            addAccount();
            break;

        case 2:
            displayAccounts();
            break;

        case 3:
            checkBalance();
            break;

        case 4:
            depositMoney();
            break;

        case 5:
            withdrawMoney();
            break;

        case 6:
            cout << "\n============================================\n";
            cout << "       Thank you for using our bank!\n";
            cout << "============================================\n";
            break;

        default:
            cout << "\n[ERROR] Invalid choice. Please select 1-6.\n";
        }

    } while (choice != 6);

    return 0;
}