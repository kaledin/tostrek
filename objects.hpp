#pragma once

#include "json.hpp"

using json = nlohmann::json;

struct CoordsFmt {
    std::string quadrant;
    int sx;
    int sy;
    int sz;
    double x;
    double y;
    double z;
};

enum ObjectType {
    ROOM, // 0
    PLAYER, // 1
    THING, // 2
    EXIT, // 3
    SHIP, // 4
};

struct GameObj {
    size_t dbref;
    ObjectType type;
    std::string name;
    std::string desc;
    size_t location;

    // Fields relevant only to players
    int health;
    std::string race;

    // Fields relevant only to things
	bool lock;
	size_t shipref;
	size_t mannedby;
	std::unordered_map<std::string, std::function<void(GameObj*, GameObj*, const std::string&)>> object_commands;

    // Fields relevant only to rooms
    std::vector<int> exits;
    
    // Fields relevant only to exits
    size_t destination;
    std::string alias;

    // Fields relevant only to spaceobjs
    std::string sp_type;
	std::string sp_class;
	std::string sp_empire;
	double maxwarp;
	double curspeed;
	std::array<double, 3> coords;
	std::array <double, 2> heading;
	std::map<char, size_t> lrs_contacts;
	
//	bool canland;
//	bool isdocked;
//	bool shields;
//	double lrs_range;
//	double maxhull;
//	double curhull;

};
