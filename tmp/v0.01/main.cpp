#include <iostream>
#include <print>
#include <fstream>
#include <vector>
#include "globals.hpp"

#include "classes.hpp"
#include "handle_input.hpp"


// global object storage
std::vector<GameObj*> world_db;

// instantiate objects
Player* load_world() {
    
    Room* bridge = new Room();
    bridge->dbref = 0;
    bridge->name = "The Bridge";
    bridge->desc = "You find yourself on the bridge of a Romulan destroyer. There are several consoles scattered around the room and a large viewscreen in the center.";


    Player* player = new Player();
    player->dbref = 1;
    player->name = "Gin'tak";
    player->desc = "An old Klingon fool.";
    player->race = "Klingon";
    player->location = 0;

    world_db.push_back(bridge);
    world_db.push_back(player);

    return player;
}

int main() {


    Player* player = load_world();
    // print some STar Trek ASCII art
    std::ifstream file("welcome.txt");
    if (file) {
        std::string line;
            while (std::getline(file, line)) {
                std::println("{}", line);
            }
    }
    else
        std::println("File not found!");
    look(get_player(world_db),world_db);

    while (true) {
        handle_input();
    }

    return 0;
}
