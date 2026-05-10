#include "Bill.h"
#include "Patient.h"
#include <iostream>
using namespace std;

Bill::Bill(int id, int patientID, int appointmentID, float amount, const char* status, const char* date) 
{
    this->bill_id = id;
    this->patient_id = patientID;
    this->appointment_id = appointmentID;
    this->amount = amount;
    this->patient = nullptr;

    //status
    int sLen = 0;
    while (status[sLen] != '\0')
    {
        sLen++;
    }
    this->status = new char[sLen + 1];
    for (int i = 0; i <= sLen; i++)
    {
        this->status[i] = status[i];
    }
    //date
    int dLen = 0;
    while (date[dLen] != '\0')
    {
        dLen++;
    }
    this->date = new char[dLen + 1];
    for (int i = 0; i <= dLen; i++)
    {
        this->date[i] = date[i];
    }
}

void Bill::display() 
{
    cout << "___ Bill Details ___" << endl;
    cout << "Bill ID: " << bill_id << endl;
    cout << "Appointment ID: " << appointment_id << endl;
    cout << "Patient ID: " << patient_id << endl;
    cout << "Amount: PKR " << amount << endl;
    cout << "Status: " << status << endl;
    cout << "Date: " << date << endl;
}

ostream& operator<<(ostream& out, const Bill& b) 
{
    out << "Bill [ " << b.bill_id << " ] - Patient: " << b.patient_id << " - Amount: " << b.amount << " - Status: " << b.status;
    return out;
}

void Bill::setPatient(Patient* p) 
{
    this->patient = p;
}

void Bill::setStatus(const char* new_stat) 
{
    if (this->status != nullptr)
    {
        delete[] this->status;
    }

    int len = 0;
    while (new_stat[len] != '\0')
    {
        len++;
    }
    this->status = new char[len + 1];
    for (int i = 0; i <= len; i++)
    {
        this->status[i] = new_stat[i];
    }
}

int Bill::getBillID() 
{ 
    return bill_id;
}
int Bill::getPatientID() 
{ 
    return patient_id;
}
int Bill::getAppointmentID() 
{ 
    return appointment_id;
}
Patient* Bill::getPatient() 
{ 
    return patient;
}
float Bill::getAmount() 
{ 
    return amount;
}
char* Bill::getStatus() 
{ 
    return status;
}
char* Bill::getDate() 
{ 
    return date;
}

Bill::~Bill() 
{
    if (status != nullptr) 
    {
        delete[] status;
        status = nullptr;
    }
    if (date != nullptr) 
    {
        delete[] date;
        date = nullptr;
    }

    // Note: We will not delete the patient pointer here cuz the Patient object is owned by the Storage<Patient> class, not the Bill.
}