#pragma once
#include <string>
#include "classes.hpp"

constexpr double deg2rad(double deg);
constexpr double rad2deg(double rad);

std::array<double, 2> calc_pitch_yaw(std::array<double, 3> pos1, std::array<double, 3> pos2);
void navconsole(Player* player, Thing* self, const std::string& args);
void console_init();
void tick_loop();
std::string showspeed(double speed);
std::string showdist(double dist); 
double dist3d(std::array<double, 3> obj1, std::array<double, 3> obj2);
void tick_all_spaceobjs();  
