#ifndef PERSCRIPTION_H
#define PERSCRIPTION_H
#include<iostream>
using namespace std;
class Patient;
class Doctor;
class Appointments;
class Prescription
{
	int prescriptionID;
	int appointmentID;
	int patientID;
	int doctorID;
	char* date;
	char* medicines;
	char* notes;
	Patient* patient;
	Doctor* doctor;
	Appointments* appointment;
public:
	Prescription(int prescriptionid, int appointmentid, int patientid, int doctorid, const char* date, const char* medicines, const char* notes);
	int getPrescriptionID();
	int getAppointmentID();
	int getPatientID();
	int getDoctorID();
	char* getDate();
	char* getMedicines();
	char* getNotes();
	void setPatient(Patient* p);
	void setDoctor(Doctor* d);
	void setAppointment(Appointments* a);
	Patient* getPatient();
	Doctor* getDoctor();
	Appointments* getAppointment();
	friend ostream& operator << (ostream& out, Prescription& p);
	void display();
	~Prescription();
};
#endif 