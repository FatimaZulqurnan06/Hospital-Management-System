#ifndef BILL_H
#define BILL_H
#include <iostream>
using namespace std;

class Patient;
class Bill
{
private:
	int bill_id;
	int patient_id;
	int appointment_id;
	Patient* patient;
	float amount;
	char* status;
	char* date;
public:
	Bill(int id, int patientID, int appointmentID, float amount, const char* status, const char* date);

	int getBillID();
	int getPatientID();
	int getAppointmentID();
	Patient* getPatient();
	float getAmount();
	char* getStatus();
	char* getDate();
	void setPatient(Patient* p);
	void setStatus(const char* new_stat);
	friend ostream& operator << (ostream& out,const Bill& b);
	void display();

	~Bill();
};
#endif