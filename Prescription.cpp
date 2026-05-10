#include "Prescription.h"
#include<iostream>
using namespace std;

Prescription::Prescription (int id, int appointmentid, int patientid, int doctorid, const char* d, const char* meds, const char* ns)
{
	prescriptionID = id;
	appointmentID = appointmentid;
	patientID = patientid;
	doctorID = doctorid;

	int i = 0;
	while (d[i] != '\0')
	{
		i++;
	}
	date = new char[i + 1];
	for (int j = 0; j < i; j++)
	{
		date[j] = d[j];
	}
	date[i] = '\0';

	int k = 0;
	while (meds[k] != '\0')
	{
		k++;
	}
	if (k >= 500)
	{
		k = 499;
	}
	medicines = new char[k + 1];
	for (int l = 0; l < k; l++)
	{
		medicines[l] = meds[l];
	}
	medicines[k] = '\0';

	int m = 0;
	while (ns[m] != '\0')
	{
		m++;
	}
	if (m >= 300)
	{
		m = 299;
	}
	notes = new char[m + 1];
	for (int n = 0; n < m; n++)
	{
		notes[n] = ns[n];
	}
	notes[m] = '\0';
}

int Prescription::getPrescriptionID()
{
	return prescriptionID;
}
int Prescription::getAppointmentID()
{
	return appointmentID;
}
int Prescription::getPatientID()
{
	return patientID;
}
int Prescription::getDoctorID()
{
	return doctorID;
}
char* Prescription::getDate()
{
	return date;
}
char* Prescription::getMedicines()
{
	return medicines;
}
char* Prescription::getNotes()
{
	return notes;
}
void Prescription::display()
{
	cout << "Prescription ID : " << prescriptionID << endl;
	cout << "Appointment ID  : " << appointmentID << endl;
	cout << "Patient ID      : " << patientID << endl;
	cout << "Doctor ID       : " << doctorID << endl;
	cout << "Date            : " << date << endl;
	cout << "Medicines       : " << medicines << endl;
	cout << "Notes           : " << notes << endl;
}
void Prescription::setPatient(Patient* p)
{
	patient = p;
}
Patient* Prescription::getPatient()
{
	return patient;
}

void Prescription::setDoctor(Doctor* d)
{
	doctor = d;
}
Doctor* Prescription::getDoctor()
{
	return doctor;
}

void Prescription::setAppointment(Appointments* a)
{
	appointment = a;
}
Appointments* Prescription::getAppointment()
{
	return appointment;
}

ostream& operator << (ostream& out, Prescription& p)
{
	out << "Date      : " << p.date << endl;
	out << "Medicines : " << p.medicines << endl;
	out << "Notes     : " << p.notes << endl;
	return out;
}

Prescription::~Prescription()
{
	delete[] date;
	delete[] medicines;
	delete[] notes;
	date = nullptr;
	medicines = nullptr;
	notes = nullptr;
}