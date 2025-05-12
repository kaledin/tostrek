#include <print>
#include <fstream>
#include <unordered_map>
#include "globals.hpp"
#include "classes.hpp"
#include "handle_input.hpp"


// global object storage
std::unordered_map<int, GameObj*> world_db;
std::unordered_map<int, Room*> rooms_db;
std::unordered_map<int, Player*> players_db;
std::unordered_map<int, Exit*> exits_db;
std::unordered_map<int, Thing*> things_db;


// instantiate objects
Player* load_world() {
    
    Room* room = new Room();
    room->dbref = 0;
    room->name = "The Bridge";
    room->desc = "You find yourself on the bridge of a Romulan destroyer. There are several consoles scattered around the room and a large viewscreen in the center.";
    world_db.emplace(room->dbref, room);
    rooms_db.emplace(room->dbref, room);

    Player* player = new Player();
    player->dbref = 1;
    player->name = "Gin'tak";
    player->desc = "An old Klingon fool.";
    player->race = "Klingon";
    player->location = 0;
    world_db.emplace(player->dbref, player);
    players_db.emplace(player->dbref, player);

    room = new Room();
    room->dbref = 3;
    room->name = "Boom Corridor";
    room->desc = "You are in the ship's boom corridor. The door forward leades to the bridge, aft leads to engineering.";
    world_db.emplace(room->dbref, room);
    rooms_db.emplace(room->dbref, room);

    Exit* exit = new Exit();
    exit->dbref = 4;
    exit->name = "Aft Corridor";
    exit->alias = "aft";
    exit->location = 0;
    exit->destination = 3;
    world_db.emplace(exit->dbref, exit);
    exits_db.emplace(exit->dbref, exit);

    Thing* thing = new Thing();
    thing->dbref = 5;
    thing->name = "Helm Console";
    thing->desc = "The console is dim and seems to be defunct.";
    thing->location = 0;
    world_db.emplace(thing->dbref, thing);
    things_db.emplace(thing->dbref, thing);

    thing = new Thing();
    thing->dbref = 6;
    thing->name = "Tricorder";
    thing->desc = "A small handheld tricorder which seems to be powered off.";
    thing->location = 1;
    world_db.emplace(thing->dbref, thing);
    things_db.emplace(thing->dbref, thing);

    exit = new Exit();
    exit->dbref = 7;
    exit->name = "Bridge";
    exit->alias = "fore";
    exit->location = 3;
    exit->destination = 0;
    world_db.emplace(exit->dbref, exit);
    exits_db.emplace(exit->dbref, exit);



    return player;
}

void cleanup(std::unordered_map<int, GameObj*>& db) {
    for (auto& [_, obj] : db)
         delete obj;
    db.clear();
    rooms_db.clear();
    players_db.clear();
    exits_db.clear();
    things_db.clear();
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
    look(player);

    while (handle_input()) {
    }
    
    cleanup(world_db);
    
    return 0;
}
