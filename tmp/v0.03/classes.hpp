#pragma once

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

// GameObj - this is the master one, should contain a dbref, name, desc, contents (?)
class GameObj {
	public:
	int dbref;
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
	int location;
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
	int location;

	Player() {
	    type = "Player";
	}
};

// Thing - how is this any different from master GameObj? Do we need this class?
class Thing : public GameObj {
	public:
	int location;

	Thing() {
	    type = "Thing";
	}
};

namespace nlohmann {

inline void to_json(json& j, const Room& r) {
    j = {
        {"type", "Room"},
        {"dbref", r.dbref},
        {"name", r.name},
        {"desc", r.desc},
    };
}

inline void from_json(const json& j, Room& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
}

inline void to_json(json& j, const Player& r) {
    j = {
        {"type", "Player"},
        {"dbref", r.dbref},
        {"name", r.name},
        {"desc", r.desc},
	{"race", r.race},
	{"location", r.location},
    };
}

inline void from_json(const json& j, Player& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
    r.race = j.at("race").get<std::string>();
    r.location = j.at("location").get<int>();
}

inline void to_json(json& j, const Exit& r) {
    j = {
        {"type", "Exit"},
        {"dbref", r.dbref},
        {"name", r.name},
	{"alias", r.alias},
	{"location", r.location},
	{"destination", r.destination},
    };
}

inline void from_json(const json& j, Exit& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.alias = j.at("alias").get<std::string>();
    r.location = j.at("location").get<int>();
    r.destination = j.at("destination").get<int>();
}

inline void to_json(json& j, const Thing& r) {
    j = {
        {"type", "Thing"},
        {"dbref", r.dbref},
        {"name", r.name},
        {"desc", r.desc},
	{"location", r.location},
    };
}

inline void from_json(const json& j, Thing& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
    r.location = j.at("location").get<int>();
}



} // namespace nlohmann
// SpaceObj - do we need a basic master class for SpaceObj, or we can go directly into star/planet/base/ship/anomaly ?
//


// Structs like CoordsAbs / CoordsFmt should also go here
//
