#include <fstream>
#include "globals.hpp"
#include <print>
#include "handle_input.hpp"
#include <optional>
#include <readline/readline.h>
#include <readline/history.h>
#include <charconv>

std::vector<size_t> free_list;

// global commands register:
std::unordered_map<std::string, CommandFn> command_table = {
    { "look", cmd_look },
    { "l", cmd_look},
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
    { "@ex", cmd_ex},
};

std::string to_string(ObjectType t) {
    switch (t) {
        case ObjectType::ROOM:   return "Room";
        case ObjectType::PLAYER: return "Player";
        case ObjectType::THING:  return "Thing";
        case ObjectType::EXIT:  return "Exit";
        case ObjectType::SHIP:  return "Ship";
    }
    return "Unknown"; // just in case
}

// Create a dummy object
GameObj* make_dummy(size_t dbref) {
    GameObj* dummy = new GameObj{};
    dummy->type = THING;
    dummy->dbref = dbref;
    dummy->name = "DUMMY";
    dummy->location = 6; // could be "Deleted Objects" dbref later
    return dummy;
}

size_t new_dbref() {
    if (!free_list.empty()) {
        size_t id = free_list.back();
        free_list.pop_back();
        return id;
    }
    return world_db.size(); // next new slot
}

void delete_object(size_t dbref) {
        if (dbref >= world_db.size()) {
            std::println("Invalid dbref {}.", dbref);
            return;
    }

    GameObj* old = world_db[dbref];
    if (old) {
        delete old; // free memory of the deleted object
    }

    // replace with dummy at the same index
    world_db[dbref] = make_dummy(dbref);

    // mark dbref reusable
    free_list.push_back(dbref);
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

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

void cmd_look(GameObj* player, const std::string& args) {
    if (args.empty())
        for (const auto& obj : world_db) {
            if (obj->dbref == player->location) {
                std::println("{}{}{}", GREEN, obj->name, CLR_RESET);
                wrapchar(obj->desc, 80);
                std::println("Contents:");
                for (const auto& player1 : world_db) {
                    if (player1->location == obj->dbref && player1->type == PLAYER)
                        std::println("{}{}{}", RED, player1->name, CLR_RESET);
                }
                for (const auto& thing : world_db) {
                    if (thing->location == obj->dbref && (thing->type == THING || thing->type == SHIP))
                        std::println("{}{}{}", BLUE, thing->name, CLR_RESET);
                }
                if (obj->type == ROOM) {
                    std::println("Obvious exits:");
                    for (const auto& exit : world_db) {
                        if (exit->type == EXIT && exit->location == obj->dbref)
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
        for (const auto& obj : world_db) {
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

void cmd_say(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        (void)player;
        std::println("You say, \"{}\"", args);
    }
}

void cmd_inv(GameObj* player, const std::string& args) {
    if (args.empty()) {
        std::println("You are carrying:");
        for (const auto& thing : world_db) {
            if (thing->location == player->dbref && thing->type == THING) {
                std::println("{}", thing->name);
            }
        }
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_help([[maybe_unused]] GameObj *player, const std::string &args) {
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

void cmd_take(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        bool found = false;
        for (const auto& obj : world_db) {
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

void cmd_drop(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        bool found = false;
        for (const auto& obj : world_db) {
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

void cmd_leave(GameObj* player, const std::string& args) {
    if (args.empty()) {
        if (world_db[player->location]->type != THING) {
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

void cmd_enter(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        bool found = false;
        for (const auto& obj : world_db) {
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

void cmd_space(GameObj* player, const std::string& args) {
    if (args.empty()) {
        (void)player;
        std::println("###  name             sp_type   empire   speed   heading     coords");
        std::println("{:-<79}", "");
        for(const auto& obj : world_db) {
            if (obj->type != SHIP) {
                continue;
            } 
            std::println("{:<4} {:<15}  {:<8}  {:<7}  {:<6}  {:<5}/{:<4}  {:.2f}:{:.2f}:{:.2f}", obj->dbref, obj->name, obj->sp_type, obj->sp_empire, obj->curspeed, obj->heading[0], obj->heading[1], obj->coords[0], obj->coords[1], obj->coords[2]);
        }
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_destroy(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        for (const auto& obj : world_db) {
            if ((obj->location == player->dbref || obj->location == player->location) && 
                                        str_tolower(obj->name) == str_tolower(args)) {
                if (obj->type == ROOM) {
                    std::println("Rooms cannot be destroyed yet.");
                    return;
                }
                if (obj->type == PLAYER) {
                    std::println("Players cannot be destroyed yet.");
                    return;
                }
                std::println("Object #{} destroyed.", obj->dbref);
                delete_object(obj->dbref);
                return;
            }
        }
        std::println("I don't see that here.");
    }
}

void cmd_create(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        GameObj* thing = new GameObj();
        thing->dbref = new_dbref();
        thing->type = THING;
        thing->name = args;
        thing->desc = "Nondescript object.";
        thing->location = player->dbref;
        if (thing->dbref >= world_db.size())
            world_db.resize(thing->dbref + 1, nullptr);
        world_db[thing->dbref] = thing;
        std::println("Created object {} with dbref {}.", thing->name, thing->dbref);
        std::println("{} {}", world_db[thing->dbref]->name, world_db[thing->dbref]->location);
    }
}

void cmd_dig(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        (void)player;
        GameObj* room = new GameObj();
        room->dbref = new_dbref();
        room->type = ROOM;
        room->name = args;
        room->desc = "Nondescript room.";
        room->location = room->dbref;
        if (room->dbref >= world_db.size())
            world_db.resize(room->dbref + 1, nullptr);
        world_db[room->dbref] = room;
        std::println("Created room {} with dbref {}.", room->name, room->dbref);
    }
}

void cmd_tel(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        size_t dbref{};        
        if (auto result = strtodigit<int>(args)) {
            dbref = static_cast<size_t>(*result);
            }
        else
            return;
        if (dbref >= world_db.size() || world_db[dbref]->type != ROOM) {
            std::println("Invalid room dbref.");
            return;
        }
        player->location = dbref;
        cmd_look(player, "");
    }
}

void cmd_listobjects(GameObj* player, const std::string& args) {
    if (args.empty()) {
        (void)player;
        std::println("dbref - type      - name");
        std::println("{:-<45}", "");
        for (const auto& obj : world_db) {
            std::println("#{:<5}- {:<10}- {:<25}", obj->dbref, to_string(obj->type), obj->name);
            }
    }
    else
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
}

void cmd_lock(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        bool found = false;
        for (const auto& obj : world_db) {
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

void cmd_open(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        if (auto pos = args.find('='); pos != std::string::npos) {
            auto arg1 = args.substr(0, pos);
            auto arg2 = args.substr(pos + 1);
            size_t dbref{};        
            if (auto result = strtodigit<int>(arg2)) {
                dbref = static_cast<size_t>(*result);
            }
            else
                return;
            if (dbref >= world_db.size() || world_db[dbref]->type != ROOM) {
                std::println("Invalid room dbref.");
                return;
            }
            GameObj* exit = new GameObj();
            exit->dbref = new_dbref();
            exit->type = EXIT;
            exit->name = arg1;
            exit->desc = "Nondescript exit.";
            exit->location = player->location;
            exit->destination = dbref;
            if (exit->dbref >= world_db.size())
                world_db.resize(exit->dbref + 1, nullptr);
            world_db[exit->dbref] = exit;
            std::println("Created exit {} with dbref {} leading to {}.", exit->name, exit->dbref, world_db[exit->destination]->name);
            }
        else {
            std::println("Usage: @open <name>=<dest_dbref>");
            return;
        }
    }
}

void cmd_alias(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        if (auto pos = args.find('='); pos != std::string::npos) {
            auto arg1 = args.substr(0, pos);
            auto arg2 = args.substr(pos + 1);
            bool found = false;
        for (const auto& obj : world_db) {
            if ((obj->type == EXIT) && (obj->location == player->location) &&
                str_tolower(obj->name) == str_tolower(arg1)) {
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

void cmd_desc(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        if (auto pos = args.find('='); pos != std::string::npos) {
            auto arg1 = args.substr(0, pos);
            auto arg2 = args.substr(pos + 1);
            bool found = false;
            if (arg1 == "here" && world_db[player->location]->type == ROOM) {
                world_db[player->location]->desc = arg2;
                std::println("{} description updated.", world_db[player->location]->name);
                found = true;
            }
        for (const auto& obj : world_db) {
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

void cmd_ex(GameObj* player, const std::string& args) {
    if (args.empty())
        std::println(R"(Huh?  (Type "?" or "help" for help.))");
    else {
        bool found = false;
        for (const auto& obj : world_db) {
            if ((obj->location == player->location || obj->location == player->dbref) && str_tolower(obj->name) == str_tolower(args)) {
                std::println("dbref   : {}", obj->dbref);
                std::println("type    : {}", to_string(obj->type));
                std::println("name    : {}", obj->name);
                std::println("desc    : {}", obj->desc);
                std::println("location: {} - {}", obj->location, world_db[obj->location]->name);
                
                found = true;
            }
        }
        if (!found)
            std::println("I don't see that here.");
        
        
    }    
}

bool check_exits(GameObj* player, std::string& input) {
    
    for (const auto& exit : world_db) {
        if (exit->type == EXIT && exit->location == player->location && exit->alias == input && exit->type == EXIT) {
            std::println("You walk towards the exit leading {}.", exit->alias);
            if (exit->lock == true)
                std::println("The door appears to be locked.");
            else {
                player->location = exit->destination;
                cmd_look(player, "");
            }
            return true;
        }
    }
    return false;
}   

bool handle_input(GameObj* player) {
    char *input = readline("> ");
    if (!input) return true;

    std::string line(input);
    free(input);

    if (!line.empty())
        add_history(line.c_str());
      
    std::string verb;
    std::string args;

    if (auto pos = line.find(' '); pos != std::string::npos) {
        verb = line.substr(0, pos);
        args = line.substr(pos + 1);
    }
    else
        verb = line;
        
    verb = str_tolower(verb);

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
    for (auto& thing : world_db) {
        if ((thing->location == player->location || thing->location == player->dbref) && thing->type == THING) {
            if (thing->object_commands.contains(verb)) {
                thing->object_commands[verb](player, thing, args);
                return true;
            }
        }
    }
    std::println(R"(Huh?  (Type "?" or "help" for help.))");
    return true;
}
