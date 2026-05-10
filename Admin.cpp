#include"Admin.h"
#include<iostream>
using namespace std;

Admin::Admin(int ID, const char* name, const char* password) : Person (ID, name, password)
{
	
}

void Admin::displayDetails ()
{
	cout << "ID      : " << ID << endl;
	cout << "Name    : " << name << endl;
}

void Admin::displayMenu()
{
	cout << "Admin Panel — MediCore" << endl;
	cout << "======================" << endl;
	cout << " 1. Add Doctor" << endl;
	cout << " 2. Remove Doctor" << endl;
	cout << " 3. Add Patient" << endl;
	cout << " 4. Remove Patient" << endl;
	cout << " 5. View All Patients" << endl;
	cout << " 6. View All Doctors" << endl;
	cout << " 7. View All Appointments" << endl;
	cout << " 8. View Unpaid Bills" << endl;
	cout << " 9. Discharge Patient" << endl;
	cout << "10. View Security Log" << endl;
	cout << "11. Generate Daily Report" << endl;
	cout << "12. Logout" << endl;
}

Admin::~Admin()
{

}