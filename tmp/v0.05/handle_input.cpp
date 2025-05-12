#include <iostream>
#include <fstream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"

std::unordered_map<std::string, std::function<void(Player*, const std::string&)>> command_table = {
    { "look", [](Player* player, const std::string& args) {
        if (args.empty())
            look(player);
        else
            lookat(player, args);
    }},
    { "say", [](Player* player, const std::string& args) {
        (void)player;
        std::println("You say: \"{}\"", args);
    }},
    { "inv", [](Player* player, const std::string& args) {
        if (args.empty())
            inv(player);
        else
            std::println(R"(Huh?  (Type "?" or "help" for help.))");
    }},
    { "help", [](Player* player, const std::string& args) {
        (void)player;
        if (args.empty())
            help();
        else
            std::println(R"(Huh?  (Type "?" or "help" for help.))");
    }},
    { "@create", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Create what?");
        else
            create(player, args);
    }},
    { "@destroy", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Destroy what?");
        else
            destroy(player, args);
    }},
    { "take", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Take what?");
        else
            take(player, args);
    }},
    { "drop", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Drop what?");
        else
            drop(player, args);
    }},
};

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

int new_dbref() {
    for (int i=0; i < 10000; ++i) {
         if (!world_db.contains(i))
            return i;
    }
    throw std::runtime_error("No available dbrefs.");
}

void drop(Player* player, const std::string& args) {

    bool found = false;
    for (const auto& [dbref, obj] : world_db) {
        if ((obj->location == player->dbref) && 
                                        str_tolower(obj->name) == str_tolower(args)) {
            obj->location = player->location;
            std::println("You drop the {}.", obj->name);
            found = true;
        }
    }
    if (!found)
            std::println("I don't see that here.");
}

void take(Player* player, const std::string& args) {

    bool found = false;
    for (const auto& [dbref, obj] : world_db) {
        if ((obj->location == player->location) && str_tolower(obj->name) == str_tolower(args)) {
            obj->location = player->dbref;
            std::println("You pick up the {}.", obj->name);
            found = true;
        }
    }
    if (!found)
            std::println("I don't see that here.");
}

void destroy(Player* player, const std::string& args) {
    for (auto it = world_db.begin(); it != world_db.end(); ++it) {
        auto& obj = it->second;
        if ((obj->location == player->dbref || obj->location == player->location) && 
                                        str_tolower(obj->name) == str_tolower(args)) {
            std::println("Object #{} destroyed.", obj->dbref);
            things_db.erase(obj->dbref);
            world_db.erase(it);
            return;
        }
    }
    std::println("I don't see that here.");
}   

void create(Player* player, const std::string& args) {
    Thing* thing = new Thing();
    thing->dbref = new_dbref();
    thing->name = args;
    thing->desc = "Nondescript object.";
    thing->location = player->dbref;
    world_db.emplace(thing->dbref, thing), things_db.emplace(thing->dbref, thing);
    std::println("Created object {} with dbref {}.", thing->name, thing->dbref);
}


void help() {
    std::ifstream file("help.txt");
        if (file) {
            std::string line;
                while (std::getline(file, line)) {
                    std::println("{}", line);
                }
        }
        else
            std::println("Help file not found!");
}

void lookat(Player* player, const std::string& args) {

    bool found = false;
    for (const auto& [dbref, obj] : world_db) {
        if ((obj->location == player->dbref || obj->location == player->location) && 
                                        str_tolower(obj->name) == str_tolower(args)) {
            std::println("{}", obj->name);
            std::println("{}", obj->desc);
            found = true;
        }
    }
    if (!found)
            std::println("I don't see that here.");
}

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
    verb = str_tolower(verb);

    std::getline(std::cin, args);
    if (args[0] == ' ')
        args.erase(0, 1);

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
    std::println(R"(Huh?  (Type "?" or "help" for help.))");
    return true;
}
