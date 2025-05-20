#include <thread>
#include <chrono>
#include "globals.hpp"
#include <cmath>
#include <print>
#include <numbers>
#include "space.hpp"
#include "handle_input.hpp"
#include <fstream>

const double c = 0.299792458;
const int sectorsize = 20000;

std::atomic<bool> running{true};

constexpr double warp2gms(double warp) {
    return warp*warp*warp*c;
}

constexpr double deg2rad(double deg) {
    return deg * (std::numbers::pi / 180.0);
}

constexpr double rad2deg(double rad) {
    return rad * (180.0 / std::numbers::pi);
}

void console_init() {
    Thing* helm = things_db.at(5);
 
    helm->object_commands["helm"] = [](Player* player, Thing* self, const std::string& args) {
	
	if (args.empty())
	    std::println(R"(Huh?  (Type "?" or "help" for help.))");

	else if (args == "console") {
	    if (self->mannedby == player->dbref) {
		self->mannedby = -1;
		std::println("You stand up from the helm console.");
	    }
	    else {
		self->mannedby = player->dbref;
		std::println("You sit down at the helm console.");
	    }
	}
	else
	    std::println(R"(Huh?  (Type "?" or "help" for help.))");
    };
  
    helm->object_commands["nav"] = [](Player* player, Thing* self, const std::string& args) {
	if (args.empty())
            std::println(R"(Huh?  (Type "?" or "help" for help.))");
	else if (self->mannedby != player->dbref)
	    std::println("You are not currently manning the helm console.");
	else
	    navconsole(player, self, args);
    };
}

void navconsole(Player* player, Thing* self, const std::string& args) {
    (void)player;
    if (args == "help") {
	std::ifstream file("navhelp.txt");
	    if (file) {
		std::string line;
		    while (std::getline(file, line)) {
			std::println("{}", line);
		    }
	    }
	    else
		std::println("Help file not found!");
    }
    else if (args == "full stop") {
	std::println("Acknowledged: decelerating to full stop.");
	ships_db[self->shipref]->curspeed = 0;
    }
    else if (auto pos = args.find(' '); pos != std::string::npos) {
        auto arg1 = args.substr(0, pos);
        auto arg2 = args.substr(pos + 1);
	if (arg1 == "imp") {
	    if (auto result = strtodbl(arg2)) {
		if (*result > -0.33 && *result < 1.0) {
		    std::println("Acknowledged: accelerating to IMPULSE {}", *result);
		    ships_db[self->shipref]->curspeed = c * (*result);
		}
		else
		    std::println("Error: cannot accelerate to IMPULSE {}", *result);
	    }
	}
	else if (arg1 == "warp") {
	    if (auto result = strtodbl(arg2)) {
		if (*result > 0 && *result < ships_db[self->shipref]->maxwarp) {
		    std::println("Acknowledged: accelerating to WARP {}.", *result);
		    ships_db[self->shipref]->curspeed = warp2gms(*result);
		}
		else
		    std::println("Error: cannot accelerate to WARP {}.", *result);
	    }
	}
	else if (auto pos1 = arg2.find('/'); pos1 != std::string::npos) {
	    auto arg3 = arg2.substr(0,pos1);
	    auto arg4 = arg2.substr(pos1 + 1);
	    auto yaw = strtodbl(arg3);
	    auto pitch = strtodbl(arg4);
	    if (yaw && pitch && *yaw >= 0.0 && *yaw <= 360.0 && *pitch >= -90.0 && *pitch <= 90.0) {
		std::println("Acknowledged: heading set to {} mark {}.", *yaw, *pitch);
		ships_db[self->shipref]->heading[0] = *yaw;
		ships_db[self->shipref]->heading[1] = *pitch;
	    }
	    else
		std::println("Error: incorrect heading/mark.");
	}
	else
	    std::println("Error: unknown command.");
    }
    else
	std::println("Error: unknown command.");
}

void tick_loop() {
    while (running) {
	tick_all_spaceobjs(); // your function`  
	std::this_thread::sleep_for(std::chrono::seconds(1));  
	}  
}

void tick_all_spaceobjs() {  
	for (auto& [_, obj] : ships_db) { 
		double delta_time = 1.0;
		obj->coords[0] += obj->curspeed * std::cos(deg2rad(obj->heading[1])) * std::cos(deg2rad(obj->heading[0])) * delta_time;
		obj->coords[1] += obj->curspeed * std::cos(deg2rad(obj->heading[1])) * std::sin(deg2rad(obj->heading[0])) * delta_time;
		obj->coords[2] += obj->curspeed * std::sin(deg2rad(obj->heading[1])) * delta_time;
	}  
}

