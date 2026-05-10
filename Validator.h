#ifndef VALIDATOR_H
#define VALIDATOR_H

class Validator

{
public:

	bool check_menu(int choice, int min, int max);
	bool check_ID(const char* ID);
	bool check_password(const char* password);
	bool check_contact(const char* contact);
	bool check_positivefloat(const char* n);
	bool check_date(const char* date);
	bool check_time(const char* time);
	bool isEqual(const char* s1, const char* s2);
};
#endif