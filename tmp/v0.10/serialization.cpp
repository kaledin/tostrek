#include "serialization.hpp"
#include <fstream>
#include "globals.hpp"
#include "json.hpp"
#include "sp_classes.hpp"
#include <array>

using json = nlohmann::json;

void to_json(json& j, const Room& r) {
    j = {
        {"type", "Room"},
        {"dbref", r.dbref},
        {"location", r.location},
        {"name", r.name},
        {"desc", r.desc},
    };
}

void from_json(const json& j, Room& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
    r.location = j.value("location", r.dbref);
}

void to_json(json& j, const Player& r) {
    j = {
        {"type", "Player"},
        {"dbref", r.dbref},
        {"name", r.name},
        {"desc", r.desc},
	{"race", r.race},
	{"location", r.location},
    };
}

void from_json(const json& j, Player& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
    r.race = j.at("race").get<std::string>();
    r.location = j.at("location").get<int>();
}

void to_json(json& j, const Exit& r) {
    j = {
        {"type", "Exit"},
        {"dbref", r.dbref},
        {"name", r.name},
	{"alias", r.alias},
        {"desc", r.desc},
	{"location", r.location},
	{"destination", r.destination},
    };
}

void from_json(const json& j, Exit& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
    r.alias = j.at("alias").get<std::string>();
    r.location = j.at("location").get<int>();
    r.destination = j.at("destination").get<int>();
}

void to_json(json& j, const Thing& r) {
    j = {
        {"type", "Thing"},
        {"dbref", r.dbref},
        {"shipref", r.shipref},
        {"mannedby", r.mannedby},
        {"lock", r.lock},
        {"name", r.name},
        {"desc", r.desc},
	{"location", r.location},
    };
}

void from_json(const json& j, Thing& r) {
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.lock = j.at("lock").get<bool>();
    r.desc = j.at("desc").get<std::string>();
    r.location = j.at("location").get<int>();

    if (j.contains("mannedby"))
        r.mannedby = j.at("mannedby").get<int>();
    else
        r.mannedby = -1;

    if (j.contains("shipref"))
        r.shipref = j.at("shipref").get<int>();
    else
        r.shipref = -1;
}

void to_json(json& j, const Ship& r) {
    j = {
        {"type", "Thing"},
        {"sp_type", r.sp_type},
        {"sp_empire", r.sp_empire},
        {"sp_class", r.sp_class},
        {"coords", r.coords},
        {"maxwarp", r.maxwarp},
        {"curspeed", r.curspeed},
        {"heading", r.heading},
        {"dbref", r.dbref},
        {"lock", r.lock},
        {"name", r.name},
        {"desc", r.desc},
	{"location", r.location},
    };
}

void from_json(const json& j, Ship& r) {
    r.sp_empire = j.at("sp_empire").get<std::string>();
    r.sp_type = j.at("sp_type").get<std::string>();
    r.sp_class = j.at("sp_class").get<std::string>();
    r.coords = j.at("coords").get<std::array<double, 3>>();
    r.maxwarp = j.at("maxwarp").get<double>();
    r.curspeed = j.at("curspeed").get<double>();
    r.heading = j.at("heading").get<std::array<double, 2>>();
    r.dbref = j.at("dbref").get<int>();
    r.name = j.at("name").get<std::string>();
    r.lock = j.at("lock").get<bool>();
    r.desc = j.at("desc").get<std::string>();
    r.location = j.at("location").get<int>();
}

// instantiate objects
void load_world(const std::string& filename) {
    std::ifstream file(filename);
    json j_array;
    file >> j_array;

    Player* player = nullptr;

    for (const auto& j : j_array) {
        std::string type = j.at("type");
        std::string sp_type = j.value("sp_type", "");

        if (type == "Room") {
            Room* room = new Room(j.get<Room>());
            rooms_db[room->dbref] = room;
            world_db[room->dbref] = room;
        }
        else if (type == "Player") {
            player = new Player(j.get<Player>());
            players_db[player->dbref] = player;
            world_db[player->dbref] = player;
        }
        else if (type == "Exit") {
            Exit* exit = new Exit(j.get<Exit>());
            exits_db[exit->dbref] = exit;
            world_db[exit->dbref] = exit;
        }
        else if (type == "Thing" && sp_type == "") {
            Thing* thing = new Thing(j.get<Thing>());
            things_db[thing->dbref] = thing;
            world_db[thing->dbref] = thing;
        }
        else if (type == "Thing" && ((sp_type == "Ship") || (sp_type == "Planet"))) {
            Ship* thing = new Ship(j.get<Ship>());
            things_db[thing->dbref] = thing;
            ships_db[thing->dbref] = thing;
            world_db[thing->dbref] = thing;
        }
    }
}

void save_world(const std::string& filename) {
    json j_array = json::array();

    for (const auto& [_, obj] : world_db) {
        json j;
        if (obj->type == "Room")
            to_json(j, *static_cast<Room*>(obj));
        if (obj->type == "Player")
            to_json(j, *static_cast<Player*>(obj));
        if (obj->type == "Exit")
            to_json(j, *static_cast<Exit*>(obj));
        if (obj->type == "Thing" && obj->sp_type == "")
            to_json(j, *static_cast<Thing*>(obj));
        if (obj->type == "Thing" && (obj->sp_type == "Ship" || obj->sp_type == "Planet"))
            to_json(j, *static_cast<Ship*>(obj));
        j_array.push_back(j);
    }

    std::ofstream file(filename);
    file << j_array.dump(4); // pretty-print with indent=4
}


