#include <print>
#include <fstream>
#include <unordered_map>
#include "globals.hpp"
#include <thread>
#include "space.hpp"
#include "classes.hpp"
#include "sp_classes.hpp"
#include "handle_input.hpp"
#include "serialization.hpp"

using json = nlohmann::json;

// global object storage
std::unordered_map<int, GameObj*> world_db;
std::unordered_map<int, Room*> rooms_db;
std::unordered_map<int, Player*> players_db;
std::unordered_map<int, Exit*> exits_db;
std::unordered_map<int, Thing*> things_db;
std::unordered_map<int, Ship*> ships_db;

void cleanup(std::unordered_map<int, GameObj*>& db) {
    for (auto& [_, obj] : db)
         delete obj;
    db.clear();
    rooms_db.clear();
    players_db.clear();
    exits_db.clear();
    things_db.clear();
    ships_db.clear();
}

int main() {


    Player* player = load_world("world.json");
    // Global control flag`  
    running = true;

    std::thread space_thread(tick_loop);
    console_init();
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

    while (handle_input(player)) {
    }

    running = false;
    space_thread.join();
    save_world("world.json");    
    cleanup(world_db);
    
    return 0;
}
