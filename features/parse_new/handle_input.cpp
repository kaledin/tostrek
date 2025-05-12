#include <iostream>
#include <fstream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"

std::unordered_map<std::string, std::function<void(Player*, const std::string&)>> command_table = {
    { "look", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("You look around.");
        else
            std::println("You look at {}.", args);
    }},
    { "say", [](Player* player, const std::string& args) {
        std::println("You say: \"{}\"", args);
    }},
    { "inv", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("You are carrying nothing. TODO!");
        else
            std::println("inv with args needs an error");
    }}
};

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


bool handle_input(Player* player) {
    std::string verb;
    std::string args;

    std::print("> ");
    std::cin >> verb;
    std::getline(std::cin, args);

// Let's do a simple check if user wants to quit on top of everything
    if (verb == "quit" && args.empty()) {
        std::println("Quitting...");
        return false;
    }
    // 0. Check if it's an exit alias
    if (check_exits(player, verb))
        return true;

    // 1. Check if it's a global command:
    if (command_table.contains(verb)) {
        command_table[verb](player, args);
        return true;
    }

    /* 2. Try object-specific commands on accessible Things
    for (auto& [_, thing] : things_db) {
        if (thing->location == player->location || thing->location == player->dbref) {
            if (thing->object_commands.contains(verb)) {
                thing->object_commands[verb](player, thing, args);
                return true;
            }
        }
    } */

    std::println("Unknown command.");
    return true;
}
