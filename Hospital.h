#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "FileHandler.h"
#include "Validator.h"
#include "Exceptions.h"
#include <iostream>
#include <ctime>

using namespace std;
class Hospital
{
private:
    Storage<Patient>     patients;
    Storage<Doctor>      doctors;
    Storage<Appointment> appointments;
    Storage<Bill>        bills;
    Storage<Prescription>prescriptions;
    Admin* admin;
    FileHandler fileHandler;
    Validator   validator;

    void copyStr(char* dest, const char* src) const;
    int strLen(const char* s) const;
    bool strEqual(const char* a, const char* b) const;
    bool strEqualCI(const char* a, const char* b) const; // case-insensitive
    void toLowerStr(const char* src, char* dest) const;
    void intToStr(int n, char* buf) const;
    void floatToStr(float f, char* buf) const;
    int strToInt(const char* s) const;

    void getTodayDate(char* buf) const;
    void sortAppointmentsByDate(Appointment** arr, int n, bool ascending) const;
    int compareDates(const char* a, const char* b) const;
    int daysBetween(const char* laterDate, const char* earlierDate) const;
    void linkAll();

    Patient* loginPatient();
    Doctor* loginDoctor();
    bool loginAdmin();

    //patient menu
    void patientMenu(Patient* p);
    void bookAppointment(Patient* p);
    void cancelAppointment(Patient* p);
    void viewMyAppointments(Patient* p);
    void viewMedicalRecords(Patient* p);
    void viewMyBills(Patient* p);
    void payBill(Patient* p);
    void topUpBalance(Patient* p);

    //doctor menu
    void doctorMenu(Doctor* d);
    void viewTodaysAppointments(Doctor* d);
    void markComplete(Doctor* d);
    void markNoShow(Doctor* d);
    void writePrescription(Doctor* d);
    void viewPatientHistory(Doctor* d);

    //admin menu
    void adminMenu();
    void addDoctor();
    void removeDoctor();
    void addPatientAdmin();
    void removePatientAdmin();
    void viewAllPatients();
    void viewAllDoctors();
    void viewAllAppointments();
    void viewUnpaidBills();
    void dischargePatient();
    void viewSecurityLog();
    void generateDailyReport();

public:
    Hospital();
    ~Hospital();

    void loadAll();
    void run();

    //some functions called by GUI 
    Patient* guiLoginPatient(const char* idStr, const char* pass);
    Doctor* guiLoginDoctor(const char* idStr, const char* pass);
    bool guiLoginAdmin(const char* idStr, const char* pass);
    void guiBookAppointment(Patient* p, int docID, const char* date, const char* slot, char* outMsg);
    void guiCancelAppointment(Patient* p, int apptID, char* outMsg);
    void guiPayBill(Patient* p, int billID, char* outMsg);
    void guiTopUp(Patient* p, const char* amtStr, char* outMsg);
    void guiMarkComplete(Doctor* d, const char* apptIDStr, char* outMsg);
    void guiMarkNoShow(Doctor* d, const char* apptIDStr, char* outMsg);
    void guiWritePrescription(Doctor* d, const char* apptIDStr, const char* meds, const char* notes, char* outMsg);
    void guiViewPatientHistory(Doctor* d, const char* patIDStr, char* outMsg);
    void guiAddDoctor(const char* name, const char* spec, const char* contact, const char* pass, const char* feeStr, char* outMsg);
    void guiRemoveDoctor(int docID, char* outMsg);
    void guiAddPatient(const char* name, const char* pass, const char* ageStr, const char* genderStr, const char* contact, const char* balStr, char* outMsg);
    void guiRemovePatient(const char* patIDStr, char* outMsg);
    void guiDischarge(const char* patIDStr, char* outMsg);
    void guiSecurityLog(char* outBuf, int bufSize);
    void guiDailyReport(char* outBuf, int bufSize);

   
    Storage<Patient>& getPatients() 
    { return patients; }
    Storage<Doctor>& getDoctors() 
    { return doctors; }
    Storage<Appointment>& getAppointments() 
    { return appointments; }
    Storage<Bill>& getBills() 
    { return bills; }
    Storage<Prescription>& getPrescriptions() 
    { return prescriptions; }

    // some utilities the GUI needs
    void getTodayStr(char* buf) const
    { 
        getTodayDate(buf);
    }
    bool isOverdue(const char* billDate, const char* today) const
    {
        return daysBetween(today, billDate) > 7;
    }
    void logSec(const char* role, const char* id, const char* result)
    {
        fileHandler.logSecurity(role, id, result);
    }
    bool strEq(const char* a, const char* b) const
    { 
        return strEqual(a, b);
    }
    bool strEqCI(const char* a, const char* b) const
    { 
        return strEqualCI(a, b);
    }
};
#endif