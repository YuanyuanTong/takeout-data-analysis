#include "conversations.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/encodedstream.h"

using namespace rapidjson;

// ************** Conversation ***********************

// default constructor
Conversation::Conversation()
	:invite_timestamp(0ll), latest_read_timestamp(0ll) {
	self_id = new char[4];
	strcpy(self_id, "N.A.");
	inviter_id = new char[4];
	strcpy(inviter_id, "N.A.");
	self_name = new char[4];
	strcpy(self_name, "N.A.");
	group_chat = false;
}

// constructs a conversation from parameters
Conversation::Conversation(const char* ii, long long it, long long lt, bool group, const char* si)
	:invite_timestamp(it), latest_read_timestamp(lt) {

	inviter_id = new char[strlen(ii)];
	strcpy(inviter_id, ii);

	self_id = new char[strlen(si)];
	strcpy(self_id, si);

	group_chat = group;
}

// copy constructor
Conversation::Conversation(const Conversation& con) {
	participants = con.participants;
	events = con.events;
	self_id = con.self_id;
	inviter_id = con.inviter_id;
	self_name = con.self_name;
	group_chat = con.group_chat;
	invite_timestamp = con.invite_timestamp;
	latest_read_timestamp = con.latest_read_timestamp;
}

// adds a new participant to the participant list
void Conversation::AddParticipant(const char* participant_id, const char* participant_name) {
	// already one participant (other than self)
	if (participants.size() == 1) {
		group_chat = true;
	}
	Participant new_participant(participant_id, participant_name);
	participants.push_back(new_participant);
}

// adds a new event to the events list
void Conversation::AddEvent(Event new_event) {
	events.push_back(new_event);
}

// edits the self name
void Conversation::SelfName(const char* sn) {
	self_name = new char[strlen(sn)];
	strcpy(self_name, sn);
}

// counts the events in a conversation
int Conversation::EventCount() const {
	return events.size();
}

// sets the events
void Conversation::SetEvents(vector<Event> new_events) {
	events = new_events;
}

// prints all the participants
void Conversation::PrintParticipants() const {
	cout << "< ";
	for (vector<Participant>::const_iterator it = participants.begin(); it != participants.end(); ++it) {
		cout << it->GetName() << " ";
	}
	cout << ">";
}

// prints all the events
void Conversation::PrintEvents() const {
	for (vector<Event>::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (strcmp(it->GetSenderId(), self_id)) {
			for (vector<Participant>::const_iterator pit = participants.begin(); pit != participants.end(); ++pit) {
				if (strcmp(pit->GetId(), it->GetSenderId()) == 0) {
					cout << "<" << pit->GetName() << "> ";
					break;
				}
			}
		}
		else {
			cout << "<" << self_name << "> ";
		}
		cout << ConvertTime(it->GetTimestamp()) << endl;

		vector<char*> texts = it->GetTexts();

		for (vector<char*>::const_iterator tit = texts.begin(); tit != texts.end(); ++tit) {
			cout << *tit;
		}
		cout << endl;
	}
}

// checks if the conversation was started by self
bool Conversation::SelfStarted() const {
	return strcmp(self_id, inviter_id) == 0;
}

// gets the invite timestamp of a conversation
long long Conversation::GetInviteTimestamp() const {
	return invite_timestamp;
}

// gets the latest read timestamp;
long long Conversation::GetLatestReadTimestamp() const {
	return latest_read_timestamp;
}

// gets the inviter id;
char* Conversation::GetInviterId() const {
	return inviter_id;
}

// checks if the conversation is group chat;
bool Conversation::IsGroupChat() const {
	return group_chat;
}

// gets all the events
vector<Event> Conversation::GetEvents() const {
	return events;
}

// gets all the participants
vector<Participant> Conversation::GetParticipants() const {
	return participants;
}

// gets events on the same date with the given Time range
vector<Event> Conversation::GetEvents(Time t, int range) const {
	vector<Event> time_events;
	for (vector<Event>::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (range) {
			long long cur_timestamp = Convert2Timestamp(t);
			if ( abs(cur_timestamp - it->GetTimestamp()) < long long(range * 24 * 60 * 60 * 1000000) ) {
				time_events.push_back(*it);
			}
		}
		else {
			if (t.SameDate(it->GetTimestamp())) {
				time_events.push_back(*it);
			}
		}		
	}
	return time_events;
}

// gets a specific type of events
vector<Event> Conversation::GetEvents(Event::Type t) const {
	vector<Event> type_events;
	for (vector<Event>::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (it->IsEventType(t)) {
			type_events.push_back(*it);
		}
	}
	return type_events;
}


// **************** ConversationList ********************************

// gets the total number of conversations
int ConversationList::Size() const {
	return conversations.size();
}

// adds a new conversation to the list
void ConversationList::AddConversation(const Conversation & con) {
	conversations.push_back(con);
}

// gets the most frequently contacted person, returns the frequency
int ConversationList::GetFrequentContact(Conversation &con) const {
	int max_frequency = 0;
	int index = 0;
	int max_index = index;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		if (it->EventCount() > max_frequency) {
			max_frequency = it->EventCount();
			max_index = index;
		}
		++index;
	}

	con = conversations[max_index];
	return max_frequency;
}

// gets the earliest started conversation
Time ConversationList::GetFirstContact(Conversation &con) const {
	long long first_timestamp = std::numeric_limits<long long>::max();
	int index = 0;
	int first_index = index;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		if (it->GetInviteTimestamp() < first_timestamp) {
			first_timestamp = it->GetInviteTimestamp();
			first_index = index;
		}
		++index;
	}

	con = conversations[first_index];
	return ConvertTime(first_timestamp);
}

// gets the most chatting day (Time) in terms of messages exchanged in a single conversation
Time ConversationList::GetMostChattingDay(Conversation &con) const {
	int day_events_cnt = 0;
	int max_cnt = 0;
	int index = 0;
	int max_idx = index;
	long long tmp_timestamp = 0;
	long long max_timestamp = tmp_timestamp;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		vector<Event> events = it->GetEvents();
		for (vector<Event>::const_iterator eit = events.begin(); eit != events.end(); ++eit) {
			if (SameDate(tmp_timestamp, eit->GetTimestamp())) {
				++day_events_cnt;
				if (day_events_cnt > max_cnt) {
					max_cnt = day_events_cnt;
					max_timestamp = tmp_timestamp;
					max_idx = index;
				}
			}
			else {
				tmp_timestamp = eit->GetTimestamp();
				day_events_cnt = 1;
			}
		}
		++index;
	}

	con = conversations[max_idx];
	return ConvertTime(max_timestamp);
}

// gets a random event of a specific type
bool ConversationList::GetType(Event::Type t, Conversation &con, Event &ev) const {
	vector<Conversation> type_conversations;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		if (it->GetEvents(t).size()) {
			type_conversations.push_back(*it);
		}
	}
	if (type_conversations.size()) {
		srand(time(NULL));
		int index = rand() % type_conversations.size();
		con = type_conversations[index];
		vector<Event> events = con.GetEvents(t);
		index = rand() % events.size();
		ev = events[index];
		return true;
	}
	else {
		return false;
	}
}

// gets a subset of the conversations happened during a day
ConversationList ConversationList::GetPeriodConversations(const char* time_string, int range) {
	ConversationList new_list;
	long long timestamp = Convert2Timestamp(time_string, true);
	Time time = ConvertTime(timestamp);
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		Conversation new_conversation(*it);
		if (it->GetEvents(time, range).size()) {
			new_conversation.SetEvents(it->GetEvents(time, range));
			new_list.AddConversation(new_conversation);
		}		
	}
	
	return new_list;
}

// prints information
void ConversationList::PrintInformation() const {
	if (Size()) {
		cout << "you talked with ";
		for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
			it->PrintParticipants();
			cout << endl;
			it->PrintEvents();
		}
	}
}

// reads a Hangouts.json file and return a conversation list
ConversationList ReadHangoutsJson(const char * file_name) {

	// a list to store all conversations in the json file
	ConversationList conversation_list;

	// reading json file
	FILE* fp1 = fopen(file_name, "rb");
	char readBuffer[256];
	FileReadStream bis(fp1, readBuffer, sizeof(readBuffer));
	AutoUTFInputStream<unsigned, FileReadStream> eis(bis);
	Document d;         // Document default uses GenericDocument<UTF8<> >
	d.ParseStream<0, AutoUTF<unsigned> >(eis).HasParseError();
	fclose(fp1);


	// Hangouts.json is stored as an object with a single array "conversations".

	SizeType n = d["conversations"].Size();
	const Value & a = d["conversations"];
	int what[3];

	// insert all conversation into the "conversations" list
	for (SizeType i = 0; i < n; ++i) {
		// basic information of a conversation
		const Value & conversation_state = a[i]["conversation"]["conversation"]["self_conversation_state"];
		const char *self_id = conversation_state["self_read_state"]["participant_id"]["gaia_id"].GetString();
		long long latest_read_timestamp = atoll(conversation_state["self_read_state"]["latest_read_timestamp"].GetString());
		const char * inviter_id = conversation_state["inviter_id"]["gaia_id"].GetString();
		long long invite_timestamp = atoll(conversation_state["invite_timestamp"].GetString());

		Conversation  cur_conversation(inviter_id, invite_timestamp, latest_read_timestamp, false, self_id);

		const Value & participant_data = a[i]["conversation"]["conversation"]["participant_data"];
		for (Value::ConstValueIterator itr = participant_data.Begin(); itr != participant_data.End(); ++itr) {
			const char * participant_id = itr->GetObject()["id"]["gaia_id"].GetString();

			// participants besides self
			if (strcmp(participant_id, self_id)) {
				const char * participant_name = itr->GetObject()["fallback_name"].GetString();
				cur_conversation.AddParticipant(participant_id, participant_name);
			}
			// self
			else {
				const char * self_name = itr->GetObject()["fallback_name"].GetString();
				cur_conversation.SelfName(self_name);
			}
		}

		// all detailed events (messages) of a conversation
		const Value & events = a[i]["events"];
		for (Value::ConstValueIterator itr = events.Begin(); itr != events.End(); ++itr) {
			const char* sender_id = itr->GetObject()["sender_id"]["gaia_id"].GetString();
			long long send_timestamp = atoll(itr->GetObject()["timestamp"].GetString());
			Event new_event(sender_id, send_timestamp);

			// hangout call event
			if (itr->GetObject().HasMember("hangout_event")) {
				HangoutEvent::EventType event_type;
				HangoutEvent::MediaType media_type;
				if (strcmp(itr->GetObject()["hangout_event"]["event_type"].GetString(), "START_HANGOUT") == 0) {
					event_type = HangoutEvent::EventType::START_HANGOUT;
				}
				if (strcmp(itr->GetObject()["hangout_event"]["event_type"].GetString(), "END_HANGOUT") == 0) {
					event_type = HangoutEvent::EventType::END_HANGOUT;
				}
				if (strcmp(itr->GetObject()["hangout_event"]["media_type"].GetString(), "AUDIO_ONLY") == 0) {
					media_type = HangoutEvent::MediaType::AUDIO_ONLY;
				}

				HangoutEvent * new_hangout_event;
				if (itr->GetObject()["hangout_event"].HasMember("hangout_duration_secs")) {
					int duration_secs = atoi(itr->GetObject()["hangout_event"]["hangout_duration_secs"].GetString());
					new_hangout_event = new HangoutEvent(event_type, media_type, duration_secs);
					new_event.SetHangoutEvent(new_hangout_event);
				}
				else {
					new_hangout_event = new HangoutEvent(event_type, media_type);
					new_event.SetHangoutEvent(new_hangout_event);
				}

				// adding participants of the hangout call
				const Value & hangout_participants = itr->GetObject()["hangout_event"]["participant_id"];
				for (Value::ConstValueIterator pitr = hangout_participants.Begin(); pitr != hangout_participants.End(); ++pitr) {
					new_hangout_event->AddParticipant(pitr->GetObject()["gaia_id"].GetString());
				}

				new_event.SetHangoutEvent(new_hangout_event);
			}

			// regular chat message
			if (itr->GetObject().HasMember("chat_message")) {
				const Value & message_content = itr->GetObject()["chat_message"]["message_content"];

				// basic text messages (including urls)
				if (message_content.HasMember("segment")) {

					const Value & segments = message_content["segment"];

					for (Value::ConstValueIterator sitr = segments.Begin(); sitr != segments.End(); ++sitr) {

						if (strcmp(sitr->GetObject()["type"].GetString(), "LINK")) {
							if (strcmp(sitr->GetObject()["type"].GetString(), "LINE_BREAK")) {
								new_event.AddText(sitr->GetObject()["text"].GetString());
							}
							else {
								new_event.AddText("\n");
							}
						}

						// texts including urls (seen as attachments)
						else {
							new_event.AddText(sitr->GetObject()["text"].GetString());
							new_event.AddAttachment(sitr->GetObject()["text"].GetString(), Attachment::Type::LINK);
						}
					}
				}

				// messages with photo, video or location attachments
				if (message_content.HasMember("attachment")) {
					// all the attachments
					const Value & attachments = message_content["attachment"];

					for (Value::ConstValueIterator aitr = attachments.Begin(); aitr != attachments.End(); ++aitr) {
						// 
						if (strcmp(aitr->GetObject()["embed_item"]["type"][0].GetString(), "PLUS_PHOTO")) {
							// location attachment
							if (!strcmp(aitr->GetObject()["embed_item"]["type"][0].GetString(), "PLACE_V2")) {
								const Value & place = aitr->GetObject()["embed_item"]["place_v2"];
								new_event.AddAttachment(place["url"].GetString(), Attachment::Type::LOCATION, place["name"].GetString());
							}
						}

						// photo or video attachment
						else {
							const Value & plus_photo = aitr->GetObject()["embed_item"]["plus_photo"];
							const char * url = plus_photo["thumbnail"]["url"].GetString();
							Attachment::Type attachment_type;
							// photo
							if (!strcmp(plus_photo["media_type"].GetString(), "PHOTO")) {
								attachment_type = Attachment::Type::PHOTO;
							}

							if (!strcmp(plus_photo["media_type"].GetString(), "VIDEO")) {
								attachment_type = Attachment::Type::VIDEO;
							}
							new_event.AddAttachment(url, attachment_type);

						}
					}
				}
			}

			// adding the current event to the conversation
			cur_conversation.AddEvent(new_event);
		}

		// adding the current conversation to the conversation list
		conversation_list.AddConversation(cur_conversation);
	}

	return conversation_list;
}