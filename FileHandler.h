#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Exceptions.h"
#include <fstream>
using namespace std;

class FileHandler
{
private:
    void splitLine(char* line, char** fields, int& count);
    int getMaxID(const char* filename);
    void myCopy(char* dest, const char* src);
    int myLen(const char* s);
    int myToInt(const char* str);
    float myToFloat(const char* str);
    void myIntToStr(int num, char* str);
    void myFloatToStr(float num, char* str);
    void myAppend(char* dest, const char* src);
    char** allocLines();
    char** allocFields();
    void freeLines(char** lines);
    void freeFields(char** fields);

public:
    void loadPatients(Storage<Patient>& storage);
    void loadDoctors(Storage<Doctor>& storage);
    void loadAdmin(Admin*& admin);
    void loadAppointments(Storage<Appointment>& storage);
    void loadBill(Storage<Bill>& storage);
    void loadPrescriptions(Storage<Prescription>& storage);

    void saveDoctor(Doctor* d);
    void saveAppointment(Appointment* a);
    void saveBill(Bill* b);
    void savePrescription(Prescription* p);

    void updatePatient(Patient* p);
    void updateAppointment(Appointment* a);
    void updateBill(Bill* b);
    void updateDoctor(Doctor* d);

    void deleteDoctor(int id);

    void logSecurity(const char* role,const char* enteredID, const char* result);

    void archivePatient(int patientID);

    int generateNewID(const char* filename);
};

#endif
