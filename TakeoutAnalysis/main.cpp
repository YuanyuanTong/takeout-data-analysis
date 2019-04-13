#include "conversations.h"
#include "mapsactivities.h"

int main() {

	cout << "Preparing to load the JSON file..." << endl;
	// a list to store all conversations in the json file
	ConversationList conversation_list = ReadHangoutsJson("Hangouts.json");
	cout << "JSON file loaded..." << endl;

	cout << "Preparing to load the maps activity file..." << endl;
	// a list of all Maps activities
	MapsActivityList maps_activity_list = ReadMapsActivityHtml("MyActivity.html");
	cout << "Maps file loaded..." << endl;

	// printing some representative information of the chat history

	// first hangout conversation
	Conversation first_hangout;
	cout << "Since you were first in Hangouts on ";
	conversation_list.GetFirstContact(first_hangout).PrintDate();
	cout << " to chat with ";
	first_hangout.PrintParticipants();
	cout << ", you have made " << conversation_list.Size() << " conversations in Hangouts." << endl;

	// most frequently used conversation
	Conversation most_visited_con;
	int visit_times = conversation_list.GetFrequentContact(most_visited_con);
	cout << "You are most frequently contacted with ";
	most_visited_con.PrintParticipants();
	cout << ", sending " << visit_times << " messages to each other in total." << endl;

	// most chatting day
	Conversation most_chatting_con;
	Time chatting_time = conversation_list.GetMostChattingDay(most_chatting_con);
	cout << "You changed " << most_chatting_con.GetEvents(chatting_time).size() << " messages with ";
	most_chatting_con.PrintParticipants();
	cout << " on ";
	chatting_time.PrintDate();
	cout << ". " << endl;
	cout << "It must be a *** day..." << endl; // extract meaning from the messages by machine learning???

	// a random prompt about a specific event
	Conversation location_con;
	Event location_event;
	conversation_list.GetType(Event::LOCATION, location_con, location_event);
	cout << "Do you still remember that on ";
	ConvertTime(location_event.GetTimestamp()).PrintDate();
	cout << " you shared a location ";
	cout << location_event.GetTypeAttachment(Event::LOCATION);
	cout << " with ";
	location_con.PrintParticipants();
	cout << "?" << endl;

	while (true) {
		// exploring more around a specific date
		char* p;
		cout << "Do you want to explore more about a specific day? (Y/N)" << endl;
		p = new char[5];
		cin >> p;
		if (strcmp(p, "Y") == 0 || strcmp(p, "y") == 0) {
			cout << "Input the date you want to explore (mm/dd/yyyy)." << endl;
			p = new char[20];
			cin >> p;
			// did not check input legality yet ...
			Convert2Timestamp(p, true);
			MapsActivityList exploration_maps_list = maps_activity_list.GetPeriodActivities(p);
			ConversationList exploration_conversation_list = conversation_list.GetPeriodConversations(p);
			cout << "On " << p << "," << endl;
			if (exploration_maps_list.Size() || exploration_conversation_list.Size()) {
				exploration_conversation_list.PrintInformation();
				exploration_maps_list.PrintInformation();
			}
			else {
				cout << "you did nothing." << endl;
			}

		}
		else {
			break;
		}
	}

	system("pause");
	return 0;
}