#include "FileHandler.h"
#include <fstream>
#include <iostream>
using namespace std;

int FileHandler::myLen(const char* s) 
{
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

void FileHandler::myCopy(char* dest, const char* src) 
{
    int i = 0;
    while (src[i] != '\0') 
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void FileHandler::myAppend(char* dest, const char* src) 
{
    int i = myLen(dest);
    int j = 0;
    while (src[j] != '\0') 
    {
        dest[i] = src[j];
        i++; j++;
    }
    dest[i] = '\0';
}

int FileHandler::myToInt(const char* str) 
{
    int res = 0;
    for (int i = 0; str[i] != '\0'; i++) res = res * 10 + (str[i] - '0');
    return res;
}

float FileHandler::myToFloat(const char* str) 
{
    float res = 0.0, factor = 1.0;
    bool decimal = false;
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if (str[i] == '.') { decimal = true; continue; }
        if (decimal) factor /= 10.0f;
        res = res * 10.0f + (str[i] - '0');
    }
    return res * factor;
}

void FileHandler::myIntToStr(int num, char* str) {
    if (num == 0) { str[0] = '0'; str[1] = '\0'; return; }
    int i = 0, temp = num;
    while (temp > 0) { temp /= 10; i++; }
    str[i] = '\0';
    for (int j = i - 1; j >= 0; j--) {
        str[j] = (num % 10) + '0';
        num /= 10;
    }
}

void FileHandler::myFloatToStr(float num, char* str) {
    int intPart = (int)num;
    myIntToStr(intPart, str);
    int len = myLen(str);
    str[len++] = '.';
    float decPart = num - intPart;
    for (int i = 0; i < 2; i++) {
        decPart *= 10;
        int d = (int)decPart;
        str[len++] = d + '0';
        decPart -= d;
    }
    str[len] = '\0';
}

char** FileHandler::allocFields() {
    char** f = new char* [10];
    for (int i = 0; i < 10; i++) f[i] = new char[200];
    return f;
}

void FileHandler::freeFields(char** f) {
    for (int i = 0; i < 10; i++) delete[] f[i];
    delete[] f;
}

char** FileHandler::allocLines() {
    char** l = new char* [200];
    for (int i = 0; i < 200; i++) l[i] = new char[300];
    return l;
}

void FileHandler::freeLines(char** l) {
    for (int i = 0; i < 200; i++) delete[] l[i];
    delete[] l;
}

void FileHandler::splitLine(char* line, char** fields, int& count) {
    count = 0;
    int i = 0, j = 0;
    while (line[i] != '\0' && count < 10) {
        if (line[i] == ',') {
            fields[count][j] = '\0';
            count++;
            j = 0;
        }
        else {
            if (j < 199) fields[count][j++] = line[i];
        }
        i++;
    }
    fields[count][j] = '\0';
    count++;
}

void FileHandler::loadPatients(Storage<Patient>& storage) 
{
    ifstream file("patients.txt");
    if (!file) throw FileNotFoundException();

    char line[300];
    char** f = allocFields();
    int count;

    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue; // Skip empty lines
        splitLine(line, f, count);

        if (count >= 7) 
        {
            
            storage.add(new Patient(
                myToInt(f[0]),    // ID
                f[1],             // Name
                f[2],             // Password
                myToInt(f[3]),    // Age
                f[4][0],          // Gender (takes first letter 'M' or 'F')
                f[5],             // Contact
                myToFloat(f[6])   // Balance
            ));
        }
    }
    freeFields(f);
    file.close();
}

void FileHandler::updatePatient(Patient* p) 
{
    char** lines = allocLines();
    char** f = allocFields();
    char tempLine[300];
    int total = 0, count = 0;

    ifstream fin("patients.txt");
    while (fin.getline(lines[total], 300)) total++;
    fin.close();

    ofstream fout("patients.txt");
    for (int i = 0; i < total; i++) 
    {
        myCopy(tempLine, lines[i]);
        splitLine(tempLine, f, count);
        if (myToInt(f[0]) == p->getID()) 
        {
            char idStr[10], ageStr[10], balStr[20];
            myIntToStr(p->getID(), idStr);
            myIntToStr(p->getAge(), ageStr);
            myFloatToStr(p->getBalance(), balStr);

            fout << idStr << "," << p->getName() << "," << p->getPassword() << "," << ageStr << "," << p->getGender() << "," << p->getContact() << "," << balStr << endl;
        }
        else 
        {
            fout << lines[i] << endl;
        }
    }
    fout.close();
    freeFields(f);
    freeLines(lines);
}

int FileHandler::getMaxID(const char* filename) 
{
    ifstream file(filename);
    if (!file) return 0;
    char line[300];
    char** f = allocFields();
    int maxID = 0, count;
    while (file.getline(line, 300)) {
        splitLine(line, f, count);
        int id = myToInt(f[0]);
        if (id > maxID) maxID = id;
    }
    freeFields(f);
    return maxID;
}

int FileHandler::generateNewID(const char* filename) 
{
    return getMaxID(filename) + 1;
}

void FileHandler::loadDoctors(Storage<Doctor>& storage) 
{
    ifstream file("doctors.txt");
    if (!file) throw FileNotFoundException();

    char line[300];
    char** f = allocFields();
    int c;

    while (file.getline(line, 300)) 
    {
        if (line[0] == '\0') continue;
        splitLine(line, f, c);
        if (c >= 6) {
            // ID, Name, Specialization, Contact, Password, Fee
            storage.add(new Doctor(myToInt(f[0]), f[1], f[2], f[3], f[4], myToFloat(f[5])));
        }
    }
    freeFields(f);
    file.close();
}

void FileHandler::saveDoctor(Doctor* d) 
{
    ofstream file("doctors.txt", ios::app);
    if (file) 
    {
        // We write the data comma-separated
        file << d->getID() << "," << d->getName() << "," << d->getSpecialization() << ","<< d->getContact() << "," << d->getPassword() << "," << d->getFee() << endl;
    }
    file.close();
}

//admin
void FileHandler::loadAdmin(Admin*& admin) 
{
    ifstream file("admin.txt");
    if (!file) throw FileNotFoundException();

    char line[200];
    char** f = allocFields();
    int c;

    if (file.getline(line, 200)) {
        splitLine(line, f, c);
        if (c >= 3) {
            // ID, Name, Password
            admin = new Admin(myToInt(f[0]), f[1], f[2]);
        }
    }
    freeFields(f);
    file.close();
}

//appointments
void FileHandler::loadAppointments(Storage<Appointment>& storage) 
{
    ifstream file("appointments.txt");
    if (!file) throw FileNotFoundException();

    char line[300];
    char** f = allocFields();
    int c;

    while (file.getline(line, 300)) 
    {
        if (line[0] == '\0') continue;
        splitLine(line, f, c);
        if (c >= 6) {
            // ID, PatID, DocID, Date, Time, Status
            storage.add(new Appointment(myToInt(f[0]), myToInt(f[1]), myToInt(f[2]), f[3], f[4], f[5]));
        }
    }
    freeFields(f);
    file.close();
}

void FileHandler::saveAppointment(Appointment* a) 
{
    ofstream file("appointments.txt", ios::app);
    if (file) {
        file << a->getApptID() << "," << a->getPatientID() << "," << a->getDoctorID() << ","
            << a->getDate() << "," << a->getTimeSlot() << "," << a->getStatus() << endl;
    }
    file.close();
}

//bills
void FileHandler::loadBill(Storage<Bill>& storage) 
{
    ifstream file("bills.txt");
    if (!file) throw FileNotFoundException();

    char line[300];
    char** f = allocFields();
    int c;

    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue;
        splitLine(line, f, c);
        if (c >= 6) {
            // ID, PatID, ApptID, Amount, Status, Date
            storage.add(new Bill(myToInt(f[0]), myToInt(f[1]), myToInt(f[2]), myToFloat(f[3]), f[4], f[5]));
        }
    }
    freeFields(f);
    file.close();
}

void FileHandler::saveBill(Bill* b) 
{
    ofstream file("bills.txt", ios::app);
    if (file) {
        file << b->getBillID() << "," << b->getPatientID() << "," << b->getAppointmentID() << ","
            << b->getAmount() << "," << b->getStatus() << "," << b->getDate() << endl;
    }
    file.close();
}

//prescription
void FileHandler::loadPrescriptions(Storage<Prescription>& storage) 
{
    ifstream file("prescriptions.txt");
    if (!file) throw FileNotFoundException();

    char line[500];
    char** f = allocFields();
    int c;

    while (file.getline(line, 500)) 
    {
        if (line[0] == '\0') continue;
        splitLine(line, f, c);
        if (c >= 7) {
            // ID, ApptID, PatID, DocID, Date, Meds, Notes
            storage.add(new Prescription(myToInt(f[0]), myToInt(f[1]), myToInt(f[2]), myToInt(f[3]), f[4], f[5], f[6]));
        }
    }
    freeFields(f);
    file.close();
}

void FileHandler::savePrescription(Prescription* p) 
{
    ofstream file("prescriptions.txt", ios::app);
    if (file) {
        file << p->getPrescriptionID() << "," << p->getAppointmentID() << "," << p->getPatientID() << "," << p->getDoctorID() << "," << p->getDate() << "," << p->getMedicines() << "," << p->getNotes() << endl;
    }
    file.close();
}

void FileHandler::updateAppointment(Appointment* a)
{
    char** lines = allocLines();
    char** f = allocFields();
    char temp[300];
    int total = 0, c;

    ifstream fin("appointments.txt");
    while (total < 200 && fin.getline(lines[total], 300)) total++;
    fin.close();

    ofstream fout("appointments.txt");
    for (int i = 0; i < total; i++)
    {
        myCopy(temp, lines[i]);
        splitLine(temp, f, c);
        if (myToInt(f[0]) == a->getApptID())
        {
            // Write updated data
            fout << a->getApptID() << "," << a->getPatientID() << "," << a->getDoctorID() << ","
                << a->getDate() << "," << a->getTimeSlot() << "," << a->getStatus() << endl;
        }
        else {
            fout << lines[i] << endl;
        }
    }
    fout.close();
    freeFields(f);
    freeLines(lines);
}

void FileHandler::updateBill(Bill* b) {
    char** lines = allocLines();
    char** f = allocFields();
    char temp[300];
    int total = 0, c;

    ifstream fin("bills.txt");
    while (total < 200 && fin.getline(lines[total], 300)) total++;
    fin.close();

    ofstream fout("bills.txt");
    for (int i = 0; i < total; i++) {
        myCopy(temp, lines[i]);
        splitLine(temp, f, c);
        if (myToInt(f[0]) == b->getBillID()) {
            fout << b->getBillID() << "," << b->getPatientID() << "," << b->getAppointmentID() << ","
                << b->getAmount() << "," << b->getStatus() << "," << b->getDate() << endl;
        }
        else {
            fout << lines[i] << endl;
        }
    }
    fout.close();
    freeFields(f);
    freeLines(lines);
}

void FileHandler::updateDoctor(Doctor* d) {
    char** lines = allocLines();
    char** f = allocFields();
    char temp[300];
    int total = 0, c;

    ifstream fin("doctors.txt");
    while (total < 200 && fin.getline(lines[total], 300)) total++;
    fin.close();

    ofstream fout("doctors.txt");
    for (int i = 0; i < total; i++) {
        myCopy(temp, lines[i]);
        splitLine(temp, f, c);
        if (myToInt(f[0]) == d->getID()) {
            fout << d->getID() << "," << d->getName() << "," << d->getSpecialization() << ","
                << d->getContact() << "," << d->getPassword() << "," << d->getFee() << endl;
        }
        else {
            fout << lines[i] << endl;
        }
    }
    fout.close();
    freeFields(f);
    freeLines(lines);
}

void FileHandler::deleteDoctor(int id) {
    char** lines = allocLines();
    char** f = allocFields();
    char temp[300];
    int total = 0, c;

    ifstream fin("doctors.txt");
    while (total < 200 && fin.getline(lines[total], 300)) total++;
    fin.close();

    ofstream fout("doctors.txt");
    for (int i = 0; i < total; i++) {
        myCopy(temp, lines[i]);
        splitLine(temp, f, c);
        // Only write back if ID doesn't match (effectively deleting)
        if (myToInt(f[0]) != id) {
            fout << lines[i] << endl;
        }
    }
    fout.close();
    freeFields(f);
    freeLines(lines);
}

void FileHandler::logSecurity(const char* role, const char* enteredID, const char* result) {
    ofstream file("security_log.txt", ios::app);
    if (file) {
        file << role << "," << enteredID << "," << result << endl;
    }
    file.close();
}

void FileHandler::archivePatient(int patientID) {
    char** lines = allocLines();
    char** f = allocFields();
    char temp[300];
    int total = 0, c;

    // 1. Move patient record to discharged.txt
    ifstream fin("patients.txt");
    ofstream farchive("discharged.txt", ios::app);

    while (total < 200 && fin.getline(lines[total], 300)) {
        myCopy(temp, lines[total]);
        splitLine(temp, f, c);
        if (myToInt(f[0]) == patientID) {
            farchive << lines[total] << endl;
        }
        total++;
    }
    fin.close();
    farchive.close();

    // 2. Overwrite patients.txt without the archived patient
    ofstream fout("patients.txt");
    for (int i = 0; i < total; i++) {
        myCopy(temp, lines[i]);
        splitLine(temp, f, c);
        if (myToInt(f[0]) != patientID) {
            fout << lines[i] << endl;
        }
    }
    fout.close();

    freeFields(f);
    freeLines(lines);
}