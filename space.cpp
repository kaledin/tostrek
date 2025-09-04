#include <thread>
#include <chrono>
#include "globals.hpp"
#include <cmath>
#include <print>
#include <numbers>
#include "space.hpp"
#include "handle_input.hpp"
#include <fstream>
#include <cctype>
#include <ncurses.h>
#include <algorithm>


const double c = 0.299792458;
const double sectorsize = 20000.0;

// std::atomic<bool> running{true};

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
    GameObj* helm = world_db[5]; // this is a dirty hack, 
 
    helm->object_commands["helm"] = [](GameObj* player, GameObj* self, const std::string& args) {
	
	if (args.empty())
	    std::println(R"(Huh?  (Type "?" or "help" for help.))");

	else if (args == "console") {
	    if (self->mannedby == player->dbref) {
		self->mannedby = std::numeric_limits<size_t>::max();
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
  
    helm->object_commands["nav"] = [](GameObj* player, GameObj* self, const std::string& args) {
	if (args.empty())
            std::println(R"(Huh?  (Type "?" or "help" for help.))");
	else if (self->mannedby != player->dbref)
	    std::println("You are not currently manning the helm console.");
	else
	    navconsole(player, self, args);
    };
}

void navconsole(GameObj* player, GameObj* self, const std::string& args) {
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
	    world_db[self->shipref]->curspeed = 0;
    }
    else if (args == "passive") {
	    std::println("---------------------- {} -- Long Range Scan -----------------------",
				    world_db[self->shipref]->name);
	    for (const auto& [key, value] : world_db[self->shipref]->lrs_contacts) {
          if(world_db[value]->type != SHIP)
            continue;
	        std::string lrscolor{};
	        std::string name{};
	        if (world_db[value]->sp_type == "Ship")
		        lrscolor = CYAN;
	        else if (world_db[value]->sp_type == "Planet")
		        lrscolor = GREEN;
	        if (dist3d(world_db[self->shipref]->coords, world_db[value]->coords)<sectorsize)
		        name = world_db[value]->name;
	        else
		        name = "- - -";
	        std::println("{}{} :  {:<10}{:3.0f}/{:<3.0f}    {:<14}{:<14}{:<}{}", lrscolor, key, 
		        str_toupper(world_db[value]->sp_type),
		        calc_pitch_yaw(world_db[self->shipref]->coords, world_db[value]->coords)[0],
		        calc_pitch_yaw(world_db[self->shipref]->coords, world_db[value]->coords)[1],
		        showdist(dist3d(world_db[self->shipref]->coords, world_db[value]->coords)), 
		        showspeed(world_db[value]->curspeed), name, CLR_RESET);
	    }
	    std::println("------------------------------------------------------------------------------");
	}

    else if (args == "window") {
        initscr();             // Start ncurses
        cbreak();              // Disable line buffering
        noecho();              // Don't echo user input
        nodelay(stdscr, TRUE); // Make getch() non-blocking
        curs_set(0);           // Hide cursor

        while (true) {
            clear();
            mvprintw(0, 0, "---------------------- %s -- Long Range Scan -----------------------",
                     world_db[self->shipref]->name.c_str());

            int row = 1;
            for (const auto& [key, value] : world_db[self->shipref]->lrs_contacts) {
                auto contact = world_db[value];
                std::string name = "- - -";
                if (dist3d(world_db[self->shipref]->coords, contact->coords) < sectorsize)
                    name = contact->name;

                auto [pitch, yaw] = calc_pitch_yaw(world_db[self->shipref]->coords, contact->coords);

                mvprintw(row++, 0, "%c : %-10s %3.0f/%-3.0f  %-14s %-14s %-s",
                         key,
                         str_toupper(contact->sp_type).c_str(),
                         pitch, yaw,
                         showdist(dist3d(world_db[self->shipref]->coords, contact->coords)).c_str(),
                         showspeed(contact->curspeed).c_str(),
                         name.c_str());
            }

            mvprintw(row + 1, 0, "Press 'q' to quit.");
            refresh();

            int ch = getch();
            if (ch == 'q' || ch == 'Q')
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1 tick delay
        }

        endwin(); // Exit ncurses mode
    }

    else if (auto pos = args.find(' '); pos != std::string::npos) {
        auto arg1 = args.substr(0, pos);
        auto arg2 = args.substr(pos + 1);
        if (arg1 == "imp") {
	        if (auto result = strtodbl(arg2)) {
		        if (*result > -0.33 && *result < 1.0) {
		        std::println("Acknowledged: accelerating to IMPULSE {}", *result);
		        world_db[self->shipref]->curspeed = c * (*result);
		        }
		        else
		            std::println("Error: cannot accelerate to IMPULSE {}", *result);
	        }
	    }
        else if (arg1 == "warp") {
	        if (auto result = strtodbl(arg2)) {
		        if (*result > 0 && *result < world_db[self->shipref]->maxwarp) {
		            std::println("Acknowledged: accelerating to WARP {}.", *result);
		            world_db[self->shipref]->curspeed = warp2gms(*result);
		        }
		        else
		            std::println("Error: cannot accelerate to WARP {}.", *result);
	        }
	    }
        else if (arg1 == "heading") {
            if (auto pos1 = arg2.find('/'); pos1 != std::string::npos) {
	            auto arg3 = arg2.substr(0,pos1);
	            auto arg4 = arg2.substr(pos1 + 1);
	            auto yaw = strtodbl(arg3);
	            auto pitch = strtodbl(arg4);
	        
                if (yaw && pitch && *yaw >= 0.0 && *yaw <= 360.0 && *pitch >= -90.0 && *pitch <= 90.0) {
		            std::println("Acknowledged: heading set to {} mark {}.", *yaw, *pitch);
		            world_db[self->shipref]->heading[0] = *yaw;
		            world_db[self->shipref]->heading[1] = *pitch;
	            }
                else
		            std::println("Error: incorrect heading/mark.");
	        }
            else
	            std::println("Error: <heading>/<mark> expected.");
        }
        else
	        std::println("Error: unknown command.");
    }
    else
        std::println("Error. Unknown command");
}

double dist3d(std::array<double, 3> obj1, std::array<double, 3> obj2) {
    return sqrt(pow(obj2[0] - obj1[0], 2) + pow(obj2[1] - obj1[1], 2) + pow(obj2[2] - obj1[2], 2) * 1.0);
}

std::string showspeed(double speed) {
    std::string str{};
    double c_speed = speed / c;
    if (c_speed == 0)
	return str = "FULL STOP";
    else if (c_speed > 1)
	return str = "Warp " + std::format("{:.2f}", std::pow(c_speed, 1.0 / 3.0));
    else
	return str = std::format("{:.2f}", c_speed) + "c";
}

std::string showdist(double dist) {
    std::string str{};
    if (dist >= sectorsize)
	return str = std::format("{:.2f}", dist/sectorsize) + " Sctrs";
    else if (dist < 0.1)
        return str = std::format("{:.1f}", std::round(dist*1000000*10.0)/10.0) + " Kms";
    else
	return str = std::format("{:.1f}", std::round(dist * 10.0)/10.0) + " Gms";
}

std::array<double, 2> calc_pitch_yaw(std::array<double, 3> pos1, std::array<double, 3> pos2) {
    auto dx = pos2[0]-pos1[0];
    auto dy = pos2[1]-pos1[1];
    auto dz = pos2[2]-pos1[2];
    double yaw = std::fmod(rad2deg(std::atan2(dy, dx)) + 360.0, 360.0);

    double pitch = rad2deg(std::atan2(dz, std::hypot(dx, dy)));

    return {yaw, pitch};
}

void update_lrs(GameObj* obj) {
    std::vector<size_t> found{};
    std::vector<char> toerase{};

    for (auto& contact : world_db) {
      if (contact->type != SHIP)
        continue;
      
	    if (contact->dbref == obj->dbref)
	      continue;

	    if (dist3d(obj->coords, contact->coords) < 100000)
	      found.push_back(contact->dbref);
    }

    for (const auto& pair : obj->lrs_contacts) {
        if (!std::ranges::contains(found, pair.second))
            toerase.push_back(pair.first);
        else
            std::erase(found, pair.second);
    }

    for (const auto& it : toerase) {
        obj->lrs_contacts.erase(it);
    }

    for (const auto& cts : found) {
        for (char key = 'A'; key <= 'Z'; ++key) {
            if (!obj->lrs_contacts.contains(key)) {
                obj->lrs_contacts.emplace(key,cts);
                break;
            }
        }
    }
}

void tick_loop() {
    while (running) {
	tick_all_spaceobjs(); // your function`  
	std::this_thread::sleep_for(std::chrono::seconds(1));  
	}  
}

void tick_all_spaceobjs() {  
	for (auto& obj : world_db) { 
    if (obj->type != SHIP)
      continue;
		double delta_time = 1.0;
		obj->coords[0] += obj->curspeed * std::cos(deg2rad(obj->heading[1])) * std::cos(deg2rad(obj->heading[0])) * delta_time;
		obj->coords[1] += obj->curspeed * std::cos(deg2rad(obj->heading[1])) * std::sin(deg2rad(obj->heading[0])) * delta_time;
		obj->coords[2] += obj->curspeed * std::sin(deg2rad(obj->heading[1])) * delta_time;
	  update_lrs(obj);
	}  
}

