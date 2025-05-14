#include <iostream>
#include <fstream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"
#include <optional>

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
    { "@lock", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Lock what?");
        else
            lock(player, args);
    }},
    { "@listobjects", [](Player* player, const std::string& args) {
        if (args.empty())
            listobjects(player);
        else
            std::println(R"(Huh?  (Type "?" or "help" for help.))");
    }},
    { "@dig", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Dig what?");
        else
            dig(player, args);
    }},
    { "enter", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Enter what?");
        else
            enter(player, args);
    }},
    { "leave", [](Player* player, const std::string& args) {
        if (args.empty())
            leave(player);
        else
            std::println(R"(Huh?  (Type "?" or "help" for help.))");
    }},
    { "@desc", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Describe what?");
        else
            desc(player, args);
    }},
    { "@open", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Open what?");
        else
            open(player, args);
    }},
    { "@alias", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Alias what?");
        else
            alias(player, args);
    }},
    { "@tel", [](Player* player, const std::string& args) {
        if (args.empty())
            std::println("Teleport where?");
        else
            teleport(player, args);
    }},
};

std::optional<int> strtoint(const std::string& str) {
    try {
        size_t pos;
        int intstr = std::stoi(str, &pos);
        if (pos != str.length()) {
            std::println("Invalid input: not a number.");
            return std::nullopt;
        }
        return intstr;
    } catch (...) {
        std::println("Invalid input.");
        return std::nullopt; // Indicate failure
    }
}

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

void dig(Player* player, const std::string& args) {
    (void)player;
    Room* room = new Room();
    room->dbref = new_dbref();
    room->name = args;
    room->desc = "Nondescript room.";
    room->location = room->dbref;
    world_db.emplace(room->dbref, room), rooms_db.emplace(room->dbref, room);
    std::println("Created room {} with dbref {}.", room->name, room->dbref);
}


void teleport(Player* player, const std::string& args) {
    int num{};        
    if (auto result = strtoint(args)) {
        num = *result;
        }
    else
        return;
    if (num < 0 || !rooms_db.contains(num)) {
        std::println("Invalid room dbref.");
        return;
    }
    player->location = num;
    look(player);
}

void listobjects(Player* player) {
    (void)player;
    std::println("dbref - type      - name");
    std::println("{:-<45}", "");
    for (const auto& [dbref, obj] : world_db) {
        std::println("#{:<5}- {:<10}- {:<25}", obj->dbref, obj->type, obj->name);
        }
}

void lock(Player* player, const std::string& args) {

    bool found = false;
    for (const auto& [dbref, obj] : things_db) {
        if ((obj->location == player->location) && str_tolower(obj->name) == str_tolower(args)) {
            obj->lock = !obj->lock;
            if (obj->lock)
                std::println("{} - {} locked.", obj->dbref, obj->name);
            else
                std::println("{} - {} unlocked.", obj->dbref, obj->name);
            found = true;
        }
    }
    if (!found)
            std::println("I don't see that here.");
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

void leave(Player* player) {
    if (world_db[player->location]->type != "Thing") {
        std::println("You can't leave.");
        return;
    }
    player->location = world_db[player->location]->location;
    std::println("You leave the {}.", world_db[player->location]->name);
    look(player);
}

void enter(Player* player, const std::string& args) {
    bool found = false;
    for (const auto& [dbref, obj] : things_db) {
        if ((obj->location == player->location) && 
                                        str_tolower(obj->name) == str_tolower(args)) {
            if (obj->lock) {
                std::println("You can't enter that.");
                return;
            }
            player->location = obj->dbref;
            std::println("You enter the {}.", obj->name);
            look(player);
            found = true;
        }
    }
    if (!found)
            std::println("I don't see that here.");
}

void take(Player* player, const std::string& args) {

    bool found = false;
    for (const auto& [dbref, obj] : things_db) {
        if ((obj->location == player->location) && str_tolower(obj->name) == str_tolower(args)) {
            if (obj->lock) {
                std::println("You can't pick that up.");
                return;
            }
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
            if (obj->type == "Room") {
                std::println("Rooms cannot be destroyed yet.");
                return;
            }
            if (obj->type == "Player") {
                std::println("Players cannot be destroyed yet.");
                return;
            }
            std::println("Object #{} destroyed.", obj->dbref);
            things_db.erase(obj->dbref);
            exits_db.erase(obj->dbref);
            delete obj;
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

void open(Player* player, const std::string& args) {
    if (auto pos = args.find('='); pos != std::string::npos) {
        auto arg1 = args.substr(0, pos);
        auto arg2 = args.substr(pos + 1);
        int num{};        
        if (auto result = strtoint(arg2)) {
            num = *result;
        }
        else
            return;
        if (num < 0 || !rooms_db.contains(num)) {
            std::println("Invalid room dbref.");
            return;
        }
        Exit* exit = new Exit();
        exit->dbref = new_dbref();
        exit->name = arg1;
        exit->desc = "Nondescript exit.";
        exit->location = player->location;
        exit->destination = num;
        world_db.emplace(exit->dbref, exit), exits_db.emplace(exit->dbref, exit);
        std::println("Created exit {} with dbref {} leading to {}.", exit->name, exit->dbref, world_db[exit->destination]->name);
        }
    else {
        std::println("Usage: @open <name>=<dest_dbref>");
        return;
    }
}

void alias(Player* player, const std::string& args) {
    if (auto pos = args.find('='); pos != std::string::npos) {
        auto arg1 = args.substr(0, pos);
        auto arg2 = args.substr(pos + 1);
    
        bool found = false;
        
        for (const auto& [dbref, obj] : exits_db) {
            if ((obj->location == player->location) && str_tolower(obj->name) == str_tolower(arg1)) {
                obj->alias = arg2;
                std::println("Alias {} added to exit {}.", obj->alias, obj->name);
                found = true;
            }
        }
        if (!found)
            std::println("I don't see that here.");       
    }
    else {
        std::println("Usage: @alias <exit>=<alias>");
        return;
    }
}


void desc(Player* player, const std::string& args) {
    if (auto pos = args.find('='); pos != std::string::npos) {
        auto arg1 = args.substr(0, pos);
        auto arg2 = args.substr(pos + 1);
    
        bool found = false;
        
        for (const auto& [dbref, obj] : world_db) {
            if ((obj->location == player->dbref || obj->location == player->location) && 
                                        str_tolower(obj->name) == str_tolower(arg1)) {
                obj->desc = arg2;
                std::println("{} description updated.", obj->name);
                found = true;
            }
        }
        if (!found)
            std::println("I don't see that here.");       
    }
    else {
        std::println("Usage: @desc <obj>=<desc>");
        return;
    }
}


void lookat(Player* player, const std::string& args) {
    
    if (args == "me") {
        std::println("{}", player->name);
        std::println("{}", player->desc);
    }
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
    
    for (const auto& [_, obj] : world_db) {
        if (obj->dbref == player->location) {
            std::println("{}{}{}", GREEN, obj->name, CLR_RESET);
            std::println("{}", obj->desc);
            std::println("Contents:");
            for (const auto& [dbref, player1] : players_db) {
                if (player1->location == obj->dbref)
                    std::println("{}{}{}", RED, player1->name, CLR_RESET);
            }
            for (const auto& [dbref, thing] : things_db) {
                if (thing->location == obj->dbref)
                    std::println("{}{}{}", BLUE, thing->name, CLR_RESET);
            }
            if (obj->type == "Room") {
                std::println("Obvious exits:");
                for (const auto& [dbref, exit] : exits_db) {
                    if (exit->location == obj->dbref)
                        std::println("- {} <{}{}{}>", exit->name, CYAN, exit->alias, CLR_RESET);
                }
            }
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
