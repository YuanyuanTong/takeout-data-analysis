#pragma once

#include <iostream>
#include <vector>
#include <time.h>

using namespace std;

// ************************** Time **********************************
class Time {

	int month;
	int day;
	int year;
	int hour;
	int minute;
	int second;

public:
	// constructor
	Time(int mo = 0, int dy = 0, int yr = 0, int hr = 0, int min = 0, int sec = 0);

	// copy constructor
	Time(const Time &tt);

	// prints the date
	void PrintDate() const;

	// checks if the given timestamp is on the same day with the current Time
	bool SameDate(long long ts);

	// checks if the two Times is on the same date
	friend bool SameDate(long long ts1, long long ts2);

	// converts the timestamp to standard time format in local time
	friend Time ConvertTime(long long timestamp);

	// output operator for Time
	friend std::ostream& operator<<(std::ostream& os, const Time& t);

	// converts a Time to a timestamp
	friend long long Convert2Timestamp(Time time);
};

// checks if the two times is on the same date
bool SameDate(long long ts1, long long ts2);
// converts the timestamp to standard time format in local time
Time ConvertTime(long long timestamp);
// output operator for Time
std::ostream& operator<<(std::ostream& os, const Time& t);

// converts a time string to a timestamp
long long Convert2Timestamp(const char* time_string, bool local_input = false);

// converts a Time to a timestamp
long long Convert2Timestamp(Time time);