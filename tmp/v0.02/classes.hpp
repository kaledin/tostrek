#pragma once

#include <string>
#include <vector>


enum class ObjType { Room, Exit, Player, Thing };

// GameObj - this is the master one, should contain a dbref, name, desc, contents (?)
class GameObj {
	public:
	ObjType type;
	int dbref;
	std::string name;
	std::string desc;

	virtual ~GameObj() = default;
};

// Room - description, exits
class Room : public GameObj {
	public:
	std::vector<int> exits;
	
	Room() {
		type = ObjType::Room;
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
		type = ObjType::Exit;
	}
};

// Player - description, contents
class Player : public GameObj {
	public:
	int health;
	std::string race;
	int location;
	Player() {
		type = ObjType::Player;
	}
};

// Thing - how is this any different from master GameObj? Do we need this class?
class Thing : public GameObj {
	public:
	int location;
	Thing() {
		type = ObjType::Thing;
	}
};


// SpaceObj - do we need a basic master class for SpaceObj, or we can go directly into star/planet/base/ship/anomaly ?
//


// Structs like CoordsAbs / CoordsFmt should also go here
//
