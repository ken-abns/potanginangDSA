#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
using namespace std;

// acc struct
struct Account {
    string accNo;      // 5-digit account number
    string name;
    string birthday;
    string contact;
    double balance;
    string pin;        // encrypted PIN
};

//node
struct Node {
    Account data;
    Node *next;
    Node *prev;
    Node(Account x) {
        data = x;
        next = prev = NULL;
    }
};

// Encryption
string encryptPin(string pin) {
    string enc = pin;
    for (char &c : enc) c = c + 3; //caesar shift
    return enc;
}

string decryptPin(string enc) {
    string pin = enc;
    for (char &c : pin) c = c - 3;
    return pin;
}

// ATM Class
class ATMList {
private:
    Node *head, *tail;
public:
    ATMList(): head(NULL), tail(NULL) {}
    ~ATMList() {
        Node *p;
        while (head != NULL) {
            p = head;
            head = head->next;
            delete p;
        }
    }

    void registerAccount(Account x);
    Node* findAccount(string accNo);
    bool verifyPin(Node* acc, string pin);

    void balanceInquiry(string accNo, string pin);
    void withdraw(string accNo, string pin, double amount);
    void deposit(string accNo, string pin, double amount);
    void fundTransfer(string fromAcc, string toAcc, string pin, double amount);
    void changePin(string accNo, string oldPin, string newPin);

    void saveToCard();
    void loadFromCard();
};

// reg
void ATMList::registerAccount(Account x) {
    Node *newNode = new Node(x);
    if (head == NULL) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    cout << "Account Registered Successfully!\n";
}

// find acc
Node* ATMList::findAccount(string accNo) {
    Node *p = head;
    while (p != NULL) {
        if (p->data.accNo == accNo) return p;
        p = p->next;
    }
    return NULL;
}

// verifying pass
bool ATMList::verifyPin(Node* acc, string pin) {
    return decryptPin(acc->data.pin) == pin;
}

// balance func
void ATMList::balanceInquiry(string accNo, string pin) {
    Node* acc = findAccount(accNo);
    if (!acc || !verifyPin(acc, pin)) {
        cout << "Invalid Account or PIN!\n"; return;
    }
    cout << "Balance: " << fixed << setprecision(2) << acc->data.balance << endl;
}

// withdraw func
void ATMList::withdraw(string accNo, string pin, double amount) {
    Node* acc = findAccount(accNo);
    if (!acc || !verifyPin(acc, pin)) {
        cout << "Invalid Account or PIN!\n"; return;
    }
    if (amount > acc->data.balance) {
        cout << "Insufficient Balance!\n"; return;
    }
    acc->data.balance -= amount;
    cout << "Withdraw Successful! New Balance: " << acc->data.balance << endl;
}
// deposit func
void ATMList::deposit(string accNo, string pin, double amount) {
    Node* acc = findAccount(accNo);
    if (!acc || !verifyPin(acc, pin)) {
        cout << "Invalid Account or PIN!\n"; return;
    }
    acc->data.balance += amount;
    cout << "Deposit Successful! New Balance: " << acc->data.balance << endl;
}
// transfer
void ATMList::fundTransfer(string fromAcc, string toAcc, string pin, double amount) {
    Node* from = findAccount(fromAcc);
    Node* to = findAccount(toAcc);
    if (!from || !to || !verifyPin(from, pin)) {
        cout << "Invalid Account or PIN!\n"; return;
    }
    if (amount > from->data.balance) {
        cout << "Insufficient Balance!\n"; return;
    }
    from->data.balance -= amount;
    to->data.balance += amount;
    cout << "Transfer Successful!\n";
}
// changing pin
void ATMList::changePin(string accNo, string oldPin, string newPin) {
    Node* acc = findAccount(accNo);
    if (!acc || !verifyPin(acc, oldPin)) {
        cout << "Invalid Account or PIN!\n"; return;
    }
    acc->data.pin = encryptPin(newPin);
    cout << "PIN Changed Successfully!\n";
}

// file operation
// save func
void ATMList::saveToCard() {
    ofstream file("atmcard.csv");
    Node *p = head;
    while (p != NULL) {
        file << p->data.accNo << "," << p->data.name << "," << p->data.birthday << ","
             << p->data.contact << "," << p->data.balance << "," << p->data.pin << endl;
        p = p->next;
    }
    file.close();
    cout << "Accounts Saved to Flash Drive!\n";
}
// load func
void ATMList::loadFromCard() {
    ifstream file("atmcard.csv");
    if (!file) return;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        Account acc;
        getline(ss, acc.accNo, ',');
        getline(ss, acc.name, ',');
        getline(ss, acc.birthday, ',');
        getline(ss, acc.contact, ',');
        ss >> acc.balance;
        ss.ignore();
        getline(ss, acc.pin, ',');
        registerAccount(acc);
    }
    file.close();
}

// checking if already have USB which is the card
bool checkCardInserted(const string &path) {
    ifstream file(path);
    if (!file) {
        cout << "Please insert card." << endl;
        return false;
    }
    cout << "Card detected!" << endl;
    return true;
}

// main menu
int menu() {
    int choice;
    cout << "\nATM System\n";
    cout << "1. Register Account\n";
    cout << "2. Balance Inquiry\n";
    cout << "3. Withdraw\n";
    cout << "4. Deposit\n";
    cout << "5. Fund Transfer\n";
    cout << "6. Change PIN\n";
    cout << "7. Exit\n";
    cout << "Enter choice: ";
    cin >> choice;
    return choice;
}

// main function
int main() {
    ATMList atm;

    // Path to flash drive card file
    string cardPath = "E:/pin.code";  

    // Checking if card is inserted
    if (!checkCardInserted(cardPath)) {
        return 0; // Exit if no card
    }

    // load acc from USB
    atm.loadFromCard();

    while (true) {
        switch(menu()) {
            case 1: { // reg acc - line 72
                Account acc;
                cout << "Enter Account No (5 digits): "; cin >> acc.accNo;
                cout << "Enter Name: "; cin >> acc.name;
                cout << "Enter Birthday: "; cin >> acc.birthday;
                cout << "Enter Contact: "; cin >> acc.contact;
                cout << "Enter Initial Deposit (min 5000): "; cin >> acc.balance;
                if (acc.balance < 5000) { cout << "Minimum deposit is 5000!\n"; break; }
                string pin;
                cout << "Set PIN: "; cin >> pin;
                acc.pin = encryptPin(pin);

                // saving file - line 153
                ofstream cardFile(cardPath);
                cardFile << acc.accNo << "," << acc.pin << endl;
                cardFile.close();

                atm.registerAccount(acc);
                atm.saveToCard();
                break;
            }
            case 2: { // balance inquiry func - line 100
                string accNo, pin;
                cout << "Enter Account No: "; cin >> accNo;
                cout << "Enter PIN: "; cin >> pin;
                atm.balanceInquiry(accNo, pin);
                break;
            }
            case 3: { // withdraw func - line 109
                string accNo, pin; double amt;
                cout << "Enter Account No: "; cin >> accNo;
                cout << "Enter PIN: "; cin >> pin;
                cout << "Enter Amount: "; cin >> amt;
                atm.withdraw(accNo, pin, amt);
                atm.saveToCard();
                break;
            }
            case 4: { // deposite func - line 120
                string accNo, pin; double amt;
                cout << "Enter Account No: "; cin >> accNo;
                cout << "Enter PIN: "; cin >> pin;
                cout << "Enter Amount: "; cin >> amt;
                atm.deposit(accNo, pin, amt);
                atm.saveToCard();
                break;
            }
            case 5: { // fund transfer - line 130
                string fromAcc, toAcc, pin; double amt;
                cout << "Enter From Account No: "; cin >> fromAcc;
                cout << "Enter PIN: "; cin >> pin;
                cout << "Enter To Account No: "; cin >> toAcc;
                cout << "Enter Amount: "; cin >> amt;
                atm.fundTransfer(fromAcc, toAcc, pin, amt);
                atm.saveToCard();
                break;
            }
            case 6: { // changing pin - line 144
                string accNo, oldPin, newPin;
                cout << "Enter Account No: "; cin >> accNo;
                cout << "Enter Old PIN: "; cin >> oldPin;
                cout << "Enter New PIN: "; cin >> newPin;
                atm.changePin(accNo, oldPin, newPin);
                atm.saveToCard();
                break;
            }
            case 7: { // exit
                cout << "Thank you for using the ATM. Goodbye!\n";
                return 0;
            }
            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
    }

    return 0;
}