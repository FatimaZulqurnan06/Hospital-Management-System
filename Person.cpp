#include"Person.h"

Person::Person(int id, const char* n, const char* p)
{
	ID = id;

	int i = 0;

	while (n[i] != '\0') //finding the size of the name array
	{
		i++;
	}

	name = new char[i + 1]; //new array

	for (int j = 0; j < i; j++) //copying the name into the new array
	{
		name[j] = n[j];
	}
	name[i] = '\0';

	int k = 0;
	while (p[k] != '\0') //password size
	{
		k++;
	}

	password = new char[k + 1]; //new array

	for (int l = 0; l < k; l++) //copying the password into new array
	{
		password[l] = p[l];
	}
	password[k] = '\0';
}

int Person::getID() const 
{
	return ID;
}

const char* Person::getName() const
{
	return name;
}

const char* Person::getPassword() const
{
	return password;
}

Person::~Person()
{
	delete[] name;
	delete[] password;
	name = nullptr;
	password = nullptr;
}