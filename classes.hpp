#pragma once

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

// GameObj - this is the master one, should contain a dbref, name, desc, contents (?)
class GameObj {
	public:
	int dbref;
	int location;
	std::string type;
	std::string name;
	std::string desc;

	virtual ~GameObj() = default;
};

// Room - description, exits
class Room : public GameObj {
	public:
	std::vector<int> exits;

	Room() {
		type = "Room";
	}
};

// Exit - linking logic: 
// each exit has a source (room dbref) and a destination (room dbref)
class Exit : public GameObj {
	public:
	int destination;
	std::string alias;

	Exit() {
	    type = "Exit";
	}
};

// Player - description, contents
class Player : public GameObj {
	public:
	int health;
	std::string race;

	Player() {
	    type = "Player";
	}
};

// Thing - how is this any different from master GameObj? Do we need this class?
class Thing : public GameObj {
	public:
	bool lock;

	Thing() {
	    type = "Thing";
	    lock = false;
	}
};

 // namespace nlohmann
// SpaceObj - do we need a basic master class for SpaceObj, or we can go directly into star/planet/base/ship/anomaly ?
//


// Structs like CoordsAbs / CoordsFmt should also go here
//
