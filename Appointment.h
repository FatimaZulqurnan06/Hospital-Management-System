#ifndef APPOINTMENT_H
#define APPOINTMENT_H
#include <iostream>
using namespace std;

class Doctor;
class Patient;
class Appointment 
{
private:
    int appointmentID;
    int patientID;
    int doctorID;
    char* date;
    char* timeSlot;
    char* status;
    Doctor* doctor;
    Patient* patient;

public:

    Appointment(int apptID, int pID, int dID, const char* d, const char* slot, const char* stat);

    friend ostream& operator<<(std::ostream& os, const Appointment& appt);
    int getApptID() const;
    int getPatientID() const;
    int getDoctorID() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;
    void setStatus(const char* newStatus);
    Patient* getPatient();
    Doctor* getDoctor();
    void setPatient(Patient* p);
    void setDoctor(Doctor* d);
    void display();
    bool operator == (Appointment& other);

    ~Appointment();
};
#endif
