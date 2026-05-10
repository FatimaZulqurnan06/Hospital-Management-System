#ifndef DOCTOR_H
#define DOCTOR_H
#include "Person.h"
#include <iostream>
using namespace std;

class Doctor : public Person
{
private:
	char* specialization;
	char* contact;
	float fee;
public:
	Doctor(int id, const char* name, const char* specialization, const char* contact, const char* password, float fee);
	
	char* getSpecialization();
	char* getContact();
	float getFee();
	void displayMenu() override;
	void displayDetails() override;
	friend ostream& operator<<(ostream& out, Doctor& d);
	bool operator == (Doctor& other);

	~Doctor();
};
#endif