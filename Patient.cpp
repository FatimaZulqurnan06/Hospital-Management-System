#include "Patient.h"
#include <iostream>
using namespace std;

Patient::Patient(int id, const char* name, const char* pass, int age, char gender, const char* contact, float balance) : Person(id, name, pass) 
{
    this->age = age;
    this->gender = gender;
    this->balance = balance;

    int l = 0;
    while (contact[l] != '\0')
    {
        l++;
    }

    this->contact = new char[l + 1];

    for (int i = 0; i <= l; i++) 
    {
        this->contact[i] = contact[i];
    }
}

Patient& Patient::operator += (float amount) 
{
    balance += amount;
    return *this;
}

Patient& Patient::operator -= (float amount) 
{
    balance -= amount;
    return *this;
}

bool Patient::operator == (const Patient& other) const 
{
    return this->ID == other.ID;
}

ostream& operator<<(ostream& os, const Patient& p) 
{
    os << "ID: " << p.ID << " | Name: " << p.name << " | Balance: " << p.balance;
    return os;
}

void Patient::displayMenu() 
{
    cout << "___ Patient Menu ___";
    cout << "1. Book Appointment" << endl;
    cout << "2. Cancel Appointment" << endl;
    cout << "3. View Appointments" << endl;
    cout << "4. Medical Records" << endl; 
    cout << "5. View Bills" << endl; 
    cout << "6. Pay Bill" << endl; 
    cout << "7. Top Up" << endl; 
    cout << "8. Logout" << endl;
}

void Patient::displayDetails() 
{
    cout << "Patient ID: " << ID << endl;
    cout << "Name: " << name << endl;
    cout << "Age: " << age << endl;
    cout << "Gender: " << gender << endl;
    cout << "Contact: " << contact << endl;
    cout << "Balance: " << balance << endl;
}

float Patient::getBalance() const 
{ 
    return balance;
}
int Patient::getAge() const 
{ 
    return age; 
}
char Patient::getGender() const 
{ 
    return gender;
}
const char* Patient::getContact() const 
{
    return contact;
}

Patient::~Patient() 
{
    if (contact != nullptr) 
    {
        delete[] contact;
        contact = nullptr;
    }
}