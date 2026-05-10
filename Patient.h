#ifndef PATIENT_H
#define PATIENT_H
#include "Person.h"
#include <iostream>
using namespace std;

class Patient : public Person 
{
private:
    int age;
    char gender;
    char* contact;
    float balance;

public:
    Patient (int id, const char* name, const char* pass, int age, char gender, const char* contact, float balance);

    Patient& operator += (float amount);
    Patient& operator -= (float amount);
    bool operator == (const Patient& other) const;
    friend ostream& operator << (ostream& os, const Patient& p);
    void displayMenu() override;
    void displayDetails() override;
    float getBalance() const;
    int getAge() const;
    char getGender() const;
    const char* getContact() const;

    ~Patient();
};
#endif