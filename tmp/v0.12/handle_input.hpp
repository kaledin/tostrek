#pragma once

#include "objects.hpp"

template <typename T>
std::optional<T> strtodigit(const std::string& str) {
    T value{};
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);
    if (result.ec == std::errc() && result.ptr == str.data() + str.size())
        return value;
    return std::nullopt;
}

std::optional<int> strtoint(const std::string& str);
std::optional<double> strtodbl(const std::string& str);
std::string str_toupper(std::string s); 

bool check_exits(GameObj* player, std::string& input);
bool handle_input(GameObj* player);

void cmd_look(GameObj* player, const std::string& args);
void cmd_say(GameObj* player, const std::string& args);
void cmd_inv(GameObj* player, const std::string& args);
void cmd_help(GameObj* player, const std::string& args);
void cmd_take(GameObj* player, const std::string& args);
void cmd_drop(GameObj* player, const std::string& args);
void cmd_enter(GameObj* player, const std::string& args);
void cmd_leave(GameObj* player, const std::string& args);
void cmd_space(GameObj* player, const std::string& args);
void cmd_create(GameObj* player, const std::string& args);
void cmd_destroy(GameObj* player, const std::string& args);
void cmd_dig(GameObj* player, const std::string& args);
void cmd_tel(GameObj* player, const std::string& args);
void cmd_listobjects(GameObj* player, const std::string& args);
void cmd_lock(GameObj* player, const std::string& args);
void cmd_open(GameObj* player, const std::string& args);
void cmd_alias(GameObj* player, const std::string& args);
void cmd_desc(GameObj* player, const std::string& args);

