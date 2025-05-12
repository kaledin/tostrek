#pragma once

#include "classes.hpp"
#include <vector>

GameObj* get_obj_by_dbref(int dbref, const std::vector<GameObj*>& world_db);
extern std::vector<GameObj*> world_db;
