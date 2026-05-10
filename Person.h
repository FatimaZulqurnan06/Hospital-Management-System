#ifndef PERSON_H
#define PERSON_H
class Person
{
protected:
	int ID = 0;
	char* name;
	char* password;

public:
	Person(int id, const char* name, const char* password);
    int getID() const;
    const char* getName() const;
    const char* getPassword() const;
	virtual void displayMenu() = 0;
    virtual void displayDetails() = 0;
	virtual ~Person();
};
#endif