#pragma once
#include <string>
#include "classes.hpp"

void save_world(const std::string& filename);
Player* load_world(const std::string& filename);
