#pragma once

#include <iostream>
#include <vector>
#include "mytime.h"

using namespace std;


// ************************ MapsActivity *******************
class MapsActivity {

	// the url linked with the activity
	char* url;
	// the current location, NULL when not available
	char* current_location;
	// "from" and "to" locations, available if it is the DIRECTION activity
	vector<char*> from_to_locations;
	// name of an activity could be
	//		SEARCH: searched place name
	//		USED_ON: third-party website name
	//		VIEW: location name viewed around
	//		DIRECTION: destination name of a searched route
	char* name;
	// the time stamp;
	long long timestamp;

public:

	// constructor
	MapsActivity();

	// copy constructor
	MapsActivity(const MapsActivity& ma);

	// a Maps activity (to the best of my knowledge) could be one of 4 types
	//		SEARCH: searching for some place
	//		USED_ON: Maps was used on a thid-party website
	//		DIRECTION: looking for a route from *** to ***
	//		VIEW: viewing aread around ***
	enum Type { SEARCH, USED_ON, DIRECTION, VIEW };

	// sets the activity name
	void SetName(const char* an);
	// sets the activity URL
	void SetUrl(const char* au);
	// sets the current location
	void SetCurLoc(const char* ac);
	// sets the from to locations
	void SetFromToLoc(const char* from, const char* to);
	// sets the type of the Maps activity
	void SetType(Type at);
	// sets the timestamp
	void SetTimestamp(const char* time);
	// gets the activity name
	char* GetName() const;
	// gets the type
	Type GetType() const;
	// gets the timestamp
	long long GetTimestamp() const;

	// output operator for MapsActivity
	friend std::ostream& operator<<(std::ostream& os, const MapsActivity& t);

private:
	Type type;
};

// output operator for MapsActivity
std::ostream& operator<<(std::ostream& os, const MapsActivity& t);

// ************************ MapsActivityList ********************
class MapsActivityList {
	vector<MapsActivity> activities;
public:
	// constructor
	MapsActivityList() {};
	// adds a new activity to the list
	void AddActivity(MapsActivity maps_activity);
	// gets the latest activity
	MapsActivity GetLatestActivity() const;
	// gets the size of the list
	unsigned int Size() const;
	// gets all the activities during a specific period
	MapsActivityList GetPeriodActivities(const char*, int range = 0);
	// prints information of the activity list
	void PrintInformation() const;

};

// reads the Maps MyActivity.html
MapsActivityList ReadMapsActivityHtml(const char* file_name);