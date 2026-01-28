#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

// -------------------- PACKAGE LINKED LIST --------------------

struct Package {
    string name;
    string type;
    double cost;
    int gb;
    int minutes;
    int offnetMinutes;
    int sms;
    Package* next;
};

// Insert package at end of linked list
void insertPackage(Package*& head, string name, string type, double cost, int gb, int minutes, int offnet, int sms) {
    Package* newPkg = new Package{name, type, cost, gb, minutes, offnet, sms, nullptr};
    if (!head) {
        head = newPkg;
        return;
    }
    Package* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newPkg;
}

// -------------------- MERGE SORT --------------------

Package* mergeLists(Package* a, Package* b) {
    if (!a) return b;
    if (!b) return a;
    Package* result = nullptr;
    if (a->cost < b->cost) {
        result = a;
        result->next = mergeLists(a->next, b);
    } else {
        result = b;
        result->next = mergeLists(a, b->next);
    }
    return result;
}

void splitList(Package* source, Package*& front, Package*& back) {
    Package* slow = source;
    Package* fast = source->next;
    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    front = source;
    back = slow->next;
    slow->next = nullptr;
}

void mergeSort(Package*& head) {
    if (!head || !head->next) return;
    Package* a; Package* b;
    splitList(head, a, b);
    mergeSort(a);
    mergeSort(b);
    head = mergeLists(a, b);
}

// -------------------- ACCOUNT --------------------

struct Account {
    string phoneNumber;
    double balance = 0.0;
    string activePackage = "None";
    int gb = 0;
    int minutes = 0;
    int offnetMinutes = 0;
    int sms = 0;
};

// -------------------- SAFE CONVERSIONS --------------------

int toInt(const string& s) { return stoi(s); }
double toDouble(const string& s) { return stod(s); }

// -------------------- FILE HANDLING FOR ACCOUNTS --------------------

void saveAccount(const Account& acc) {
    string filename = acc.phoneNumber + "_account.txt";
    ofstream f(filename);
    f << "Number: " << acc.phoneNumber << "\n";
    f << "Balance=" << acc.balance << "\n";
    f << "Package=" << acc.activePackage << "\n";
    f << "GB=" << acc.gb << "\n";
    f << "Minutes=" << acc.minutes << "\n";
    f << "Offnet=" << acc.offnetMinutes << "\n";
    f << "SMS=" << acc.sms << "\n";
    f.close();
}

void loadAccount(Account& acc) {
    string filename = acc.phoneNumber + "_account.txt";
    ifstream f(filename);
    if (!f.is_open()) {
        cout << "No previous record found. Creating new account.\n";
        return;
    }
    string line;
    getline(f, line); acc.phoneNumber = line.substr(line.find(": ") + 2);
    getline(f, line); acc.balance = toDouble(line.substr(line.find("=") + 1));
    getline(f, line); acc.activePackage = line.substr(line.find("=") + 1);
    getline(f, line); acc.gb = toInt(line.substr(line.find("=") + 1));
    getline(f, line); acc.minutes = toInt(line.substr(line.find("=") + 1));
    getline(f, line); acc.offnetMinutes = toInt(line.substr(line.find("=") + 1));
    getline(f, line); acc.sms = toInt(line.substr(line.find("=") + 1));
    cout << "Account loaded successfully!\n";
    f.close();
}

// -------------------- CUSTOMER SUPPORT (PER ACCOUNT FILE) --------------------

void addSupportMessage(const string& phoneNumber) {
    cout << "Enter your complaint/message: ";
    string msg;
    cin.ignore();
    getline(cin, msg);

    string filename = phoneNumber + "_complaints.txt";
    ofstream f(filename, ios::app); // append mode
    f << msg << "\n";
    f.close();

    cout << "Your message has been added to your support queue.\n";
}

void processSupport(const string& phoneNumber) {
    string filename = phoneNumber + "_complaints.txt";
    ifstream f(filename);
    if (!f.is_open()) {
        cout << "No pending support requests.\n";
        return;
    }

    queue<string> tempQueue;
    string line;
    while (getline(f, line)) tempQueue.push(line);
    f.close();

    if (tempQueue.empty()) {
        cout << "No pending support requests.\n";
        return;
    }

    cout << "Processing: " << tempQueue.front() << "\n";
    tempQueue.pop();

    // Rewrite the remaining complaints back to the file
    ofstream fout(filename);
    while (!tempQueue.empty()) {
        fout << tempQueue.front() << "\n";
        tempQueue.pop();
    }
    fout.close();
}

void customerSupportMenu(const string& phoneNumber) {
    cout << "\n1. Submit Complaint\n2. Process Complaint\nChoose: ";
    int c;
    cin >> c;
    if (c == 1) addSupportMessage(phoneNumber);
    else if (c == 2) processSupport(phoneNumber);
    else cout << "Invalid.\n";
}

// -------------------- PHONE VALIDATION --------------------

string getValidPhoneNumber() {
    string num;
    while (true) {
        cout << "Enter phone number (11 digits, starts with 03): ";
        cin >> num;
        if (num.length() == 11 && num.substr(0, 2) == "03") return num;
        cout << "? Invalid phone number. Try again.\n";
    }
}

// -------------------- MAIN MENU FUNCTIONS --------------------

void addBalance(Account& acc) {
    cout << "Enter amount (100 / 200 / 500 / 1000): ";
    int x; cin >> x;
    if (x == 100 || x == 200 || x == 500 || x == 1000) {
        acc.balance += x;
        cout << "Balance updated to: " << acc.balance << "\n";
        saveAccount(acc);
        return;
    }
    cout << "Invalid amount.\n";
}

void showAccount(const Account& acc) {
    cout << "\n---- ACCOUNT INFO ----\n";
    cout << "Phone: " << acc.phoneNumber << "\n";
    cout << "Balance: " << acc.balance << "\n";
    cout << "Active Package: " << acc.activePackage << "\n";
    cout << "GB: " << acc.gb << "\n";
    cout << "Minutes: " << acc.minutes << "\n";
    cout << "Offnet Minutes: " << acc.offnetMinutes << "\n";
    cout << "SMS: " << acc.sms << "\n";
}

void subscribePackage(Account& acc, Package* head) {
    cout << "\n---- AVAILABLE PACKAGES ----\n";
    Package* temp = head; int index = 1;
    while (temp) {
        cout << index++ << ". " << temp->name << " - Rs." << temp->cost
             << " | " << temp->gb << "GB, "
             << temp->minutes << " min, "
             << temp->offnetMinutes << " offnet, "
             << temp->sms << " SMS\n";
        temp = temp->next;
    }

    cout << "Choose package number: ";
    int choice; cin >> choice;
    temp = head;
    for (int i = 1; i < choice && temp; i++) temp = temp->next;
    if (!temp) { cout << "Invalid package.\n"; return; }
    if (acc.balance < temp->cost) { cout << "Insufficient balance.\n"; return; }

    acc.balance -= temp->cost;
    acc.activePackage = temp->name;
    acc.gb += temp->gb;
    acc.minutes += temp->minutes;
    acc.offnetMinutes += temp->offnetMinutes;
    acc.sms += temp->sms;

    cout << "Package subscribed successfully!\n";
    saveAccount(acc);
}

void unsubscribe(Account& acc) {
    if (acc.activePackage == "None") { cout << "No active package.\n"; return; }
    acc.activePackage = "None";
    acc.gb = acc.minutes = acc.offnetMinutes = acc.sms = 0;
    cout << "Unsubscribed.\n";
    saveAccount(acc);
}

// -------------------- MAIN PROGRAM --------------------

int main() {
    Package* packages = nullptr;

    insertPackage(packages, "Mega Offer", "Monthly", 1000, 60, 150, 100, 1000);
    insertPackage(packages, "Super Offer", "Monthly", 800, 40, 100, 100, 800);
    insertPackage(packages, "Rare Offer", "Weekly", 500, 30, 80, 80, 500);
    insertPackage(packages, "Common Offer", "Weekly", 300, 25, 50, 50, 250);
    insertPackage(packages, "Day Offer", "Daily", 200, 20, 30, 30, 200);
    insertPackage(packages, "Night Offer", "Daily", 100, 10, 10, 10, 100);

    mergeSort(packages);

    Account user;
    user.phoneNumber = getValidPhoneNumber();
    loadAccount(user);

    while (true) {
        cout << "\n---- MAIN MENU ----\n";
        cout << "1. Add Balance\n2. View Account\n3. Subscribe Package\n4. Customer Support\n5. Change Number\n6. Unsubscribe\n7. Exit\nChoose: ";
        int ch; cin >> ch;

        if (ch == 1) addBalance(user);
        else if (ch == 2) showAccount(user);
        else if (ch == 3) subscribePackage(user, packages);
        else if (ch == 4) customerSupportMenu(user.phoneNumber);
        else if (ch == 5) {
            user = Account();
            user.phoneNumber = getValidPhoneNumber();
            loadAccount(user);
        }
        else if (ch == 6) unsubscribe(user);
        else if (ch == 7) break;
        else cout << "Invalid option.\n";
    }

    return 0;
}
