#pragma once
#include <string>
#include "classes.hpp"

constexpr double deg2rad(double deg);
constexpr double rad2deg(double rad);

void navconsole(Player* player, Thing* self, const std::string& args);
void console_init();
void tick_loop();
void tick_all_spaceobjs();  
