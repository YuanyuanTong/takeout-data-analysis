#pragma once

#include "participants.h"

// **************** Participant ********************************

// constructor
Participant::Participant() {
	gaia_id = new char[4];
	strcpy(gaia_id, "N.A.");
	participant_name = new char[4];
	strcpy(participant_name, "N.A.");
}

// constructs with parameters
Participant::Participant(const char* id, const char* name) {
	gaia_id = new char[strlen(id)];
	strcpy(gaia_id, id);
	participant_name = new char[strlen(name)];
	strcpy(participant_name, name);
}

// copy constructor
Participant::Participant(const Participant &par) {
	gaia_id = par.gaia_id;
	participant_name = par.participant_name;
}

// gets the name of the participant
char * Participant::GetName() const {
	return participant_name;
}

// gets the gaia id of the participant
char * Participant::GetId() const {
	return gaia_id;
}

