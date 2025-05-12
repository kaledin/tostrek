#pragma once

#include "classes.hpp"
#include <unordered_map>

// GameObj* get_obj_by_dbref(int dbref, const std::vector<GameObj*>& world_db);
extern std::unordered_map<int, GameObj*> world_db;
extern std::unordered_map<int, Room*> rooms_db;
extern std::unordered_map<int, Player*> players_db;
extern std::unordered_map<int, Exit*> exits_db;
extern std::unordered_map<int, Thing*> things_db;

#define CLR_RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
