#include <iostream>
#include <fstream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"
#include <optional>


// global commands register:
std::unordered_map<std::string, CommandFn> command_table = {
    { "look", cmd_look },
    { "say", cmd_say },
    { "inv", cmd_inv },
    { "help", cmd_help },
    { "take", cmd_take },
    { "drop", cmd_drop },
    { "enter", cmd_enter },
    { "leave", cmd_leave },
    { "@space", cmd_space },
    { "@create", cmd_create },
    { "@destroy", cmd_destroy },
    { "@dig", cmd_dig },
    { "@tel", cmd_tel },
    { "@listobjects", cmd_listobjects },
    { "@lock", cmd_lock },
    { "@open", cmd_open },
    { "@alias", cmd_alias },
    { "@desc", cmd_desc },
};

int new_dbref() {
    for (int i=0; i < 10000; ++i) {
         if (!world_db.contains(i))
            return i;
    }
    throw std::runtime_error("No available dbrefs.");
}

void wrapchar(std::string& str, size_t size) {
    size_t counter = 0;
    std::string word{};
    size_t pos = 0;

    for (auto& ch : str) {
        pos++;
        if (ch == '\n') {
            counter = 0;
            word.clear();
            std::println("");
        }
        else if (ch == '\t') {
            counter += 4;
            word.clear();
            if (counter < size)
                std::print("    ");
            else {
                counter = 4;
                std::println("");
                std::print("    ");
            }
        }
        else if (ch == ' ' || pos == str.length()) {
            word += ch;
            if (counter + word.length() < size) {
                std::print("{}", word);
                counter += word.length();
                word.clear();
            }
            else {
                counter = word.length();;
                std::println("");
                std::print("{}", word);
                word.clear();
            }
        }
        else 
            word += ch;
    }
    std::println("");
}

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

std::optional<double> strtodbl(const std::string& str) {
    try {
        size_t pos;
        double dblstr = std::stod(str, &pos);
        if (pos != str.length()) {
            std::println("Invalid input: not a number.");
            return std::nullopt;
        }
        return dblstr;
    } catch (...) {
        std::println("Invalid input.");
        return std::nullopt; // Indicate failure
    }
}

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

void cmd_look(Player* player, const std::string& args) {
    if (args.empty())
        for (const auto& [_, obj] : world_db) {
            if (obj->dbref == player->location) {
                std::println("{}{}{}", GREEN, obj->name, CLR_RESET);
                wrapchar(obj->desc, 80);
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
    else {
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
}

void cmd_say(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        (void)player;
        std::println("You say, \"{}\"", args);
    }
}

void cmd_inv(Player* player, const std::string& args) {
    if (args.empty()) {
        int loc = player->dbref;

        std::println("You are carrying:");
        for (const auto& [dbref, thing] : things_db) {
            if (thing->location == loc) {
                std::println("{}", thing->name);
            }
        }
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_help(Player *player, const std::string &args) {
    (void)player;
    if (args.empty()) {
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
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_take(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
}

void cmd_drop(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
}

void cmd_leave(Player* player, const std::string& args) {
    if (args.empty()) {
        if (world_db[player->location]->type != "Thing") {
            std::println("You can't leave.");
            return;
        }
        player->location = world_db[player->location]->location;
        std::println("You leave the {}.", world_db[player->location]->name);
        cmd_look(player, "");
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_enter(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
                cmd_look(player, "");
                found = true;
            }
        }
        if (!found)
            std::println("I don't see that here.");
    }
}

void cmd_space(Player* player, const std::string& args) {
    if (args.empty()) {
        (void)player;
        std::println("###  name             sp_type   empire   speed   heading     coords");
        std::println("{:-<79}", "");
        for(const auto& [dbref, obj] : ships_db) 
            std::println("{:<4} {:<15}  {:<8}  {:<7}  {:<6}  {:<5}/{:<4}  {:.2f}:{:.2f}:{:.2f}", obj->dbref, obj->name, obj->sp_type, obj->sp_empire, obj->curspeed, obj->heading[0], obj->heading[1], obj->coords[0], obj->coords[1], obj->coords[2]);
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_destroy(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
}

void cmd_create(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        Thing* thing = new Thing();
        thing->dbref = new_dbref();
        thing->name = args;
        thing->desc = "Nondescript object.";
        thing->location = player->dbref;
        world_db.emplace(thing->dbref, thing), things_db.emplace(thing->dbref, thing);
        std::println("Created object {} with dbref {}.", thing->name, thing->dbref);
    }
}

void cmd_dig(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        (void)player;
        Room* room = new Room();
        room->dbref = new_dbref();
        room->name = args;
        room->desc = "Nondescript room.";
        room->location = room->dbref;
        world_db.emplace(room->dbref, room), rooms_db.emplace(room->dbref, room);
        std::println("Created room {} with dbref {}.", room->name, room->dbref);
    }
}

void cmd_tel(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
        cmd_look(player, "");
    }
}

void cmd_listobjects(Player* player, const std::string& args) {
    if (args.empty()) {
        (void)player;
        std::println("dbref - type      - name");
        std::println("{:-<45}", "");
        for (const auto& [dbref, obj] : world_db) {
            std::println("#{:<5}- {:<10}- {:<25}", obj->dbref, obj->type, obj->name);
            }
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_lock(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
}

void cmd_open(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
}

void cmd_alias(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
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
}

void cmd_desc(Player* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        if (auto pos = args.find('='); pos != std::string::npos) {
            auto arg1 = args.substr(0, pos);
            auto arg2 = args.substr(pos + 1);
            bool found = false;
            if (arg1 == "here" && world_db[player->location]->type == "Room") {
                world_db[player->location]->desc = arg2;
                std::println("{} description updated.", world_db[player->location]->name);
                found = true;
            }
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
            cmd_look(player, "");
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

    // 2. Try object-specific commands on accessible Things
    for (auto& [_, thing] : things_db) {
        if (thing->location == player->location || thing->location == player->dbref) {
            if (thing->object_commands.contains(verb)) {
                thing->object_commands[verb](player, thing, args);
                return true;
            }
        }
    }
    std::println(R"(Huh?  (Type "?" or "help" for help.))");
    return true;
}
