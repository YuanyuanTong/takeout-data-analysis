#pragma once
#include <string.h>

// **************** Participant ********************************
class Participant {
	char* gaia_id;
	char* participant_name;
public:

	// constructor
	Participant();

	// constructs with parameters
	Participant(const char* id, const char* name = "");

	// copy constructor
	Participant(const Participant &par);

	// gets the name of the participant
	char * GetName() const;

	// gets the gaia id of the participant
	char * GetId() const;
};
