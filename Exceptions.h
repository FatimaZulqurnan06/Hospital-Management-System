#ifndef HOSPITALEXCEPTION_H
#define HOSPITALEXCEPTION_H

class HospitalException 
{
protected:
    char message[200];

public:
    HospitalException(const char* msg);
    virtual const char* what() const;
    virtual ~HospitalException() {}
};

class FileNotFoundException : public HospitalException 
{
public:
    FileNotFoundException();
};

class InsufficientFundsException : public HospitalException 
{
public:
    InsufficientFundsException();
};

class SlotUnavailableException : public HospitalException 
{
public:
    SlotUnavailableException();
};

class InvalidInputException : public HospitalException 
{
public:
    InvalidInputException(const char* msg);
};

#endif