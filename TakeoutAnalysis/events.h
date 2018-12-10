#pragma once
#include <iostream>
#include <vector>
#include "participants.h"
using namespace std;

// ******************** Attachment ***************************************
// An attachment can be a photo or video or maybe other files??
class Attachment {
	char * name;
	char * link;
public:
	enum Type { PHOTO, VIDEO, LOCATION, LINK };
	// constructor
	Attachment(const char* au, Type at, const char* an = "");

	// gets the attachment link
	char* GetLink() const;

	// gets the attachment type
	Type GetType() const;

	// output operator for Time
	friend ostream& operator<<(std::ostream& os, const Attachment& a);

private:
	Type attachment_type;
};

// output operator for Time
ostream& operator<<(std::ostream& os, const Attachment& a);


// ******************* HangoutEvent **********************
class HangoutEvent {
	int duration_secs;
	vector<Participant> participants;

public:
	enum EventType { START_HANGOUT, END_HANGOUT };
	enum MediaType { AUDIO_ONLY };

	// constructor
	HangoutEvent(EventType et = START_HANGOUT, MediaType mt = AUDIO_ONLY, int ds = 0);

	// adds participants to the hangout event
	void AddParticipant(const char* participant_id);

private:
	EventType event_type;
	MediaType media_type;
};


// ****************** Event (linked with a single message) *********************************
// An event is basically a text message, but can also contain urls, photos and videos.
// It can also be location information, linked to a Google Maps url.
// It can also be a Hangout call, video or audio.
class Event {

	char* sender_id;
	long long timestamp;
	bool is_hangout_event;
	vector<char*> texts;
	vector<Attachment> attachments;
	HangoutEvent* hangout_event;

public:
	//
	enum Type { TEXT, PHOTO, VIDEO, CALL, LOCATION, URL };
	// constructor
	Event(const char* si = "", long long ts = 0);

	// if the event is a hangout call event
	bool IsHangoutEvent() const;

	// adds a new text segment
	void AddText(const char* txt);

	// adds an attachment
	void AddAttachment(const char* a_link, Attachment::Type a_type, const char* a_name = "");

	// sets the current event as a hangout call event
	void SetHangoutEvent(HangoutEvent* he);

	//gets the sender id
	char * GetSenderId() const;

	// gets the timestamp of the event
	long long GetTimestamp() const;

	// checks if the event contains the give type content
	bool IsEventType(Type type) const;

	// gets the k'th attachment of a specific type
	Attachment GetTypeAttachment(Type t, int k = 0);

	// gets the text
	vector<char*> GetTexts() const;
};

