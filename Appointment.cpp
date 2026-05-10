#include "Appointment.h"
#include "Patient.h"
#include "Doctor.h"

Appointment::Appointment(int apptID, int pID, int dID, const char* d, const char* slot, const char* stat) 
{
    this->appointmentID = apptID;
    this->patientID = pID;
    this->doctorID = dID;
    this->doctor = nullptr;
    this->patient = nullptr;

    int i = 0;
    while (d[i] != '\0')
    {
        i++;
    }
    this->date = new char[i + 1];
    for (int j = 0; j < i; j++)
    {
        this->date[j] = d[j];
    }
    this->date[i] = '\0';

    int k = 0;
    while (slot[k] != '\0') k++;
    this->timeSlot = new char[k + 1];
    for (int l = 0; l < k; l++) this->timeSlot[l] = slot[l];
    this->timeSlot[k] = '\0';

    int m = 0;
    while (stat[m] != '\0') m++;
    this->status = new char[m + 1];
    for (int n = 0; n < m; n++) this->status[n] = stat[n];
    this->status[m] = '\0';
}

bool Appointment::operator == (Appointment& other) 
{
    if (this->doctorID != other.doctorID) 
    {
        return false;
    }

    const char* cancelStr = "cancelled";

    bool thisIsCancelled = true;

    int i = 0;
    while (this->status[i] != '\0' || cancelStr[i] != '\0') 
    {
        if (this->status[i] != cancelStr[i]) 
        {
            thisIsCancelled = false;
            break;
        }
        i++;
    }

    bool otherIsCancelled = true;
    int j = 0;
    while (other.status[j] != '\0' || cancelStr[j] != '\0') 
    {
        if (other.status[j] != cancelStr[j]) 
        {
            otherIsCancelled = false;
            break;
        }
        j++;
    }

    if (thisIsCancelled || otherIsCancelled) 
    {
        return false;
    }

    bool datesMatch = true;
    int k = 0;
    while (this->date[k] != '\0' || other.date[k] != '\0') 
    {
        if (this->date[k] != other.date[k]) 
        {
            datesMatch = false;
            break;
        }
        k++;
    }

    bool slotsMatch = true;
    int l = 0;
    while (this->timeSlot[l] != '\0' || other.timeSlot[l] != '\0') 
    {
        if (this->timeSlot[l] != other.timeSlot[l]) 
        {
            slotsMatch = false;
            break;
        }
        l++;
    }

    if (datesMatch && slotsMatch) 
    {
        return true;
    }

    return false;
}

void Appointment::setStatus(const char* newStatus) 
{
    if (this->status) delete[] this->status;

    int len = 0;
    while (newStatus[len] != '\0') len++;
    this->status = new char[len + 1];
    for (int i = 0; i <= len; i++) {
        this->status[i] = newStatus[i];
    }
}

void Appointment::display() 
{
    cout << "Appointment ID : " << appointmentID << endl;
    cout << "Date           : " << date << endl;
    cout << "Time Slot      : " << timeSlot << endl;
    cout << "Status         : " << status << endl;
}

int Appointment::getApptID() const { return appointmentID; }
int Appointment::getPatientID() const { return patientID; }
int Appointment::getDoctorID() const { return doctorID; }
const char* Appointment::getDate() const { return date; }
const char* Appointment::getTimeSlot() const { return timeSlot; }
const char* Appointment::getStatus() const { return status; }
Patient* Appointment::getPatient() { return patient; }
Doctor* Appointment::getDoctor() { return doctor; }
void Appointment::setPatient(Patient* p) { patient = p; }
void Appointment::setDoctor(Doctor* d) { doctor = d; }

Appointment::~Appointment() 
{
    delete[] date;
    delete[] timeSlot;
    delete[] status;
}

ostream& operator<<(ostream& os, const Appointment& appt) 
{
    os << "Appt[" << appt.appointmentID << "] Date: " << appt.date << " Slot: " << appt.timeSlot;
    return os;
}