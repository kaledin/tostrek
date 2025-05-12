#include <iostream>
#include <fstream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"

void look(Player* player) {
    // basic 'look' functionality
    int loc = player->location;

    auto it = rooms_db.find(loc);
    if (it == rooms_db.end())
        return;

    Room* current_room = it->second;
    if (!current_room)
        return;
   
    std::println("{}{}{}", GREEN, current_room->name, CLR_RESET);
    std::println("{}", current_room->desc);
    std::println("Contents:");
 /*   for (int dbref : current_room->contents) {
        if (auto it = world_db.find(dbref); it != world_db.end())
            std::println("{}", it->second->name);
    } */
    for (const auto& [dbref, player1] : players_db) {
        if (player1->location == loc) {
            std::println("{}{}{}", RED, player1->name, CLR_RESET);
        }
    }
    for (const auto& [dbref, thing] : things_db) {
        if (thing->location == loc) {
            std::println("{}{}{}", BLUE, thing->name, CLR_RESET);
        }
    }


    std::println("Obvious exits:");
    for (const auto& [dbref, exit] : exits_db) {
        if (exit->location == loc) {
            std::println("- {} <{}{}{}>", exit->name, CYAN, exit->alias, CLR_RESET);
        }
    }
}

void inv(Player* player) {
    int loc = player->dbref;

    std::println("You are carrying:");
    for (const auto& [dbref, thing] : things_db) {
        if (thing->location == loc) {
            std::println("{}", thing->name);
        }
    }
}

bool check_exits(Player* player, std::string& input) {
    int loc = player->location;
    auto it = rooms_db.find(loc);
    if (it == rooms_db.end())
        return false;

    Room* current_room = it->second;
    if (!current_room)
        return false;
    
    for (const auto& [dbref, exit] : exits_db) {
        if (exit->location == loc && exit->alias == input) {
            std::println("You walk towards the exit leading {}.", exit->alias);
            player->location = exit->destination;
            look(player);
            return true;
        }
    }
    return false;
}   

bool handle_input() {
    std::string user_input;
    std::print("> ");
    std::getline(std::cin, user_input);

    if (check_exits(players_db.at(1), user_input))
        return true;

    else if (user_input == "quit") {
        std::println("Quitting...");
        return false;
    }
    else if (user_input == "help") {
        std::ifstream file("help.txt");
        if (file) {
            std::string line;
                while (std::getline(file, line)) {
                    std::println("{}", line);
                }
        }
        else
            std::println("File not found!");
    }
    else if (user_input == "look")
        look(players_db.at(1));
    else if (user_input == "inv")
        inv(players_db.at(1));
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    return true;
}
