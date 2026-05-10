#define _CRT_SECURE_NO_WARNINGS
#include "Hospital.h"
#include "GUI.h"
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;

Hospital::Hospital() : admin(nullptr) 
{

}

Hospital::~Hospital()
{
    if (admin != nullptr)
    {
        delete admin;
        admin = nullptr;
    }
    // Storage destructors handle their own heap objects.
}

//here we are loading all our datafiles
void Hospital::loadAll()
{
    try { fileHandler.loadPatients(patients); }
    catch (FileNotFoundException& e) { std::cout << e.what() << endl; }

    try { fileHandler.loadDoctors(doctors); }
    catch (FileNotFoundException& e) { std::cout << e.what() << endl; }

    try { fileHandler.loadAdmin(admin); }
    catch (FileNotFoundException& e) { std::cout << e.what() << endl; }

    try { fileHandler.loadAppointments(appointments); }
    catch (FileNotFoundException& e) { std::cout << e.what() << endl; }

    try { fileHandler.loadBill(bills); }
    catch (FileNotFoundException& e) { std::cout << e.what() << endl; }

    try { fileHandler.loadPrescriptions(prescriptions); }
    catch (FileNotFoundException& e) { std::cout << e.what() << endl; }

    linkAll();
}

void Hospital::linkAll()
{
    // linking appointments to patients n docs
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr)
        {
            continue;
        }
        a->setPatient(patients.findByID(a->getPatientID()));
        a->setDoctor(doctors.findByID(a->getDoctorID()));
    }
    //linking bills to patients
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr)
        {
            continue;
        }
        b->setPatient(patients.findByID(b->getPatientID()));
    }
    //linking prescriptions to patients and docs
    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (pr == nullptr)
        {
            continue;
        }
        pr->setPatient(patients.findByID(pr->getPatientID()));
        pr->setDoctor(doctors.findByID(pr->getDoctorID()));
    }
}

//some helper functions as strings aren't allowed
void Hospital::copyStr(char* dest, const char* src) const
{
    int i = 0;
    while (src[i] != '\0') 
    { 
        dest[i] = src[i]; i++;
    }
    dest[i] = '\0';
}

int Hospital::strLen(const char* s) const
{
    int i = 0;
    while (s[i] != '\0')
    {
        i++;
    }
    return i;
}

bool Hospital::strEqual(const char* a, const char* b) const
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i]) return false;
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

// some case-insensitive comparisons
bool Hospital::strEqualCI(const char* a, const char* b) const
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z')
        {
            ca += 32;
        }
        if (cb >= 'A' && cb <= 'Z')
        {
            cb += 32;
        }
        if (ca != cb)
        {
            return false;
        }
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

void Hospital::toLowerStr(const char* src, char* dest) const
{
    int i = 0;
    while (src[i] != '\0')
    {
        char c = src[i];
        if (c >= 'A' && c <= 'Z') 
        { 
            c += 32;
        }
        dest[i] = c;
        i++;
    }
    dest[i] = '\0';
}

void Hospital::intToStr(int n, char* buf) const
{
    if (n == 0)
    { 
        buf[0] = '0';
        buf[1] = '\0';
        return; 
    }
    int i = 0, tmp = n;

    while (tmp > 0)
    { 
        tmp /= 10;
        i++;
    }
    buf[i] = '\0';
    for (int j = i - 1; j >= 0; j--)
    {
        buf[j] = (n % 10) + '0';
        n /= 10;
    }
}

void Hospital::floatToStr(float f, char* buf) const
{
    int ip = (int)f;
    intToStr(ip, buf);
    int len = strLen(buf);
    buf[len++] = '.';
    float dp = f - ip;
    for (int i = 0; i < 2; i++)
    {
        dp *= 10;
        int d = (int)dp;
        buf[len++] = d + '0';
        dp -= d;
    }
    buf[len] = '\0';
}

int Hospital::strToInt(const char* s) const
{
    int res = 0;
    for (int i = 0; s[i] != '\0'; i++) res = res * 10 + (s[i] - '0');
    return res;
}

void Hospital::getTodayDate(char* buf) const
{
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    // day
    buf[0] = ((now->tm_mday / 10) % 10) + '0';
    buf[1] = (now->tm_mday % 10) + '0';
    buf[2] = '-';
    int m = now->tm_mon + 1;
    buf[3] = (m / 10) + '0';
    buf[4] = (m % 10) + '0';
    buf[5] = '-';
    int y = now->tm_year + 1900;
    buf[6] = (y / 1000) + '0';
    buf[7] = ((y / 100) % 10) + '0';
    buf[8] = ((y / 10) % 10) + '0';
    buf[9] = (y % 10) + '0';
    buf[10] = '\0';
}

// this is to compare DD-MM-YYYY strings. it returns -1 if a < b, 0 equal, 1 if a > b
int Hospital::compareDates(const char* a, const char* b) const
{
    // Conversion to YYYY-MM-DD a: [0-1]=DD [3-4]=MM [6-9]=YYYY
    auto toNum = [](const char* d) -> int
    {
       int y = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
       int mo = (d[3] - '0') * 10 + (d[4] - '0');
       int dy = (d[0] - '0') * 10 + (d[1] - '0');
       return y * 10000 + mo * 100 + dy;
    };
    int na = toNum(a), nb = toNum(b);
    if (na < nb) return -1;
    if (na > nb) return  1;
    return 0;
}

// Returns how many days later is laterDate compared to earlierDate (approximate)
int Hospital::daysBetween(const char* laterDate, const char* earlierDate) const
{
    auto toJulian = [](const char* d) -> int
    {
            int y = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
            int m = (d[3] - '0') * 10 + (d[4] - '0');
            int dy = (d[0] - '0') * 10 + (d[1] - '0');

            // Simple Julian Day Number approximation
            int a = (14 - m) / 12;
            int yr = y + 4800 - a;
            int mn = m + 12 * a - 3;
            return dy + (153 * mn + 2) / 5 + 365 * yr + yr / 4 - yr / 100 + yr / 400 - 32045;
    };
    return toJulian(laterDate) - toJulian(earlierDate);
}

//this is sorting array of appointment pointers by date using bubble sort
void Hospital::sortAppointmentsByDate(Appointment** arr, int n, bool ascending) const
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            int cmp = compareDates(arr[j]->getDate(), arr[j + 1]->getDate());
            bool doSwap = ascending ? (cmp > 0) : (cmp < 0);
            if (doSwap)
            {
                Appointment* tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

//the main run loop / our login screen
void Hospital::run()
{
    int choice;
    do
    {
        cout << endl << "Welcome to MediCore Hospital Management System" << endl;
        cout << "Login as:" << endl;
        cout << " 1. Patient" << endl;
        cout << " 2. Doctor" << endl;
        cout << " 3. Admin" << endl;
        cout << " 4. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (!validator.check_menu(choice, 1, 4))
        {
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }

        if (choice == 1)
        {
            Patient* p = loginPatient();
            if (p != nullptr) patientMenu(p);
        }
        else if (choice == 2)
        {
            Doctor* d = loginDoctor();
            if (d != nullptr) doctorMenu(d);
        }
        else if (choice == 3)
        {
            if (loginAdmin()) adminMenu();
        }
    } while (choice != 4);

    cout << "Thank you for using MediCore. Goodbye!" << endl;
}

//some helpers for login
Patient* Hospital::loginPatient()
{
    int attempts = 0;
    while (attempts < 3)
    {
        char idBuf[20], passBuf[50];
        cout << "Enter Patient ID: ";
        cin >> idBuf;
        cout << "Enter Password : ";
        cin >> passBuf;

        if (!validator.check_ID(idBuf))
        {
            cout << "Invalid ID format." << endl;
            attempts++;
            // Log failed attempt
            fileHandler.logSecurity("Patient", idBuf, "FAILED");
            if (attempts == 3) { cout << "Account locked. Contact admin." << endl; }
            continue;
        }

        int id = strToInt(idBuf);
        Patient* p = patients.findByID(id);

        if (p != nullptr && strEqual(p->getPassword(), passBuf))
        {
            fileHandler.logSecurity("Patient", idBuf, "SUCCESS");
            cout << "Login successful. Welcome, " << p->getName() << "!" << endl;
            return p;
        }
        else
        {
            fileHandler.logSecurity("Patient", idBuf, "FAILED");
            cout << "Invalid ID or password." << endl;
            attempts++;
            if (attempts == 3) cout << "Account locked. Contact admin." << endl;
        }
    }
    return nullptr;
}

Doctor* Hospital::loginDoctor()
{
    int attempts = 0;
    while (attempts < 3)
    {
        char idBuf[20], passBuf[50];
        cout << "Enter Doctor ID: ";
        cin >> idBuf;
        cout << "Enter Password : ";
        cin >> passBuf;

        if (!validator.check_ID(idBuf))
        {
            cout << "Invalid ID format." << endl;
            fileHandler.logSecurity("Doctor", idBuf, "FAILED");
            attempts++;
            if (attempts == 3) cout << "Account locked. Contact admin." << endl;
            continue;
        }

        int id = strToInt(idBuf);
        Doctor* d = doctors.findByID(id);

        if (d != nullptr && strEqual(d->getPassword(), passBuf))
        {
            fileHandler.logSecurity("Doctor", idBuf, "SUCCESS");
            cout << "Login successful. Welcome, Dr. " << d->getName() << "!" << endl;
            return d;
        }
        else
        {
            fileHandler.logSecurity("Doctor", idBuf, "FAILED");
            cout << "Invalid ID or password." << endl;
            attempts++;
            if (attempts == 3) cout << "Account locked. Contact admin." << endl;
        }
    }
    return nullptr;
}

bool Hospital::loginAdmin()
{
    if (admin == nullptr)
    {
        cout << "Admin record not loaded." << endl;
        return false;
    }
    int attempts = 0;
    while (attempts < 3)
    {
        char idBuf[20], passBuf[50];
        cout << "Enter Admin ID: ";
        cin >> idBuf;
        cout << "Enter Password: ";
        cin >> passBuf;

        if (!validator.check_ID(idBuf))
        {
            cout << "Invalid ID format." << endl;
            fileHandler.logSecurity("Admin", idBuf, "FAILED");
            attempts++;
            if (attempts == 3) cout << "Account locked. Contact admin." << endl;
            continue;
        }

        int id = strToInt(idBuf);
        if (id == admin->getID() && strEqual(admin->getPassword(), passBuf))
        {
            fileHandler.logSecurity("Admin", idBuf, "SUCCESS");
            cout << "Admin login successful." << endl;
            return true;
        }
        else
        {
            fileHandler.logSecurity("Admin", idBuf, "FAILED");
            cout << "Invalid ID or password." << endl;
            attempts++;
            if (attempts == 3) cout << "Account locked. Contact admin." << endl;
        }
    }
    return false;
}

//patient menu
void Hospital::patientMenu(Patient* p)
{
    int choice;
    do
    {
        cout << endl << "Welcome, " << p->getName() << endl;
        cout << "Balance: PKR ";
        char bal[20]; floatToStr(p->getBalance(), bal);
        cout << bal << endl;
        p->displayMenu();
        cout << "Enter choice: ";
        cin >> choice;

        if (!validator.check_menu(choice, 1, 8))
        {
            cout << "Invalid choice." << endl;
            continue;
        }

        switch (choice)
        {
        case 1: bookAppointment(p);      break;
        case 2: cancelAppointment(p);    break;
        case 3: viewMyAppointments(p);   break;
        case 4: viewMedicalRecords(p);   break;
        case 5: viewMyBills(p);          break;
        case 6: payBill(p);              break;
        case 7: topUpBalance(p);         break;
        case 8: cout << "Logged out." << endl; break;
        }
    } while (choice != 8);
}

//booking an appointment
void Hospital::bookAppointment(Patient* p)
{
    //searchin by specialization
    char spec[60];
    cout << "Enter specialization to search (e.g. Cardiology): ";
    cin.ignore();
    cin.getline(spec, 60);

    //collecting any matching doctors
    Doctor* matching[100];
    int matchCount = 0;
    for (int i = 0; i < doctors.size(); i++)
    {
        Doctor* d = doctors.getDataAtIndex(i);
        if (d == nullptr) continue;
        if (strEqualCI(d->getSpecialization(), spec))
        matching[matchCount++] = d;
    }

    if (matchCount == 0)
    {
        cout << "No doctors available for that specialization." << endl;
        return;
    }

    cout << "\nAvailable Doctors:" << endl;
    cout << "ID  | Name                  | Fee (PKR)" << endl;
    for (int i = 0; i < matchCount; i++)
    {
        char fee[20]; floatToStr(matching[i]->getFee(), fee);
        cout << matching[i]->getID() << "   | " << matching[i]->getName()
            << "  | " << fee << endl;
    }

    // --- Step 2: Doctor selection ---
    char docIDBuf[20];
    cout << "Enter Doctor ID: ";
    cin >> docIDBuf;

    Doctor* chosenDoc = nullptr;
    if (validator.check_ID(docIDBuf))
        chosenDoc = doctors.findByID(strToInt(docIDBuf));

    if (chosenDoc == nullptr)
    {
        cout << "Doctor not found." << endl;
        return;
    }

    // --- Step 3: Date input (max 3 attempts) ---
    char date[20];
    int dateAttempts = 0;
    bool validDate = false;
    while (dateAttempts < 3)
    {
        cout << "Enter date (DD-MM-YYYY): ";
        cin >> date;
        if (validator.check_date(date))
        {
            validDate = true;
            break;
        }
        cout << "Invalid date. Use format DD-MM-YYYY." << endl;
        dateAttempts++;
    }
    if (!validDate) return;

    // --- Step 4: Show available time slots ---
    const char* allSlots[8] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };

    auto showSlots = [&]()
        {
            cout << "\nAvailable time slots for " << chosenDoc->getName() << " on " << date << ":" << endl;
            bool anyFree = false;
            for (int s = 0; s < 8; s++)
            {
                bool taken = false;
                for (int i = 0; i < appointments.size(); i++)
                {
                    Appointment* a = appointments.getDataAtIndex(i);
                    if (a == nullptr) continue;
                    if (a->getDoctorID() == chosenDoc->getID() &&
                        strEqual(a->getDate(), date) &&
                        strEqual(a->getTimeSlot(), allSlots[s]) &&
                        !strEqual(a->getStatus(), "cancelled"))
                    {
                        taken = true;
                        break;
                    }
                }
                if (!taken) { cout << "  " << allSlots[s] << endl; anyFree = true; }
            }
            if (!anyFree) cout << "  (No slots available)" << endl;
        };

    // --- Step 5: Time slot selection (retry on conflict) ---
    char slot[10];
    bool slotBooked = false;
    while (!slotBooked)
    {
        showSlots();
        cout << "Enter time slot (e.g. 09:00): ";
        cin >> slot;

        if (!validator.check_time(slot))
        {
            cout << "Invalid time slot." << endl;
            continue;
        }

        // Check conflict using exception
        bool conflict = false;
        for (int i = 0; i < appointments.size(); i++)
        {
            Appointment* a = appointments.getDataAtIndex(i);
            if (a == nullptr) continue;
            if (a->getDoctorID() == chosenDoc->getID() &&
                strEqual(a->getDate(), date) &&
                strEqual(a->getTimeSlot(), slot) &&
                !strEqual(a->getStatus(), "cancelled"))
            {
                conflict = true;
                break;
            }
        }

        if (conflict)
        {
            try { throw SlotUnavailableException(); }
            catch (SlotUnavailableException& e)
            {
                cout << e.what() << endl;
                continue;
            }
        }

        // --- Step 6: Check balance ---
        if (p->getBalance() < chosenDoc->getFee())
        {
            try { throw InsufficientFundsException(); }
            catch (InsufficientFundsException& e)
            {
                cout << e.what() << endl;
                return;
            }
        }

        // --- Step 7: Create appointment and bill ---
        int apptID = fileHandler.generateNewID("appointments.txt");
        int billID = fileHandler.generateNewID("bills.txt");

        Appointment* newAppt = new Appointment(apptID, p->getID(), chosenDoc->getID(),
            date, slot, "pending");
        newAppt->setPatient(p);
        newAppt->setDoctor(chosenDoc);
        appointments.add(newAppt);
        fileHandler.saveAppointment(newAppt);

        Bill* newBill = new Bill(billID, p->getID(), apptID,
            chosenDoc->getFee(), "unpaid", date);
        newBill->setPatient(p);
        bills.add(newBill);
        fileHandler.saveBill(newBill);

        // Deduct balance
        *p -= chosenDoc->getFee();
        fileHandler.updatePatient(p);

        char idStr[20]; intToStr(apptID, idStr);
        cout << "Appointment booked successfully. Appointment ID: " << idStr << endl;
        slotBooked = true;
    }
}

// ----------------------------------------------------------
//  2. Cancel Appointment
// ----------------------------------------------------------
void Hospital::cancelAppointment(Patient* p)
{
    // Collect pending appointments for this patient
    Appointment* pending[100];
    int pCount = 0;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getPatientID() == p->getID() && strEqual(a->getStatus(), "pending"))
            pending[pCount++] = a;
    }

    if (pCount == 0)
    {
        cout << "You have no pending appointments." << endl;
        return;
    }

    cout << "\nPending Appointments:" << endl;
    cout << "Appt ID | Doctor Name             | Date       | Time Slot" << endl;
    for (int i = 0; i < pCount; i++)
    {
        Doctor* d = doctors.findByID(pending[i]->getDoctorID());
        cout << pending[i]->getApptID() << "       | ";
        if (d != nullptr) cout << d->getName(); else cout << "N/A";
        cout << "  | " << pending[i]->getDate()
            << " | " << pending[i]->getTimeSlot() << endl;
    }
    char apptIDBuf[20];
    cout << "Enter Appointment ID to cancel: ";
    cin >> apptIDBuf;
    int apptID = strToInt(apptIDBuf);

    // Validate it belongs to this patient and is pending
    Appointment* target = nullptr;
    for (int i = 0; i < pCount; i++)
    {
        if (pending[i]->getApptID() == apptID)
        {
            target = pending[i];
            break;
        }
    }

    if (target == nullptr)
    {
        cout << "Invalid appointment ID." << endl;
        return;
    }

    // Get the doctor's fee for refund
    Doctor* d = doctors.findByID(target->getDoctorID());
    float fee = (d != nullptr) ? d->getFee() : 0.0f;

    // Update appointment status
    target->setStatus("cancelled");
    fileHandler.updateAppointment(target);

    // Refund balance using += operator
    *p += fee;
    fileHandler.updatePatient(p);

    // Cancel the corresponding bill
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (b->getAppointmentID() == apptID && b->getPatientID() == p->getID())
        {
            b->setStatus("cancelled");
            fileHandler.updateBill(b);
            break;
        }
    }

    char feeBuf[20]; floatToStr(fee, feeBuf);
    cout << "Appointment cancelled. PKR " << feeBuf << " refunded to your balance." << endl;
}

//view appointments
void Hospital::viewMyAppointments(Patient* p)
{
    Appointment* arr[100];
    int n = 0;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getPatientID() == p->getID())
            arr[n++] = a;
    }

    if (n == 0) { cout << "No appointments found." << endl; return; }

    // Sort ascending by date
    sortAppointmentsByDate(arr, n, true);

    cout << "\nYour Appointments:" << endl;
    cout << "ID  | Doctor          | Specialization    | Date       | Slot  | Status" << endl;
    for (int i = 0; i < n; i++)
    {
        Doctor* d = doctors.findByID(arr[i]->getDoctorID());
        cout << arr[i]->getApptID() << "   | ";
        if (d != nullptr)
            cout << d->getName() << "  | " << d->getSpecialization();
        else
            cout << "N/A             | N/A              ";
        cout << " | " << arr[i]->getDate()
            << " | " << arr[i]->getTimeSlot()
            << " | " << arr[i]->getStatus() << endl;
    }
}

//view medical records
void Hospital::viewMedicalRecords(Patient* p)
{
    Prescription* arr[100];
    int n = 0;
    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (pr == nullptr) continue;
        if (pr->getPatientID() == p->getID())
            arr[n++] = pr;
    }

    if (n == 0) { cout << "No medical records found." << endl; return; }

    // Sort by date descending starting by most recent first
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (compareDates(arr[j]->getDate(), arr[j + 1]->getDate()) < 0)
            {
                Prescription* tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }

    cout << endl << "Medical Records:" << endl;
    for (int i = 0; i < n; i++)
    {
        Doctor* d = doctors.findByID(arr[i]->getDoctorID());
        cout << "Date     : " << arr[i]->getDate() << endl;
        cout << "Doctor   : ";
        if (d != nullptr) cout << d->getName(); else cout << "N/A";
        cout << endl;
        cout << "Medicines: " << arr[i]->getMedicines() << endl;
        cout << "Notes    : " << arr[i]->getNotes() << endl;
    }
}

//view bills
void Hospital::viewMyBills(Patient* p)
{
    bool found = false;
    float totalUnpaid = 0.0f;

    cout << endl<<"Your Bills:" << endl;
   
    cout << "Bill ID | Appt ID | Amount (PKR) | Status   | Date" << endl;
 

    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (b->getPatientID() == p->getID())
        {
            found = true;
            char amtBuf[20]; floatToStr(b->getAmount(), amtBuf);
            cout << b->getBillID() << "       | "<< b->getAppointmentID() << "       | "<< amtBuf << "       | "<< b->getStatus() << "  | "<< b->getDate() << endl;
            if (strEqual(b->getStatus(), "unpaid"))
            totalUnpaid += b->getAmount();
        }
    }

    if (!found) 
    { 
        cout << "No bills found." << endl;
        return;
    }

    char totalBuf[20]; floatToStr(totalUnpaid, totalBuf);
    cout << "Total Outstanding: PKR " << totalBuf << endl;
}

//pay bill
void Hospital::payBill(Patient* p)
{
    // Show unpaid bills for this patient
    Bill* unpaid[100];
    int uCount = 0;

    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (b->getPatientID() == p->getID() && strEqual(b->getStatus(), "unpaid"))
            unpaid[uCount++] = b;
    }

    if (uCount == 0) { cout << "No unpaid bills." << endl; return; }

    cout << endl<< "Unpaid Bills:" << endl;
    cout << "Bill ID | Appt ID | Amount (PKR) | Date" << endl;
    for (int i = 0; i < uCount; i++)
    {
        char amt[20]; floatToStr(unpaid[i]->getAmount(), amt);
        cout << unpaid[i]->getBillID() << "       | "<< unpaid[i]->getAppointmentID() << "       | "<< amt << "         | "<< unpaid[i]->getDate() << endl;
    }

    char billIDBuf[20];
    cout << "Enter Bill ID to pay: ";
    cin >> billIDBuf;
    int billID = strToInt(billIDBuf);

    Bill* target = nullptr;
    for (int i = 0; i < uCount; i++)
    {
        if (unpaid[i]->getBillID() == billID)
        {
            target = unpaid[i];
            break;
        }
    }

    if (target == nullptr)
    {
        cout << "Invalid bill ID." << endl;
        return;
    }

    // check balance
    if (p->getBalance() < target->getAmount())
    {
        try { throw InsufficientFundsException(); }
        catch (InsufficientFundsException& e) { cout << e.what() << endl; return; }
    }

    // deduct using overloaded -= operator
    *p -= target->getAmount();
    fileHandler.updatePatient(p);

    target->setStatus("paid");
    fileHandler.updateBill(target);

    char balBuf[20]; floatToStr(p->getBalance(), balBuf);
    cout << "Bill paid successfully. Remaining balance: PKR " << balBuf << endl;
}

//top up balance
void Hospital::topUpBalance(Patient* p)
{
    int attempts = 0;
    while (attempts < 3)
    {
        char amtBuf[30];
        cout << "Enter amount to add (PKR): ";
        cin >> amtBuf;

        if (!validator.check_positivefloat(amtBuf))
        {
            try { throw InvalidInputException("Invalid amount. Must be a positive number."); }
            catch (InvalidInputException& e)
            {
                cout << e.what() << endl;
                attempts++;
                continue;
            }
        }

        // Parse float manually
        float amount = 0.0f, factor = 1.0f;
        bool decimal = false;
        for (int i = 0; amtBuf[i] != '\0'; i++)
        {
            if (amtBuf[i] == '.') { decimal = true; continue; }
            if (decimal) factor /= 10.0f;
            amount = amount * 10.0f + (amtBuf[i] - '0');
        }
        amount *= factor;

        // Add using overloaded += operator
        *p += amount;
        fileHandler.updatePatient(p);

        char balBuf[20]; floatToStr(p->getBalance(), balBuf);
        cout << "Balance updated. New balance: PKR " << balBuf << endl;
        return;
    }
}

//doctor menu
void Hospital::doctorMenu(Doctor* d)
{
    int choice;
    do
    {
        cout << "\nWelcome, Dr. " << d->getName() << endl;
        cout << "Specialization: " << d->getSpecialization() << endl;
        d->displayMenu();
        cout << "Enter choice: ";
        cin >> choice;

        if (!validator.check_menu(choice, 1, 6))
        {
            cout << "Invalid choice." << endl;
            continue;
        }

        switch (choice)
        {
        case 1: viewTodaysAppointments(d);  break;
        case 2: markComplete(d);            break;
        case 3: markNoShow(d);              break;
        case 4: writePrescription(d);       break;
        case 5: viewPatientHistory(d);      break;
        case 6: cout << "Logged out." << endl; break;
        }
    } while (choice != 6);
}

//view todays appt
void Hospital::viewTodaysAppointments(Doctor* d)
{
    char today[11]; getTodayDate(today);

    // Collect
    Appointment* arr[100];
    int n = 0;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getDoctorID() == d->getID() && strEqual(a->getDate(), today))
            arr[n++] = a;
    }

    if (n == 0) { cout << "No appointments scheduled for today." << endl; return; }

    // Sort by time slot ascending
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            bool swap = false;
            const char* s1 = arr[j]->getTimeSlot();
            const char* s2 = arr[j + 1]->getTimeSlot();
            for (int k = 0; s1[k] != '\0' && s2[k] != '\0'; k++)
            {
                if (s1[k] > s2[k]) 
                { 
                    swap = true; break;
                }
                if (s1[k] < s2[k]) break;
            }
            if (swap) 
            { 
                Appointment* t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t; 
            }
        }
    }

    cout <<endl<< "Today's Appointments (" << today << "):" << endl;
    cout << "Appt ID | Patient Name          | Slot  | Status" << endl;
    for (int i = 0; i < n; i++)
    {
        Patient* pt = patients.findByID(arr[i]->getPatientID());
        cout << arr[i]->getApptID() << "       | ";
        if (pt != nullptr) cout << pt->getName(); else cout << "N/A";
        cout << "  | " << arr[i]->getTimeSlot()
            << " | " << arr[i]->getStatus() << endl;
    }
}

//marking an appt as complete
void Hospital::markComplete(Doctor* d)
{
    char today[11]; getTodayDate(today);

    cout << "Today's pending appointments:" << endl;
    bool any = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getDoctorID() == d->getID() &&
            strEqual(a->getDate(), today) &&
            strEqual(a->getStatus(), "pending"))
        {
            Patient* pt = patients.findByID(a->getPatientID());
            cout << "  ID: " << a->getApptID() << " | ";
            if (pt != nullptr) cout << pt->getName(); else cout << "N/A";
            cout << " | " << a->getTimeSlot() << endl;
            any = true;
        }
    }
    if (!any) { cout << "No pending appointments today." << endl; return; }

    char apptIDBuf[20];
    cout << "Enter Appointment ID: ";
    cin >> apptIDBuf;
    int apptID = strToInt(apptIDBuf);

    // Validate
    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getApptID() == apptID && a->getDoctorID() == d->getID() && strEqual(a->getStatus(), "pending") && strEqual(a->getDate(), today))
        {
            target = a;
            break;
        }
    }

    if (target == nullptr)
    {
        cout << "Invalid appointment ID." << endl;
        return;
    }

    target->setStatus("completed");
    fileHandler.updateAppointment(target);
    cout << "Appointment marked as completed." << endl;
}

//mark an appt as no show
void Hospital::markNoShow(Doctor* d)
{
    char today[11]; getTodayDate(today);

    cout << "Today's pending appointments:" << endl;
    bool any = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getDoctorID() == d->getID() &&
            strEqual(a->getDate(), today) &&
            strEqual(a->getStatus(), "pending"))
        {
            Patient* pt = patients.findByID(a->getPatientID());
            cout << "  ID: " << a->getApptID() << " | ";
            if (pt != nullptr) cout << pt->getName(); else cout << "N/A";
            cout << " | " << a->getTimeSlot() << endl;
            any = true;
        }
    }
    if (!any) { cout << "No pending appointments today." << endl; return; }

    char apptIDBuf[20];
    cout << "Enter Appointment ID: ";
    cin >> apptIDBuf;
    int apptID = strToInt(apptIDBuf);

    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getApptID() == apptID &&
            a->getDoctorID() == d->getID() &&
            strEqual(a->getStatus(), "pending") &&
            strEqual(a->getDate(), today))
        {
            target = a;
            break;
        }
    }

    if (target == nullptr)
    {
        cout << "Invalid appointment ID." << endl;
        return;
    }

    target->setStatus("no-show");
    fileHandler.updateAppointment(target);

    // Cancel corresponding bill (no refund)
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (b->getAppointmentID() == apptID)
        {
            b->setStatus("cancelled");
            fileHandler.updateBill(b);
            break;
        }
    }

    cout << "Appointment marked as no-show." << endl;
}

//write a prescription
void Hospital::writePrescription(Doctor* d)
{
    char apptIDBuf[20];
    cout << "Enter Appointment ID: ";
    cin >> apptIDBuf;
    int apptID = strToInt(apptIDBuf);

    // Validate: belongs to this doctor, status == completed
    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getApptID() == apptID && a->getDoctorID() == d->getID() && strEqual(a->getStatus(), "completed"))
        {
            target = a;
            break;
        }
    }

    if (target == nullptr)
    {
        cout << "Invalid appointment ID (must be completed and yours)." << endl;
        return;
    }

    // this is to check if prescription already exists
    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (pr == nullptr) continue;
        if (pr->getAppointmentID() == apptID)
        {
            cout << "Prescription already written for this appointment." << endl;
            return;
        }
    }

    // Read medicines (up to 499 chars) and notes (up to 299 chars)
    char medicines[500];
    char notes[300];

    cout << "Enter medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
    cin.ignore();
    cin.getline(medicines, 500);

    cout << "Enter notes (max 300 chars): ";
    cin.getline(notes, 300);

    // Get today's date for prescription
    char today[11]; getTodayDate(today);

    int presID = fileHandler.generateNewID("prescriptions.txt");
    Prescription* pr = new Prescription
    (presID,apptID,target->getPatientID(),d->getID(),today,medicines,notes);
    pr->setPatient(patients.findByID(target->getPatientID()));
    pr->setDoctor(d);
    prescriptions.add(pr);
    fileHandler.savePrescription(pr);

    cout << "Prescription saved." << endl;
}

//viewing patients history
void Hospital::viewPatientHistory(Doctor* d)
{
    char patIDBuf[20];
    cout << "Enter Patient ID: ";
    cin >> patIDBuf;
    int patID = strToInt(patIDBuf);

    Patient* pt = patients.findByID(patID);
    if (pt == nullptr)
    {
        cout << "Access denied. You can only view records of your own patients." << endl;
        return;
    }

    // Check at least one completed appointment with this doctor
    bool hasAccess = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getPatientID() == patID &&
            a->getDoctorID() == d->getID() &&
            strEqual(a->getStatus(), "completed"))
        {
            hasAccess = true;
            break;
        }
    }

    if (!hasAccess)
    {
        cout << "Access denied. You can only view records of your own patients." << endl;
        return;
    }

    // Collect prescriptions written by this doctor for this patient
    Prescription* arr[100];
    int n = 0;
    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (pr == nullptr) continue;
        if (pr->getPatientID() == patID && pr->getDoctorID() == d->getID())
            arr[n++] = pr;
    }

    if (n == 0) { cout << "No prescriptions found for this patient." << endl; return; }

    // Sort descending by date
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (compareDates(arr[j]->getDate(), arr[j + 1]->getDate()) < 0)
            {
                Prescription* tmp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = tmp;
            }

    cout <<endl<< "Medical History for " << pt->getName() << ":" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << "Date     : " << arr[i]->getDate() << endl;
        cout << "Medicines: " << arr[i]->getMedicines() << endl;
        cout << "Notes    : " << arr[i]->getNotes() << endl;
    }
}

//admin menu
void Hospital::adminMenu()
{
    int choice;
    do
    {
        admin->displayMenu();
        cout << "Enter choice: ";
        cin >> choice;

        if (!validator.check_menu(choice, 1, 12))
        {
            cout << "Invalid choice." << endl;
            continue;
        }

        switch (choice)
        {
        case 1:  addDoctor();           
            break;
        case 2:  removeDoctor();        
            break;
        case 3:  addPatientAdmin();     
            break;
        case 4:  removePatientAdmin();  
            break;
        case 5:  viewAllPatients();     
            break;
        case 6:  viewAllDoctors();      
            break;
        case 7:  viewAllAppointments(); 
            break;
        case 8:  viewUnpaidBills();     
            break;
        case 9:  dischargePatient();    
            break;
        case 10: viewSecurityLog();     
            break;
        case 11: generateDailyReport(); 
            break;
        case 12: cout << "Logged out." << endl; 
            break;
        }
    } while (choice != 12);
}

//add doc
void Hospital::addDoctor()
{
    char name[60], spec[60], contact[20], pass[50];
    char feeBuf[20];

    cin.ignore();
    cout << "Enter doctor name (max 50 chars): ";
    cin.getline(name, 51);

    cout << "Enter specialization (max 50 chars): ";
    cin.getline(spec, 51);

    // Contact: exactly 11 digits
    bool validContact = false;
    for (int att = 0; att < 3; att++)
    {
        cout << "Enter contact (11 digits): ";
        cin >> contact;
        if (validator.check_contact(contact)) { validContact = true; break; }
        cout << "Invalid contact number." << endl;
    }
    if (!validContact) return;

    // Password: min 6 chars
    bool validPass = false;
    for (int att = 0; att < 3; att++)
    {
        cout << "Enter password (min 6 chars): ";
        cin >> pass;
        if (validator.check_password(pass)) { validPass = true; break; }
        cout << "Invalid password." << endl;
    }
    if (!validPass) return;

    // Consultation fee
    bool validFee = false;
    float fee = 0.0f;
    for (int att = 0; att < 3; att++)
    {
        cout << "Enter consultation fee (PKR): ";
        cin >> feeBuf;
        if (validator.check_positivefloat(feeBuf))
        {
            // Parse float
            float f = 0.0f, factor = 1.0f;
            bool dec = false;
            for (int i = 0; feeBuf[i] != '\0'; i++)
            {
                if (feeBuf[i] == '.') { dec = true; continue; }
                if (dec) factor /= 10.0f;
                f = f * 10.0f + (feeBuf[i] - '0');
            }
            fee = f * factor;
            validFee = true;
            break;
        }
        cout << "Invalid fee." << endl;
    }
    if (!validFee) return;

    int newID = fileHandler.generateNewID("doctors.txt");
    Doctor* d = new Doctor(newID, name, spec, contact, pass, fee);
    doctors.add(d);
    fileHandler.saveDoctor(d);

    char idBuf[20]; intToStr(newID, idBuf);
    cout << "Doctor added successfully. ID: " << idBuf << endl;
}

//remove doctor
void Hospital::removeDoctor()
{
    viewAllDoctors();

    char docIDBuf[20];
    cout << "Enter Doctor ID to remove: ";
    cin >> docIDBuf;
    int docID = strToInt(docIDBuf);

    Doctor* d = doctors.findByID(docID);
    if (d == nullptr) { cout << "Doctor not found." << endl; return; }

    // Check for pending appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getDoctorID() == docID && strEqual(a->getStatus(), "pending"))
        {
            cout << "Cannot remove doctor with pending appointments. Cancel or reassign them first." << endl;
            return;
        }
    }

    fileHandler.deleteDoctor(docID);
    doctors.removeByID(docID);  // This also deletes the Doctor object
    cout << "Doctor removed." << endl;
}

//add patient
void Hospital::addPatientAdmin()
{
    char name[60], pass[50], contact[20];
    char ageBuf[10], balBuf[20];
    char gender;

    cin.ignore();
    cout << "Enter patient name: ";
    cin.getline(name, 51);

    cout << "Enter password (min 6 chars): ";
    cin >> pass;
    if (!validator.check_password(pass)) { cout << "Invalid password." << endl; return; }

    cout << "Enter age: ";
    cin >> ageBuf;
    int age = strToInt(ageBuf);

    cout << "Enter gender (M/F): ";
    cin >> gender;

    cout << "Enter contact (11 digits): ";
    cin >> contact;
    if (!validator.check_contact(contact)) { cout << "Invalid contact." << endl; return; }

    cout << "Enter initial balance (PKR): ";
    cin >> balBuf;
    float balance = 0.0f, factor = 1.0f;
    bool dec = false;
    for (int i = 0; balBuf[i] != '\0'; i++)
    {
        if (balBuf[i] == '.') { dec = true; continue; }
        if (dec) factor /= 10.0f;
        balance = balance * 10.0f + (balBuf[i] - '0');
    }
    balance *= factor;

    int newID = fileHandler.generateNewID("patients.txt");
    Patient* p = new Patient(newID, name, pass, age, gender, contact, balance);
    patients.add(p);

    ofstream f("patients.txt", ios::app);
    if (f)
    {
        char idStr[10], ageStr[10], balStr[20];
        intToStr(newID, idStr);
        intToStr(age, ageStr);
        floatToStr(balance, balStr);
        f << idStr << "," << name << "," << pass << "," << ageStr << "," << gender << "," << contact << "," << balStr << endl;
    }
    f.close();

    char idBuf[20]; intToStr(newID, idBuf);
    cout << "Patient added successfully. ID: " << idBuf << endl;
}

//remove patient
void Hospital::removePatientAdmin()
{
    char patIDBuf[20];
    cout << "Enter Patient ID to remove: ";
    cin >> patIDBuf;
    int patID = strToInt(patIDBuf);

    Patient* p = patients.findByID(patID);
    if (p == nullptr) { cout << "Patient not found." << endl; return; }

    // Check for unpaid bills / pending appointments
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (b->getPatientID() == patID && strEqual(b->getStatus(), "unpaid"))
        {
            cout << "Cannot remove patient with unpaid bills." << endl;
            return;
        }
    }
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getPatientID() == patID && strEqual(a->getStatus(), "pending"))
        {
            cout << "Cannot remove patient with pending appointments." << endl;
            return;
        }
    }

    fileHandler.archivePatient(patID);
    patients.removeByID(patID);
    cout << "Patient removed." << endl;
}

//view all patients
void Hospital::viewAllPatients()
{
    cout << endl << "All Patients:" << endl;
    cout << "ID  | Name          | Age | G | Contact       | Balance    | Unpaid Bills" << endl;

    for (int i = 0; i < patients.size(); i++)
    {
        Patient* p = patients.getDataAtIndex(i);
        if (p == nullptr) continue;

        // Count unpaid bills
        int unpaidCount = 0;
        for (int j = 0; j < bills.size(); j++)
        {
            Bill* b = bills.getDataAtIndex(j);
            if (b == nullptr) continue;
            if (b->getPatientID() == p->getID() && strEqual(b->getStatus(), "unpaid"))
                unpaidCount++;
        }

        char balBuf[20]; floatToStr(p->getBalance(), balBuf);
        char cntBuf[10]; intToStr(unpaidCount, cntBuf);

        cout << p->getID() << "   | " << p->getName()<< "  | " << p->getAge() << "  | " << p->getGender()<< " | " << p->getContact() << " | " << balBuf << " | " << cntBuf << endl;
    }
}

//view all doctors
void Hospital::viewAllDoctors()
{
    cout << "\nAll Doctors:" << endl;
    cout << "ID  | Name          | Specialization    | Contact       | Fee" << endl;

    for (int i = 0; i < doctors.size(); i++)
    {
        Doctor* d = doctors.getDataAtIndex(i);
        if (d == nullptr) continue;
        char feeBuf[20]; floatToStr(d->getFee(), feeBuf);
        cout << d->getID() << "   | " << d->getName()
            << "  | " << d->getSpecialization()
            << "  | " << d->getContact()
            << " | " << feeBuf << endl;
    }
}

void Hospital::viewAllAppointments()
{
    int n = appointments.size();
    if (n == 0) { cout << "No appointments found." << endl; return; }

    // Copy pointers for sorting
    Appointment** arr = new Appointment * [n];
    int valid = 0;
    for (int i = 0; i < n; i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a != nullptr) arr[valid++] = a;
    }

    // Sort descending by date
    sortAppointmentsByDate(arr, valid, false);

    cout << endl<< "All Appointments:" << endl;
    cout << "ID  | Patient          | Doctor            | Date       | Slot  | Status" << endl;
    for (int i = 0; i < valid; i++)
    {
        Patient* pt = patients.findByID(arr[i]->getPatientID());
        Doctor* d = doctors.findByID(arr[i]->getDoctorID());
        cout << arr[i]->getApptID() << "   | ";
        if (pt != nullptr) cout << pt->getName(); else cout << "N/A";
        cout << "  | ";
        if (d != nullptr) cout << d->getName(); else cout << "N/A";
        cout << "  | " << arr[i]->getDate() << " | " << arr[i]->getTimeSlot() << " | " << arr[i]->getStatus() << endl;
    }
    delete[] arr;
}

//view unpaid bill (w an overdue flag)
void Hospital::viewUnpaidBills()
{
    char today[11]; getTodayDate(today);

    cout << endl<< "Unpaid Bills:" << endl;
    cout << "Bill ID | Patient Name      | Amount (PKR) | Date" << endl;

    bool any = false;
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (!strEqual(b->getStatus(), "unpaid")) continue;

        any = true;
        Patient* pt = patients.findByID(b->getPatientID());
        char amtBuf[20]; floatToStr(b->getAmount(), amtBuf);

        // Check overdue: bill date is > 7 days before today
        int diff = daysBetween(today, b->getDate());
        bool overdue = diff > 7;

        cout << b->getBillID() << "       | ";
        if (pt != nullptr) cout << pt->getName(); else cout << "N/A";
        cout << "  | " << amtBuf << "         | " << b->getDate();
        if (overdue) cout << " [OVERDUE]";
        cout << endl;
    }

    if (!any) cout << "No unpaid bills found." << endl;
}

//discharge patient
void Hospital::dischargePatient()
{
    char patIDBuf[20];
    cout << "Enter Patient ID: ";
    cin >> patIDBuf;
    int patID = strToInt(patIDBuf);

    Patient* p = patients.findByID(patID);
    if (p == nullptr) { cout << "Patient not found." << endl; return; }

    // Check unpaid bills
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (b->getPatientID() == patID && strEqual(b->getStatus(), "unpaid"))
        {
            cout << "Cannot discharge patient with unpaid bills." << endl;
            return;
        }
    }

    // Check pending appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (a->getPatientID() == patID && strEqual(a->getStatus(), "pending"))
        {
            cout << "Cannot discharge patient with pending appointments." << endl;
            return;
        }
    }

    fileHandler.archivePatient(patID);

    ofstream farch("discharged.txt", ios::app);

    farch << "--- APPOINTMENTS ---" << endl;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr || a->getPatientID() != patID) continue;
        farch << a->getApptID() << "," << a->getPatientID() << "," << a->getDoctorID() << "," << a->getDate() << "," << a->getTimeSlot() << "," << a->getStatus() << endl;
    }

    farch << "--- PRESCRIPTIONS ---" << endl;
    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (pr == nullptr || pr->getPatientID() != patID) continue;
        farch << pr->getPrescriptionID() << "," << pr->getAppointmentID() << "," << pr->getPatientID() << "," << pr->getDoctorID() << "," << pr->getDate() << "," << pr->getMedicines() << "," << pr->getNotes() << endl;
    }

    farch << "--- BILLS ---" << endl;
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr || b->getPatientID() != patID) continue;
        char amtBuf[20]; floatToStr(b->getAmount(), amtBuf);
        farch << b->getBillID() << "," << b->getPatientID() << ","<< b->getAppointmentID() << "," << amtBuf << ","<< b->getStatus() << "," << b->getDate() << endl;
    }
    farch.close();
    patients.removeByID(patID);

    cout << "Patient discharged and archived successfully." << endl;
}

//view security log
void Hospital::viewSecurityLog()
{
    ifstream f("security_log.txt");
    if (!f) { cout << "No security events logged." << endl; return; }

    char line[300];
    bool empty = true;
    cout << "\nSecurity Log:" << endl;
    while (f.getline(line, 300))
    {
        if (line[0] != '\0')
        {
            cout << line << endl;
            empty = false;
        }
    }
    f.close();

    if (empty) cout << "No security events logged." << endl;
}

//generate daily report
void Hospital::generateDailyReport()
{
    char today[11]; getTodayDate(today);

    //count appointments today by status
    int totalToday = 0, pendingToday = 0, completedToday = 0,
        noshowToday = 0, cancelledToday = 0;

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a == nullptr) continue;
        if (!strEqual(a->getDate(), today)) continue;

        totalToday++;
        if (strEqual(a->getStatus(), "pending"))   pendingToday++;
        else if (strEqual(a->getStatus(), "completed")) completedToday++;
        else if (strEqual(a->getStatus(), "no-show"))   noshowToday++;
        else if (strEqual(a->getStatus(), "cancelled")) cancelledToday++;
    }

    //Revenue: paid bills dated today
    float revenue = 0.0f;
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b == nullptr) continue;
        if (strEqual(b->getStatus(), "paid") && strEqual(b->getDate(), today))
            revenue += b->getAmount();
    }

    cout << "\n=== Daily Report: " << today << " ===" << endl;

    char tBuf[10], pBuf[10], cBuf[10], nBuf[10], canBuf[10], revBuf[20];
    intToStr(totalToday, tBuf);
    intToStr(pendingToday, pBuf);
    intToStr(completedToday, cBuf);
    intToStr(noshowToday, nBuf);
    intToStr(cancelledToday, canBuf);
    floatToStr(revenue, revBuf);

    cout << "Total appointments today: " << tBuf << " (Pending: " << pBuf << " Completed: " << cBuf << " No-show: " << nBuf << " Cancelled: " << canBuf << ")" << endl;

    cout << "Revenue collected today (paid bills): PKR " << revBuf << endl;

    //patients with outstanding unpaid bills
    cout << "\nPatients with outstanding unpaid bills:" << endl;
    cout << "Patient Name          | Total Owed (PKR)" << endl;

    for (int i = 0; i < patients.size(); i++)
    {
        Patient* p = patients.getDataAtIndex(i);
        if (p == nullptr) continue;

        float owed = 0.0f;
        for (int j = 0; j < bills.size(); j++)
        {
            Bill* b = bills.getDataAtIndex(j);
            if (b == nullptr) continue;
            if (b->getPatientID() == p->getID() && strEqual(b->getStatus(), "unpaid"))
                owed += b->getAmount();
        }

        if (owed > 0.0f)
        {
            char owedBuf[20]; floatToStr(owed, owedBuf);
            cout << p->getName() << "  | " << owedBuf << endl;
        }
    }

    //Doctor-wise summary for today
    cout << "\nDoctor-wise summary for today:" << endl;
    cout << "Doctor Name           | Completed | Pending | No-show" << endl;

    for (int i = 0; i < doctors.size(); i++)
    {
        Doctor* d = doctors.getDataAtIndex(i);
        if (d == nullptr) continue;

        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appointments.size(); j++)
        {
            Appointment* a = appointments.getDataAtIndex(j);
            if (a == nullptr) continue;
            if (a->getDoctorID() != d->getID() || !strEqual(a->getDate(), today)) continue;
            if (strEqual(a->getStatus(), "completed")) dc++;
            else if (strEqual(a->getStatus(), "pending")) dp++;
            else if (strEqual(a->getStatus(), "no-show")) dn++;
        }

        if (dc + dp + dn > 0)
        {
            char dcBuf[5], dpBuf[5], dnBuf[5];
            intToStr(dc, dcBuf); intToStr(dp, dpBuf); intToStr(dn, dnBuf);
            cout << d->getName() << "  | " << dcBuf
                << "         | " << dpBuf
                << "       | " << dnBuf << endl;
        }
    }
}

//some bridge methods needed for the gui
Patient* Hospital::guiLoginPatient(const char* idStr, const char* pass)
{
    if (!validator.check_ID(idStr)) return nullptr;
    int id = strToInt(idStr);
    Patient* p = patients.findByID(id);
    if (p != nullptr && strEqual(p->getPassword(), pass))
    {
        fileHandler.logSecurity("Patient", idStr, "SUCCESS");
        return p;
    }
    return nullptr;
}

Doctor* Hospital::guiLoginDoctor(const char* idStr, const char* pass)
{
    if (!validator.check_ID(idStr)) return nullptr;
    int id = strToInt(idStr);
    Doctor* d = doctors.findByID(id);
    if (d != nullptr && strEqual(d->getPassword(), pass))
    {
        fileHandler.logSecurity("Doctor", idStr, "SUCCESS");
        return d;
    }
    return nullptr;
}

bool Hospital::guiLoginAdmin(const char* idStr, const char* pass)
{
    if (admin == nullptr) return false;
    if (!validator.check_ID(idStr)) return false;
    int id = strToInt(idStr);
    if (id == admin->getID() && strEqual(admin->getPassword(), pass))
    {
        fileHandler.logSecurity("Admin", idStr, "SUCCESS");
        return true;
    }
    return false;
}

void Hospital::guiBookAppointment(Patient* p, int docID,
    const char* date, const char* slot, char* outMsg)
{
    Doctor* chosenDoc = doctors.findByID(docID);
    if (chosenDoc == nullptr) { copyStr(outMsg, "Doctor not found."); return; }

    // Check slot conflict
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getDoctorID() == docID &&
            strEqual(a->getDate(), date) &&
            strEqual(a->getTimeSlot(), slot) &&
            !strEqual(a->getStatus(), "cancelled"))
        {
            copyStr(outMsg, "Slot already taken. Please choose another.");
            return;
        }
    }

    if (p->getBalance() < chosenDoc->getFee())
    {
        copyStr(outMsg, "Insufficient balance. Please top up first.");
        return;
    }

    int apptID = fileHandler.generateNewID("appointments.txt");
    int billID = fileHandler.generateNewID("bills.txt");

    Appointment* newAppt = new Appointment(apptID, p->getID(), docID,
        date, slot, "pending");
    newAppt->setPatient(p);
    newAppt->setDoctor(chosenDoc);
    appointments.add(newAppt);
    fileHandler.saveAppointment(newAppt);

    Bill* newBill = new Bill(billID, p->getID(), apptID,
        chosenDoc->getFee(), "unpaid", date);
    newBill->setPatient(p);
    bills.add(newBill);
    fileHandler.saveBill(newBill);

    *p -= chosenDoc->getFee();
    fileHandler.updatePatient(p);

    char idBuf[20]; intToStr(apptID, idBuf);
    copyStr(outMsg, "Appointment booked! ID: ");
    // append idBuf manually
    int i = strLen(outMsg);
    for (int j = 0; idBuf[j]; j++) outMsg[i++] = idBuf[j];
    outMsg[i] = '\0';
}

void Hospital::guiCancelAppointment(Patient* p, int apptID, char* outMsg)
{
    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getApptID() == apptID && a->getPatientID() == p->getID()
            && strEqual(a->getStatus(), "pending"))
        {
            target = a; break;
        }
    }
    if (!target) { copyStr(outMsg, "Invalid appointment ID."); return; }

    Doctor* d = doctors.findByID(target->getDoctorID());
    float fee = d ? d->getFee() : 0.0f;

    target->setStatus("cancelled");
    fileHandler.updateAppointment(target);

    *p += fee;
    fileHandler.updatePatient(p);

    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (!b) continue;
        if (b->getAppointmentID() == apptID && b->getPatientID() == p->getID())
        {
            b->setStatus("cancelled"); fileHandler.updateBill(b); break;
        }
    }

    char feeBuf[20]; floatToStr(fee, feeBuf);
    copyStr(outMsg, "Cancelled. PKR ");
    int i = strLen(outMsg);
    for (int j = 0; feeBuf[j]; j++) outMsg[i++] = feeBuf[j];
    const char* tail = " refunded.";
    for (int j = 0; tail[j]; j++) outMsg[i++] = tail[j];
    outMsg[i] = '\0';
}

void Hospital::guiPayBill(Patient* p, int billID, char* outMsg)
{
    Bill* target = nullptr;
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (!b) continue;
        if (b->getBillID() == billID && b->getPatientID() == p->getID()
            && strEqual(b->getStatus(), "unpaid"))
        {
            target = b; break;
        }
    }
    if (!target) { copyStr(outMsg, "Invalid bill ID."); return; }

    if (p->getBalance() < target->getAmount())
    {
        copyStr(outMsg, "Insufficient balance. Please top up first."); return;
    }

    *p -= target->getAmount();
    fileHandler.updatePatient(p);
    target->setStatus("paid");
    fileHandler.updateBill(target);

    char balBuf[20]; floatToStr(p->getBalance(), balBuf);
    copyStr(outMsg, "Bill paid. New balance: PKR ");
    int i = strLen(outMsg);
    for (int j = 0; balBuf[j]; j++) outMsg[i++] = balBuf[j];
    outMsg[i] = '\0';
}

void Hospital::guiTopUp(Patient* p, const char* amtStr, char* outMsg)
{
    if (!validator.check_positivefloat(amtStr))
    {
        copyStr(outMsg, "Invalid amount. Enter a positive number."); return;
    }

    float amount = 0.0f, factor = 1.0f;
    bool dec = false;
    for (int i = 0; amtStr[i]; i++)
    {
        if (amtStr[i] == '.') { dec = true; continue; }
        if (dec) factor /= 10.0f;
        amount = amount * 10.0f + (amtStr[i] - '0');
    }
    amount *= factor;

    *p += amount;
    fileHandler.updatePatient(p);

    char balBuf[20]; floatToStr(p->getBalance(), balBuf);
    copyStr(outMsg, "Balance updated. New balance: PKR ");
    int i = strLen(outMsg);
    for (int j = 0; balBuf[j]; j++) outMsg[i++] = balBuf[j];
    outMsg[i] = '\0';
}

void Hospital::guiMarkComplete(Doctor* d, const char* apptIDStr, char* outMsg)
{
    char today[11]; getTodayDate(today);
    int apptID = strToInt(apptIDStr);
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getApptID() == apptID && a->getDoctorID() == d->getID()
            && strEqual(a->getStatus(), "pending") && strEqual(a->getDate(), today))
        {
            a->setStatus("completed");
            fileHandler.updateAppointment(a);
            copyStr(outMsg, "Appointment marked as completed."); return;
        }
    }
    copyStr(outMsg, "Invalid appointment ID (must be today's pending appointment).");
}

void Hospital::guiMarkNoShow(Doctor* d, const char* apptIDStr, char* outMsg)
{
    char today[11]; getTodayDate(today);
    int apptID = strToInt(apptIDStr);
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getApptID() == apptID && a->getDoctorID() == d->getID()
            && strEqual(a->getStatus(), "pending") && strEqual(a->getDate(), today))
        {
            a->setStatus("no-show");
            fileHandler.updateAppointment(a);
            for (int j = 0; j < bills.size(); j++)
            {
                Bill* b = bills.getDataAtIndex(j);
                if (b && b->getAppointmentID() == apptID)
                {
                    b->setStatus("cancelled"); fileHandler.updateBill(b); break;
                }
            }
            copyStr(outMsg, "Appointment marked as no-show."); return;
        }
    }
    copyStr(outMsg, "Invalid appointment ID (must be today's pending appointment).");
}

void Hospital::guiWritePrescription(Doctor* d, const char* apptIDStr,
    const char* meds, const char* notes, char* outMsg)
{
    int apptID = strToInt(apptIDStr);
    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getApptID() == apptID && a->getDoctorID() == d->getID()
            && strEqual(a->getStatus(), "completed"))
        {
            target = a; break;
        }
    }
    if (!target)
    {
        copyStr(outMsg, "Invalid ID (must be your completed appointment)."); return;
    }

    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (pr && pr->getAppointmentID() == apptID)
        {
            copyStr(outMsg, "Prescription already exists for this appointment."); return;
        }
    }

    char today[11]; getTodayDate(today);
    int presID = fileHandler.generateNewID("prescriptions.txt");
    Prescription* pr = new Prescription(presID, apptID,
        target->getPatientID(), d->getID(),
        today, meds, notes);
    pr->setPatient(patients.findByID(target->getPatientID()));
    pr->setDoctor(d);
    prescriptions.add(pr);
    fileHandler.savePrescription(pr);
    copyStr(outMsg, "Prescription saved successfully.");
}

void Hospital::guiViewPatientHistory(Doctor* d, const char* patIDStr, char* outMsg)
{
    int patID = strToInt(patIDStr);
    Patient* pt = patients.findByID(patID);
    if (!pt) { copyStr(outMsg, "Patient not found."); return; }

    bool hasAccess = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getPatientID() == patID && a->getDoctorID() == d->getID()
            && strEqual(a->getStatus(), "completed"))
        {
            hasAccess = true; break;
        }
    }
    if (!hasAccess)
    {
        copyStr(outMsg, "Access denied. No completed appointments with this patient."); return;
    }

    outMsg[0] = '\0';
    int pos = 0;
    auto app = [&](const char* s)
        {
            for (int i = 0; s[i] && pos < 1998; i++) outMsg[pos++] = s[i];
            outMsg[pos] = '\0';
        };

    bool any = false;
    for (int i = 0; i < prescriptions.size(); i++)
    {
        Prescription* pr = prescriptions.getDataAtIndex(i);
        if (!pr || pr->getPatientID() != patID || pr->getDoctorID() != d->getID()) continue;
        app("Date: "); app(pr->getDate()); app("\n");
        app("Medicines: "); app(pr->getMedicines()); app("\n");
        app("Notes: "); app(pr->getNotes()); app("\n\n");
        any = true;
    }
    if (!any) copyStr(outMsg, "No prescriptions found for this patient.");
}

void Hospital::guiAddDoctor(const char* name, const char* spec,
    const char* contact, const char* pass,
    const char* feeStr, char* outMsg)
{
    if (!validator.check_contact(contact))
    {
        copyStr(outMsg, "Invalid contact (must be 11 digits)."); return;
    }
    if (!validator.check_password(pass))
    {
        copyStr(outMsg, "Invalid password (min 6 chars)."); return;
    }
    if (!validator.check_positivefloat(feeStr))
    {
        copyStr(outMsg, "Invalid fee."); return;
    }

    float fee = 0.0f, factor = 1.0f; bool dec = false;
    for (int i = 0; feeStr[i]; i++)
    {
        if (feeStr[i] == '.') { dec = true; continue; }
        if (dec) factor /= 10.0f;
        fee = fee * 10.0f + (feeStr[i] - '0');
    }
    fee *= factor;

    int newID = fileHandler.generateNewID("doctors.txt");
    Doctor* d = new Doctor(newID, name, spec, contact, pass, fee);
    doctors.add(d);
    fileHandler.saveDoctor(d);

    char idBuf[20]; intToStr(newID, idBuf);
    copyStr(outMsg, "Doctor added. ID: ");
    int i = strLen(outMsg);
    for (int j = 0; idBuf[j]; j++) outMsg[i++] = idBuf[j];
    outMsg[i] = '\0';
}

void Hospital::guiRemoveDoctor(int docID, char* outMsg)
{
    Doctor* d = doctors.findByID(docID);
    if (!d) { copyStr(outMsg, "Doctor not found."); return; }

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a) continue;
        if (a->getDoctorID() == docID && strEqual(a->getStatus(), "pending"))
        {
            copyStr(outMsg, "Cannot remove: doctor has pending appointments."); return;
        }
    }
    fileHandler.deleteDoctor(docID);
    doctors.removeByID(docID);
    copyStr(outMsg, "Doctor removed successfully.");
}

void Hospital::guiAddPatient(const char* name, const char* pass,
    const char* ageStr, const char* genderStr,
    const char* contact, const char* balStr, char* outMsg)
{
    if (!validator.check_password(pass))
    {
        copyStr(outMsg, "Invalid password (min 6 chars)."); return;
    }
    if (!validator.check_contact(contact))
    {
        copyStr(outMsg, "Invalid contact (must be 11 digits)."); return;
    }

    int age = strToInt(ageStr);
    char gender = genderStr[0];

    float balance = 0.0f, factor = 1.0f; bool dec = false;
    for (int i = 0; balStr[i]; i++)
    {
        if (balStr[i] == '.') { dec = true; continue; }
        if (dec) factor /= 10.0f;
        balance = balance * 10.0f + (balStr[i] - '0');
    }
    balance *= factor;

    int newID = fileHandler.generateNewID("patients.txt");
    Patient* p = new Patient(newID, name, pass, age, gender, contact, balance);
    patients.add(p);
    fileHandler.updatePatient(p); // updatePatient rewrites the file since patient isn't
    // in the file yet we append directly:
    ofstream f("patients.txt", ios::app);
    if (f)
    {
        char idStr[10], ageStr2[10], balStr2[20];
        intToStr(newID, idStr); intToStr(age, ageStr2); floatToStr(balance, balStr2);
        f << idStr << "," << name << "," << pass << "," << ageStr2 << ","
            << gender << "," << contact << "," << balStr2 << "\n";
    }
    f.close();

    char idBuf[20]; intToStr(newID, idBuf);
    copyStr(outMsg, "Patient added. ID: ");
    int i = strLen(outMsg);
    for (int j = 0; idBuf[j]; j++) outMsg[i++] = idBuf[j];
    outMsg[i] = '\0';
}

void Hospital::guiRemovePatient(const char* patIDStr, char* outMsg)
{
    int patID = strToInt(patIDStr);
    Patient* p = patients.findByID(patID);
    if (!p) { copyStr(outMsg, "Patient not found."); return; }

    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b && b->getPatientID() == patID && strEqual(b->getStatus(), "unpaid"))
        {
            copyStr(outMsg, "Cannot remove: patient has unpaid bills."); return;
        }
    }
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a && a->getPatientID() == patID && strEqual(a->getStatus(), "pending"))
        {
            copyStr(outMsg, "Cannot remove: patient has pending appointments."); return;
        }
    }
    fileHandler.archivePatient(patID);
    patients.removeByID(patID);
    copyStr(outMsg, "Patient removed and archived.");
}

void Hospital::guiDischarge(const char* patIDStr, char* outMsg)
{
    int patID = strToInt(patIDStr);
    Patient* p = patients.findByID(patID);
    if (!p) { copyStr(outMsg, "Patient not found."); return; }

    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b && b->getPatientID() == patID && strEqual(b->getStatus(), "unpaid"))
        {
            copyStr(outMsg, "Cannot discharge: patient has unpaid bills."); return;
        }
    }
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (a && a->getPatientID() == patID && strEqual(a->getStatus(), "pending"))
        {
            copyStr(outMsg, "Cannot discharge: patient has pending appointments."); return;
        }
    }

    fileHandler.archivePatient(patID);
    patients.removeByID(patID);
    copyStr(outMsg, "Patient discharged and archived successfully.");
}

void Hospital::guiSecurityLog(char* outBuf, int bufSize)
{
    ifstream f("security_log.txt");
    if (!f) { copyStr(outBuf, "No security events logged."); return; }

    int pos = 0;
    char line[300];
    while (f.getline(line, 300) && pos < bufSize - 2)
    {
        for (int i = 0; line[i] && pos < bufSize - 2; i++)
            outBuf[pos++] = line[i];
        outBuf[pos++] = '\n';
    }
    outBuf[pos] = '\0';
    if (pos == 0) copyStr(outBuf, "No security events logged.");
    f.close();
}

void Hospital::guiDailyReport(char* outBuf, int bufSize)
{
    char today[11]; getTodayDate(today);
    int pos = 0;

    auto app = [&](const char* s)
        {
            for (int i = 0; s[i] && pos < bufSize - 2; i++)
                outBuf[pos++] = s[i];
            outBuf[pos] = '\0';
        };
    auto appNL = [&]() { if (pos < bufSize - 2) outBuf[pos++] = '\n'; outBuf[pos] = '\0'; };

    app("=== Daily Report: "); app(today); app(" ==="); appNL();

    int tot = 0, pend = 0, comp = 0, ns = 0, canc = 0;
    float rev = 0.0f;

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment* a = appointments.getDataAtIndex(i);
        if (!a || !strEqual(a->getDate(), today)) continue;
        tot++;
        if (strEqual(a->getStatus(), "pending"))   pend++;
        else if (strEqual(a->getStatus(), "completed")) comp++;
        else if (strEqual(a->getStatus(), "no-show"))   ns++;
        else if (strEqual(a->getStatus(), "cancelled")) canc++;
    }
    for (int i = 0; i < bills.size(); i++)
    {
        Bill* b = bills.getDataAtIndex(i);
        if (b && strEqual(b->getStatus(), "paid") && strEqual(b->getDate(), today))
            rev += b->getAmount();
    }

    char nbuf[20];
    app("Total Appointments: "); intToStr(tot, nbuf); app(nbuf); appNL();
    app("  Completed: "); intToStr(comp, nbuf); app(nbuf); appNL();
    app("  Pending  : "); intToStr(pend, nbuf); app(nbuf); appNL();
    app("  No-Show  : "); intToStr(ns, nbuf);   app(nbuf); appNL();
    app("  Cancelled: "); intToStr(canc, nbuf); app(nbuf); appNL();
    app("Revenue (PKR): "); floatToStr(rev, nbuf); app(nbuf); appNL();
    appNL();

    app("--- Patients with Unpaid Bills ---"); appNL();
    for (int i = 0; i < patients.size(); i++)
    {
        Patient* p = patients.getDataAtIndex(i);
        if (!p) continue;
        float owed = 0.0f;
        for (int j = 0; j < bills.size(); j++)
        {
            Bill* b = bills.getDataAtIndex(j);
            if (b && b->getPatientID() == p->getID() && strEqual(b->getStatus(), "unpaid"))
                owed += b->getAmount();
        }
        if (owed > 0.0f)
        {
            app(p->getName()); app(": PKR ");
            floatToStr(owed, nbuf); app(nbuf); appNL();
        }
    }

    appNL();
    app("--- Doctor Summary ---"); appNL();
    for (int i = 0; i < doctors.size(); i++)
    {
        Doctor* d = doctors.getDataAtIndex(i);
        if (!d) continue;
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appointments.size(); j++)
        {
            Appointment* a = appointments.getDataAtIndex(j);
            if (!a || a->getDoctorID() != d->getID() || !strEqual(a->getDate(), today)) continue;
            if (strEqual(a->getStatus(), "completed")) dc++;
            else if (strEqual(a->getStatus(), "pending")) dp++;
            else if (strEqual(a->getStatus(), "no-show")) dn++;
        }
        if (dc + dp + dn > 0)
        {
            app(d->getName()); app(": C=");
            intToStr(dc, nbuf); app(nbuf);
            app(" P="); intToStr(dp, nbuf); app(nbuf);
            app(" N="); intToStr(dn, nbuf); app(nbuf); appNL();
        }
    }
}