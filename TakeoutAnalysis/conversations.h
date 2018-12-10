#pragma once
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "events.h"
#include "participants.h"
#include "mytime.h"

using namespace std;


// **************** Conversation ********************************
class Conversation {

	// self gaia id
	char* self_id;
	// self name
	char* self_name;
	//the inviter of the conversation
	char* inviter_id;
	// when were the participants first invited to the conversation
	long long invite_timestamp;
	// when was the conversation last read
	long long latest_read_timestamp;
	// if the current conversation is a group chat (more than 2 people)
	bool group_chat;

	// a list of events
	vector<Event> events;
	// a list of participants in the current conversation
	vector<Participant> participants;

public:
	// default constructor
	Conversation();
	// constructs a conversation from parameters
	Conversation(const char* ii, long long it, long long lt, bool group = false, const char* si = "User0");
	// copy constructor
	Conversation(const Conversation& con);

	// adds a new participant to the participant list (excluding self)
	void AddParticipant(const char* participant_id, const char* participant_name);
	// adds a new event to the event list
	void AddEvent(Event event);
	// edits the self name
	void SelfName(const char* sn = "N.A.");
	// counts the events in a conversation
	int EventCount() const;
	// sets the events
	void SetEvents(vector<Event> new_events);

	// prints all the participants
	void PrintParticipants() const;
	// prints all the events
	void PrintEvents() const;
	// checks if the conversation was started by self
	bool SelfStarted() const;
	// gets the invite timestamp
	long long GetInviteTimestamp() const;
	// gets the latest read timestamp;
	long long GetLatestReadTimestamp() const;
	// gets the inviter id;
	char * GetInviterId() const;
	// checks if the conversation is group chat;
	bool IsGroupChat() const;
	// gets all the participants
	vector<Participant> GetParticipants() const;
	// gets all the events
	vector<Event> GetEvents() const;
	// gets events around the given Time range
	vector<Event> GetEvents(Time t, int range = 0) const;
	// gets a specific type of events
	vector<Event> GetEvents(Event::Type t) const;

};


// **************** ConversationList ********************************
class ConversationList {
	vector<Conversation> conversations;
public:
	// constructor
	ConversationList() {}
	// gets the total number of conversations
	int Size() const;
	// adds a new conversation to the list
	void AddConversation(const Conversation & con);
	// gets the most frequently visited conversation
	int GetFrequentContact(Conversation &con) const;
	// gets the earliest started conversation
	Time GetFirstContact(Conversation &con) const;
	// gets the most chatting day (Time) in terms of messages exchanged in a single conversation
	Time GetMostChattingDay(Conversation &con) const;
	// gets a random event of a specific type, return false if there is no
	bool GetType(Event::Type t, Conversation &con, Event &ev) const;
	// gets a subset of the conversations happened during a specific period
	ConversationList GetPeriodConversations(const char* time_string, int range = 0);
	// prints information
	void PrintInformation() const;
};


// reads a Hangouts.json file and return a conversation list
ConversationList ReadHangoutsJson(const char * file_name);