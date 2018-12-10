#include "events.h"
#pragma once
#include <iostream>
#include <vector>
#include "participants.h"
using namespace std;

// ******************** Attachment ***************************************

	// constructor
Attachment::Attachment(const char* au, Type at, const char* an) {
	name = new char[strlen(an)];
	strcpy(name, an);
	link = new char[strlen(au)];
	strcpy(link, au);
	attachment_type = at;
}

// gets the attachment link
char* Attachment::GetLink() const {
	return link;
}
// gets the attachment type
Attachment::Type Attachment::GetType() const {
	return attachment_type;
}

// output operator for Time
ostream& operator<<(std::ostream& os, const Attachment& a) {
	os << "[" << a.name << "]" << " (" << a.link << ")";
	return os;
}



// ******************* HangoutEvent **********************


	// constructor
HangoutEvent::HangoutEvent(HangoutEvent::EventType et, HangoutEvent::MediaType mt, int ds) {
	event_type = et;
	media_type = mt;
	duration_secs = ds;
}

// adds participants to the hangout event
void HangoutEvent::AddParticipant(const char* participant_id) {
	Participant new_participant(participant_id);
	participants.push_back(new_participant);
}



// ****************** Event (linked with a single message) *********************************
// An event is basically a text message, but can also contain urls, photos and videos.
// It can also be location information, linked to a Google Maps url.
// It can also be a Hangout call, video or audio.

	// constructor
Event::Event(const char* si, long long ts) {
	sender_id = new char[strlen(si)];
	strcpy(sender_id, si);
	timestamp = ts;
	is_hangout_event = false;
}

// if the event is a hangout call event
bool Event::IsHangoutEvent() const {
	return is_hangout_event;
}

// adds a new text segment
void Event::AddText(const char* txt) {
	char * new_text = new char[strlen(txt)];
	strcpy(new_text, txt);
	texts.push_back(new_text);
}

// adds an attachment
void Event::AddAttachment(const char* a_link, Attachment::Type a_type, const char* a_name) {
	Attachment new_attachment(a_link, a_type, a_name);
	attachments.push_back(new_attachment);
}

// sets the current event as a hangout call event
void Event::SetHangoutEvent(HangoutEvent* he) {
	is_hangout_event = true;
	hangout_event = he;
}

//gets the sender id
char * Event::GetSenderId() const {
	return sender_id;
}

// gets the timestamp of the event
long long Event::GetTimestamp() const {
	return timestamp;
}

// checks if the event contains the give type content
bool Event::IsEventType(Event::Type type) const {
	switch (type)
	{

	case Event::TEXT:
		return texts.size();
		break;

	case Event::PHOTO:
		for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
			if (it->GetType() == Attachment::Type::PHOTO) {
				return true;
			}
		}
		return false;
		break;

	case Event::VIDEO:
		for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
			if (it->GetType() == Attachment::Type::VIDEO) {
				return true;
			}
		}
		return false;
		break;

	case Event::CALL:
		return is_hangout_event;
		break;

	case Event::LOCATION:
		for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
			if (it->GetType() == Attachment::Type::LOCATION) {
				return true;
			}
		}
		return false;
		break;

	case Event::URL:
		for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
			if (it->GetType() == Attachment::Type::LINK) {
				return true;
			}
		}
		return false;
		break;

	default:
		return false;
		break;
	}
}

// gets the k'th attachment of a specific type
Attachment Event::GetTypeAttachment(Event::Type t, int k) {
	Attachment::Type attachment_type;
	switch (t)
	{
	case Event::PHOTO:
		attachment_type = Attachment::PHOTO;
		break;
	case Event::VIDEO:
		attachment_type = Attachment::VIDEO;
		break;
	case Event::LOCATION:
		attachment_type = Attachment::LOCATION;
		break;
	case Event::URL:
		attachment_type = Attachment::LINK;
		break;
	default:
		break;
	}
	vector<Attachment> type_attachments;
	for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
		if (it->GetType() == attachment_type) {
			type_attachments.push_back(*it);
		}
	}
	return type_attachments[k];
}

// gets the text
vector<char*> Event::GetTexts() const {
	return texts;
}