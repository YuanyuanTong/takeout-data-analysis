#include "mytime.h"

#define MONTH_COUNT 12
#define EST -5;

// ************** Time ***********************

// constructor
Time::Time(int mo, int dy, int yr, int hr, int min, int sec) {
	month = mo;
	day = dy;
	year = yr;
	hour = hr;
	minute = min;
	second = sec;
}

// copy constructor
Time::Time(const Time &tt) {
	month = tt.month;
	day = tt.day;
	year = tt.year;
	hour = tt.hour;
	minute = tt.minute;
	second = tt.second;
}

// prints the date
void Time::PrintDate() const {
	cout << year << "/" << month << "/" << day;
}

// checks if the given timestamp is on the same day with the current Time
bool Time::SameDate(long long ts) {
	Time time = ConvertTime(ts);
	return (day == time.day && month == time.month && year == time.year);
}

// checks if the two Times is on the same date
bool SameDate(long long ts1, long long ts2) {
	Time t1 = ConvertTime(ts1);
	Time t2 = ConvertTime(ts2);
	return (t1.day == t2.day && t1.month == t2.month && t1.year == t2.year);
}

// converts the timestamp to standard time format in local time
Time ConvertTime(long long timestamp) {

	time_t tick = (time_t)(timestamp / 1000000);
	struct tm tm;
	char s[50];

	Time local_time;

	tm = *localtime(&tick);

	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);

	local_time.year = atoi(s);
	local_time.month = atoi(s + 5);
	local_time.day = atoi(s + 8);
	local_time.hour = atoi(s + 11);
	local_time.minute = atoi(s + 14);
	local_time.second = atoi(s + 17);

	return local_time;
}

// output operator for Time
std::ostream& operator<<(std::ostream& os, const Time& t) {
	os << t.hour << ":" << t.minute << ":" << t.second << " " << t.month << "/" << t.day << "/" << t.year;
	return os;
}

// converts a time string to a timestamp
long long Convert2Timestamp(const char* time_string, bool local_input)
{
	long long timestamp;
	char* str = new char[strlen(time_string)];
	strcpy(str, time_string);

	// Convert 
	if (local_input) {

		struct tm * timeinfo;
		timeinfo = new tm();
		char *pch = strtok(str, "/");
		timeinfo->tm_mon = atoi(pch) - 1;
		pch = strtok(NULL, "/");
		timeinfo->tm_mday = atoi(pch);
		pch = strtok(NULL, "/");
		timeinfo->tm_year = atoi(pch) - 1900;
		timeinfo->tm_hour = 12;

		// get timestamp from the timeinfo
		timestamp = mktime(timeinfo);

		// in microseconds, to be compatible with Hangouts.json timestamp
		return timestamp * 1000000LL;

	}
	else {
		vector<char *> results;
		char *pch = strtok(str, ",");
		while (pch != NULL)
		{
			results.push_back(pch);
			pch = strtok(NULL, " ,");
		}


		const char* month_name[] =
		{ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		struct tm * timeinfo;

		// get timeinfo from the string
		timeinfo = new tm();
		// year
		timeinfo->tm_year = atoi(results[1]) - 1900;
		// month
		pch = strtok(results[0], " ");
		for (int i = 0; i < MONTH_COUNT; ++i) {
			if (strcmp(pch, month_name[i]) == 0) {
				timeinfo->tm_mon = i;
			}
		}
		// day
		pch = strtok(NULL, " ");
		timeinfo->tm_mday = atoi(pch);
		// hour
		pch = strtok(results[2], ":");
		timeinfo->tm_hour = atoi(pch);
		// minute
		pch = strtok(NULL, ":");
		timeinfo->tm_min = atoi(pch);
		// second
		pch = strtok(NULL, ":");
		timeinfo->tm_sec = atoi(pch);

		// convert time from 12-hour clock to 24-hour clock
		if (strcmp(results[3], "PM") == 0) {
			if (timeinfo->tm_hour != 12) {
				timeinfo->tm_hour += 12;
			}
		}
		else {
			if (timeinfo->tm_hour == 12) {
				timeinfo->tm_hour = 0;
			}
		}

		// convert from EST (Google takeout html standard) to UTC
		if (strcmp(results[4], "EST") == 0) {
			timeinfo->tm_hour -= EST;
		}

		// get timestamp from the timeinfo
		timestamp = _mkgmtime64(timeinfo);

		// in microseconds, to be compatible with Hangouts.json timestamp
		return timestamp * 1000000LL;
	}
	
}

// converts a Time to a timestamp
long long Convert2Timestamp(Time time) {
	struct tm * timeinfo;
	timeinfo = new tm();

	timeinfo->tm_mon = time.month - 1;
	timeinfo->tm_mday = time.day;
	timeinfo->tm_year = time.year - 1900;
	timeinfo->tm_hour = 12;

	// get timestamp from the timeinfo
	long long timestamp = mktime(timeinfo);

	// in microseconds, to be compatible with Hangouts.json timestamp
	return timestamp * 1000000LL;

}