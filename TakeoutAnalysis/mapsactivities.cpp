#include "mapsactivities.h"

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "rapidxml-1.13/rapidxml_iterators.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

using namespace rapidxml;

// ************************ MapsActivity ***********************
// constructor
MapsActivity::MapsActivity() {
	type = SEARCH; // default type
	timestamp = -1LL;
}

// copy constructor
MapsActivity::MapsActivity(const MapsActivity& ma) {
	type = ma.type;
	name = ma.name;
	url = ma.url;
	current_location = ma.current_location;
	from_to_locations = ma.from_to_locations;
	timestamp = ma.timestamp;
}

// sets the activity name
void MapsActivity::SetName(const char* an) {
	name = new char[strlen(an)];
	strcpy(name, an);
}

// sets the activity URL
void MapsActivity::SetUrl(const char* au) {
	url = new char[strlen(au)];
	strcpy(url, au);
}

// sets the current location
void MapsActivity::SetCurLoc(const char* ac) {
	current_location = new char[strlen(ac)];
	strcpy(current_location, ac);
}

// sets the from to locations
void MapsActivity::SetFromToLoc(const char* from, const char* to) {
	char * from_loc = new char[strlen(from)];
	strcpy(from_loc, from);
	char * to_loc = new char[strlen(to)];
	strcpy(to_loc, to);
	from_to_locations.push_back(from_loc);
	from_to_locations.push_back(to_loc);
}

// sets the type of the Maps activity
void MapsActivity::SetType(Type at) {
	type = at;
}

// sets the timestamp
void MapsActivity::SetTimestamp(const char* time) {
	timestamp = Convert2Timestamp(time);
}

// gets the activity name
char* MapsActivity::GetName() const {
	return name;
}

// gets the type
MapsActivity::Type MapsActivity::GetType() const {
	return type;
}
// gets the timestamp
long long MapsActivity::GetTimestamp() const {
	return timestamp;
}

// output operator for MapsActivity
std::ostream& operator<<(std::ostream& os, const MapsActivity& t) {
	os << ConvertTime(t.timestamp) << endl;
	vector<char*> from_to = t.from_to_locations;

	switch (t.type)
	{
	case MapsActivity::DIRECTION:
		os << "You searched direction between ";
		for (vector<char*>::const_iterator it = from_to.begin(); it != from_to.end(); ++it) {
			os << "[" << *it << "] ";
		}
		break;
	case MapsActivity::SEARCH:
		os << "You searched for " << "[" << t.name << "]";
		break;
	case MapsActivity::USED_ON:
		os << "You used Maps on " << "[" << t.name << "]";
		break;
	case MapsActivity::VIEW:
		os << "You viewed area around " << "[" << t.name << "]";
		break;
	default:
		break;
	}
	return os;
}

// ************************ MapsActivityList ********************

// adds a new activity to the list
void MapsActivityList::AddActivity(MapsActivity maps_activity) {
	activities.push_back(maps_activity);
}

// gets the latest activity
MapsActivity MapsActivityList::GetLatestActivity() const {
	return activities.back();
}

// gets the size of the list
unsigned int MapsActivityList::Size() const {
	return activities.size();
}

// gets all the activities during a specific period
MapsActivityList MapsActivityList::GetPeriodActivities(const char* time_string, int range) {
	MapsActivityList new_list;
	long long timestamp = Convert2Timestamp(time_string, true);
	for (vector<MapsActivity>::const_iterator it = activities.begin(); it != activities.end(); ++it) {
		long long activity_timestamp = it->GetTimestamp();
		if (range == 0) {
			if (SameDate(timestamp, activity_timestamp)) {
				new_list.AddActivity(*it);
			}
		}
		else {
			if (abs(timestamp - activity_timestamp) < long long(range * 24 * 60 * 60 * 1000000)) {
				new_list.AddActivity(*it);
			}
		}

	}
	return new_list;
}

// prints information of the activity list
void MapsActivityList::PrintInformation() const {
	for (vector<MapsActivity>::const_iterator it = activities.begin(); it != activities.end(); ++it) {
		cout << *it << endl;
	}
}

// trim the heading and trailing whitespace of a string
string trim_whitespace(const char* input_string) {
	string str = input_string;
	str.erase(str.find_last_not_of(" \f\n\r\t\v") + 1);
	str.erase(0, str.find_first_not_of(" \f\n\r\t\v"));
	return str;
}


// reads the Maps MyActivity.html
MapsActivityList ReadMapsActivityHtml(const char* file_name) {

	MapsActivityList maps_activity_list;

	file<> xmlFile(file_name);
	xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	// the out layer <div> node
	xml_node<> *pRoot = doc.first_node()->first_node("body")->first_node("div");

	// a list of all Maps activities

	// each pNode represents a Maps usage activity
	for (xml_node<> *pNode = pRoot->first_node("div"); pNode; pNode = pNode->next_sibling())
	{
		MapsActivity new_activity;
		xml_node<> *pSubNode = pNode->first_node("div")->first_node("div");
		while (strcmp(pSubNode->first_attribute("class")->value(),
			"content-cell mdl-cell mdl-cell--6-col mdl-typography--body-1")) {
			pSubNode = pSubNode->next_sibling();
		}
		xml_node<> *pRefNode = pSubNode->first_node();

		string str = trim_whitespace(pRefNode->value());

		// discard useless type (uninformative one)
		if (str.find("Used&nbsp;Maps") != string::npos) {
			continue;
		}

		// DIRECTION type
		else if (str.find("Directions to") != string::npos) {
			new_activity.SetType(MapsActivity::DIRECTION);

			pRefNode = pRefNode->next_sibling();
			new_activity.SetName(trim_whitespace(pRefNode->value()).data());

			new_activity.SetUrl(pRefNode->first_attribute("href")->value());

			pRefNode = pRefNode->next_sibling()->next_sibling();
			string from_str = trim_whitespace(pRefNode->value());
			pRefNode = pRefNode->next_sibling()->next_sibling();
			string to_str = trim_whitespace(pRefNode->value());
			new_activity.SetFromToLoc(from_str.data(), to_str.data());
		}

		// VIEW type
		else if (str.find("Viewed area around") != string::npos) {
			new_activity.SetType(MapsActivity::VIEW);

			str.erase(0, 19);
			new_activity.SetName(str.data());

			new_activity.SetUrl(pRefNode->first_attribute("href")->value());
		}
		// USED_ON type
		else if (str.find("Used a map on") != string::npos) {
			new_activity.SetType(MapsActivity::USED_ON);

			pRefNode = pRefNode->next_sibling();
			new_activity.SetName(trim_whitespace(pRefNode->value()).data());

			new_activity.SetUrl(pRefNode->first_attribute("href")->value());
		}
		// SEARCH type
		else if (str.find("Searched for") != string::npos) {
			new_activity.SetType(MapsActivity::SEARCH);

			pRefNode = pRefNode->next_sibling();
			new_activity.SetName(trim_whitespace(pRefNode->value()).data());

			new_activity.SetUrl(pRefNode->first_attribute("href")->value());
		}
		else {
			new_activity.SetType(MapsActivity::SEARCH);
			new_activity.SetName(str.data());
			new_activity.SetUrl(pRefNode->first_attribute("href")->value());
		}

		// set the timestamp
		pRefNode = pRefNode->next_sibling()->next_sibling();
		new_activity.SetTimestamp(trim_whitespace(pRefNode->value()).data());

		while (strcmp(pSubNode->first_attribute("class")->value(),
			"content-cell mdl-cell mdl-cell--12-col mdl-typography--caption")) {
			pSubNode = pSubNode->next_sibling();
		}
		pRefNode = pSubNode->last_node("b");
		if (strcmp(pRefNode->value(), "Locations:") == 0) {
			pRefNode = pSubNode->first_node("a");
			new_activity.SetCurLoc(pRefNode->first_attribute("href")->value());
		}

		// ignore the repetitive item caused by internet delay ???
		if (maps_activity_list.Size() > 1) {
			MapsActivity last_activity = maps_activity_list.GetLatestActivity();
			if (last_activity.GetType() == new_activity.GetType()
				&& strcmp(last_activity.GetName(), new_activity.GetName()) == 0) {
				long long time_diff = abs(last_activity.GetTimestamp() - new_activity.GetTimestamp());
				if (time_diff < 30000000) {
					continue;
				}
			}
		}

		maps_activity_list.AddActivity(new_activity);
	}

	return maps_activity_list;
}