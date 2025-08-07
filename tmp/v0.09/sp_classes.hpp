#pragma once

#include <string>
#include <array>
#include "classes.hpp"
#include <map>

/* struct CoordsAbs {
    double x;
    double y;
    double z;
}; */

struct CoordsFmt {
    std::string quadrant;
    int sx;
    int sy;
    int sz;
    double x;
    double y;
    double z;
};

class SpaceObj : public Thing {
	public:

	double maxwarp;
	double curspeed;
	std::array<double, 3> coords;
	std::array <double, 2> heading;
};

class Ship : public SpaceObj {
	public:

	std::string sp_class;
	std::string sp_empire;
	std::map<char, int> lrs_contacts;
//	bool canland;
//	bool isdocked;
//	bool shields;
//	double lrs_range;
//	double maxhull;
//	double curhull;
//	Ship() {
//	    sp_type = "Ship";
//	}
};

