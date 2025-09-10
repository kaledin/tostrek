#include <atomic>
#include <print>
#include <fstream>
#include "globals.hpp"
#include <thread>
#include "space.hpp"
#include "objects.hpp"
#include "handle_input.hpp"
#include "serialization.hpp"
#include <readline/history.h>

// global object storage
std::vector<GameObj*> world_db;

std::atomic<bool> running{false};

void cleanup(std::vector<GameObj*>& db) {
    for (auto& obj : db)
         delete obj;
    db.clear();
}

int main() {

    load_world("world.json");
    GameObj* player = world_db[1];

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
    cmd_look(player, "");

    while (handle_input(player)) {
    }

    running = false;
    space_thread.join();
    save_world("world.json");    
    cleanup(world_db);
    clear_history();
        
    return 0;
}
