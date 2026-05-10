#include "Doctor.h"
#include <iostream>
using namespace std;

Doctor::Doctor(int id, const char* name, const char* specialization, const char* contact, const char* password, float fee) : Person(id, name, password) 
{ 
    //specialization
    int sLen = 0;
    while (specialization[sLen] != '\0')
    {
        sLen++;
    }
    this->specialization = new char[sLen + 1];
    for (int i = 0; i <= sLen; i++) 
    {
        this->specialization[i] = specialization[i];
    }
    //contact
    int cLen = 0;
    while (contact[cLen] != '\0')
    {
        cLen++;
    }
    this->contact = new char[cLen + 1];
    for (int i = 0; i <= cLen; i++) 
    {
        this->contact[i] = contact[i];
    }
    //fee
    this->fee = fee;
}

void Doctor::displayMenu() 
{
    cout << endl << "___ Doctor Menu ___" << endl;
    cout << "1. View Appointments" << endl;
    cout << "2. View Patient History" << endl;
    cout << "3. Write Prescription" << endl;
    cout << "4. Logout" << endl;
}

void Doctor::displayDetails() 
{
    cout << "Doctor ID: " << ID << endl;
    cout << "Name: " << name << endl;
    cout << "Specialization: " << specialization << endl;
    cout << "Contact: " << contact << endl;
    cout << "Consultation Fee: " << fee << endl;
}

bool Doctor::operator==(Doctor& other) 
{
    return this->ID == other.ID;
}

ostream& operator<<(ostream& out, Doctor& d) 
{
    out << "ID: " << d.ID << " | Dr. " << d.name << " (" << d.specialization << ")";
    return out;
}

char* Doctor::getSpecialization()
{ 
    return specialization;
}
char* Doctor::getContact()
{ 
    return contact;
}
float Doctor::getFee()
{ 
    return fee;
}

Doctor::~Doctor() 
{
    if (specialization != nullptr) 
    {
        delete[] specialization;
        specialization = nullptr;
    }

    if (contact != nullptr) 
    {
        delete[] contact;
        contact = nullptr;
    }
}