#pragma once

#include "classes.hpp"

bool handle_input(Player* player);

std::optional<int> strtoint(const std::string& str);
std::optional<double> strtodbl(const std::string& str);
std::string str_toupper(std::string s); 
void look(Player* player);
bool check_exits(Player* player, std::string& input);
void help();
void inv(Player* player);
void space(Player* player);
void lookat(Player* player, const std::string& args);
void create(Player* player, const std::string& args);
void destroy(Player* player, const std::string& args);
void take(Player* player, const std::string& args);
void drop(Player* player, const std::string& args);
void listobjects(Player* player);
void leave(Player* player);
void lock(Player* player, const std::string& args);
void dig(Player* player, const std::string& args);
void teleport(Player* player, const std::string& args);
void alias(Player* player, const std::string& args);
void open(Player* player, const std::string& args);
void desc(Player* player, const std::string& args);
void enter(Player* player, const std::string& args);
