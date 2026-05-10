#include "Validator.h"

bool Validator::check_menu(int choice, int min, int max)
{
	if (choice >= min && choice <= max)
	{
		return true;
	}
	return false;
}

bool Validator::check_ID (const char* ID)
{
	if (ID[0] == '\0' || ID == nullptr)
	{
		return false;
	}
	for (int i = 0; ID[i] != '\0'; i++) 
	{
		if (ID[i] < '0' || ID[i] > '9')
		{
			return false;
		}
	}
	return true;
}

bool Validator::check_password (const char* password)
{
	if (password == nullptr || password[0] == '\0')
	{
		return false;
	}

	int n = 0;
	while (password[n] != '\0') 
	{
		n++;
	}
	if (n < 6)
	{
		return false;
	}
	return true;
}

bool Validator::check_contact(const char* contact)
{
	int n = 0;
	while (contact[n] != '\0') 
	{
		if (contact[n] < '0' || contact[n] > '9') return false;
		n++;
	}
	if (n == 11)
	{
		return true;
	}
	return false;
}

bool Validator::check_positivefloat (const char* n)
{
	if (n[0] == '\0' || n == nullptr)  // check if string is empty
	{
		return false;
	}

	int decimal = 0; //number of decimals
	int decimalPosition = -1; //decimal is at index -1 meaning its out of bound/does not exist yet
	int len = 0; //to traverse through the array and also find the index of the decimal

	while (n[len] != '\0') 
	{
		if (n[len] == '.') 
		{
			decimal++;
			decimalPosition = len;
			if (decimal > 1)
			{
				return false;
			}
		}
		else if (n[len] < '0' || n[len] > '9') 
		{
			return false;
		}

		len++;
	}

	if (decimal == 1 && decimalPosition == len - 1) //decimal can not be at the last index
	{
		return false;
	}

	float result = 0;
	int j = 0;
	while (n[j] != '\0' && n[j] != '.') 
	{
		result = result * 10 + (n[j] - '0');
		j++;
	}

	if (result > 0 || (n[0] == '0' && decimal == 1))
	{
		return true;
	}
	return false;
}

bool Validator::check_date(const char* date)
{
	int len = 0;
	while (date[len] != '\0')
	{
		len++;
	}
	if (len != 10)
	{
		return false;
	}

	if (date[2] != '-' || date[5] != '-')
	{
		return false;
	}

	for (int i = 0; i < 10; i++) 
	{
		if (i == 2 || i == 5)
		{
			continue;
		}
		if (date[i] < '0' || date[i] > '9')
		{
			return false;
		}
	}

	int day = (date[0] - '0') * 10 + (date[1] - '0');
	int month = (date[3] - '0') * 10 + (date[4] - '0');
	int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');

	if (year < 2026 || year > 2100)
	{
		return false;
	}
	if (month < 1 || month > 12)
	{
		return false;
	}
	if (day < 1 || day > 31)
	{
		return false;
	}

	if (month == 4 || month == 6 || month == 9 || month == 11) 
	{
		if (day > 30)
		{
			return false;
		}
	}

	if (month == 2) 
	{
		bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
		int maxDay = isLeap ? 29 : 28;
		if (day > maxDay) return false;
	}

	return true;
}

bool Validator::check_time(const char* time)
{
	if (time == nullptr || time[0] == '\0') 
	{
		return false;
	}

	int len = 0;
	while (time[len] != '\0')
	{
		len++;
	}
	if (len != 5)
	{
		return false;
	}
	if (time[2] != ':')
	{
		return false;
	}

	for (int i = 0; i < 5; i++) 
	{
		if (i == 2) //this skips the colon
		{
			continue;
		}

		if (time[i] < '0' || time[i] > '9')
		{
			return false;
		}
	}

	int hour = (time[0] - '0') * 10 + (time[1] - '0');
	int minute = (time[3] - '0') * 10 + (time[4] - '0');

	if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
	{
		return false;
	}

	const char* timeSlots[8] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };

	for (int i = 0; i < 8; i++) 
	{
		if (isEqual(time, timeSlots[i])) 
		{
			return true;
		}
	}
	return false;
}

bool Validator::isEqual(const char* s1, const char* s2) 
{
	int i = 0;
	while (s1[i] != '\0' && s2[i] != '\0') 
	{
		if (s1[i] != s2[i])
		{
			return false;
		}
		i++;
	}
	if (s1[i] == '\0' && s2[i] == '\0')
	{
		return true;
	}
	return false;
}