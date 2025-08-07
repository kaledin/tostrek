#pragma once

#include "classes.hpp"

std::optional<int> strtoint(const std::string& str);
std::optional<double> strtodbl(const std::string& str);
std::string str_toupper(std::string s); 

bool check_exits(Player* player, std::string& input);
bool handle_input(Player* player);

void cmd_look(Player* player, const std::string& args);
void cmd_say(Player* player, const std::string& args);
void cmd_inv(Player* player, const std::string& args);
void cmd_help(Player* player, const std::string& args);
void cmd_take(Player* player, const std::string& args);
void cmd_drop(Player* player, const std::string& args);
void cmd_enter(Player* player, const std::string& args);
void cmd_leave(Player* player, const std::string& args);
void cmd_space(Player* player, const std::string& args);
void cmd_create(Player* player, const std::string& args);
void cmd_destroy(Player* player, const std::string& args);
void cmd_dig(Player* player, const std::string& args);
void cmd_tel(Player* player, const std::string& args);
void cmd_listobjects(Player* player, const std::string& args);
void cmd_lock(Player* player, const std::string& args);
void cmd_open(Player* player, const std::string& args);
void cmd_alias(Player* player, const std::string& args);
void cmd_desc(Player* player, const std::string& args);

