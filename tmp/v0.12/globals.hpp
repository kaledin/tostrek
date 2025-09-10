#pragma once

#include "objects.hpp"
#include <vector>
#include <unordered_map>
#include <atomic>

extern std::atomic<bool> running;

// GameObj* get_obj_by_dbref(int dbref, const std::vector<GameObj*>& world_db);
extern std::vector<GameObj*> world_db;

using CommandFn = void(*)(GameObj*, const std::string&);
extern std::unordered_map<std::string, CommandFn> command_table;


#define CLR_RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
