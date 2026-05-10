#include "Exceptions.h"
#include <iostream>
using namespace std;

HospitalException::HospitalException(const char* msg) 
{
    int i = 0;
    while (msg[i] != '\0' && i < 199) 
    {
        message[i] = msg[i];
        i++;
    }
    message[i] = '\0';
}

const char* HospitalException::what() const 
{
    return message;
}

FileNotFoundException::FileNotFoundException() : HospitalException("Error: Required data file not found.") 
{

}

InsufficientFundsException::InsufficientFundsException() : HospitalException("Transaction Failed: Insufficient balance.") 
{

}

SlotUnavailableException::SlotUnavailableException() : HospitalException("Booking Error: Selected time slot is already taken.") 
{

}

InvalidInputException::InvalidInputException(const char* msg) : HospitalException(msg) 
{

}