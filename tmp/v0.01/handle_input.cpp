#include <iostream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"

void look(Player* player, const std::vector<GameObj*>& world_db) {
    // basic 'look' functionality
    int loc = player->location;
    Room* current_room = nullptr;
    for (auto* obj: world_db) {
        if (obj->dbref == loc) {
            current_room = dynamic_cast<Room*>(obj);
            break;
        }
    }
    
    std::println("{}", current_room->name);
    std::println("{}", current_room->desc);
}

void handle_input() {
    std::string user_input;
    std::print("> ");
    std::getline(std::cin, user_input);

    if (user_input == "quit") {
        for (GameObj* obj: world_db) {
             delete obj;
        }
        std::println("Quitting...");
        exit(0);
    }
    else if (user_input == "help")
        std::println("There is no help yet, you're on your own.");
    else if (user_input == "look")
        look(get_player(world_db), world_db);
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}
