#pragma once

#include "globals.hpp"
#include <string>
#include <vector>
#include "classes.hpp"

void handle_input();


inline Player* get_player(const std::vector<GameObj*>& world_db) {
    GameObj* obj = get_obj_by_dbref(1, world_db);
    return dynamic_cast<Player*>(obj);
}

void look(Player* player, const std::vector<GameObj*>& world_db);
