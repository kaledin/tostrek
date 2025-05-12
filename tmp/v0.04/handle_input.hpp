#pragma once

#include "classes.hpp"

bool handle_input();

void look(Player* player);
bool check_exits(Player* player, std::string& input);
void inv(Player* player);
