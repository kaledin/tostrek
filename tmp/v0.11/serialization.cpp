#include "serialization.hpp"
#include <fstream>
#include "globals.hpp"
#include "json.hpp"
#include "objects.hpp"
#include <array>

using json = nlohmann::json;

void to_json(json& j, const GameObj& r) {
    j = {
        {"type", static_cast<int>(r.type)},
        {"location", r.location},
        {"dbref", r.dbref},
        {"name", r.name},
        {"desc", r.desc},
        {"race", r.race},
	    {"alias", r.alias},
	    {"destination", r.destination},
        {"shipref", r.shipref},
        {"mannedby", r.mannedby},
        {"lock", r.lock},
        {"sp_type", r.sp_type},
        {"sp_empire", r.sp_empire},
        {"sp_class", r.sp_class},
        {"coords", r.coords},
        {"maxwarp", r.maxwarp},
        {"curspeed", r.curspeed},
        {"heading", r.heading},
    };
}

void from_json(const json& j, GameObj& r) {
    r.type = static_cast<ObjectType>(j.at("type").get<int>());
    r.dbref = j.at("dbref").get<size_t>();
    r.name = j.at("name").get<std::string>();
    r.desc = j.at("desc").get<std::string>();
    r.location = j.at("location").get<size_t>();
    r.race = j.value("race", "");
    r.alias = j.value("alias", "");
    r.destination = j.value("destination", std::numeric_limits<size_t>::max());
    r.lock = j.value("lock", false);

    if (j.contains("mannedby"))
        r.mannedby = j.at("mannedby").get<size_t>();
    else
        r.mannedby = std::numeric_limits<size_t>::max();

    if (j.contains("shipref"))
        r.shipref = j.at("shipref").get<size_t>();
    else
        r.shipref = std::numeric_limits<size_t>::max();
    r.sp_empire = j.value("sp_empire", "");
    r.sp_type = j.value("sp_type", "");
    r.sp_class = j.value("sp_class", "");
    r.coords = j.value("coords", std::array<double, 3>{0,0,0});
    r.maxwarp = j.value("maxwarp", 0);
    r.curspeed = j.value("curspeed", 0);
    r.heading = j.value("heading", std::array<double, 2>{0,0});
}

// instantiate objects
void load_world(const std::string& filename) {
    std::ifstream file(filename);
    json j_array;
    file >> j_array;

//    GameObj* player = nullptr;

    for (const auto& j : j_array) {
        GameObj* obj = new GameObj(j.get<GameObj>());
        world_db.push_back(obj);
    }
}

void save_world(const std::string& filename) {
    json j_array = json::array();

    for (const auto* obj : world_db) {
        if (obj) {
            j_array.push_back(*obj);
        }
    }

    std::ofstream file(filename);
    file << j_array.dump(4);
}


