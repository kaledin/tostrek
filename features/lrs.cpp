#include <vector>
#include <map>
#include <print>
#include <algorithm>

int main() {

    // initial lrs_contacts stored on the space object
    std::map<char, int> lrs_cts = {{'A', 123}, {'B',234}, {'C',345}};
    
    // vector of the dbrefs found by new sensor scan
    std::vector<int> found{123,345,567,443};

    // vector of dbrefs to erase
    std::vector<char> toerase{};


    // if lrs_contacts not found in news scan vector, remove from map
    // if found, then erase it from the new scan vector
    for (const auto& pair : lrs_cts) {
        if (!std::ranges::contains(found, pair.second))
            toerase.push_back(pair.first);
        else
            std::erase(found, pair.second);
    }

    for (const auto& it : toerase) {
        lrs_cts.erase(it);
    }

    for (const auto& it : found)
         std::print("{} ", it);
    std::println("");

    char key = 'A';

    // add new contacts to lrs_contacts map
    for (const auto& cts : found) {
        for (char key = 'A'; key <= 'Z'; ++key) {
            if (!lrs_cts.contains(key)) {
                lrs_cts.emplace(key,cts);
                break;
            }
         }
    }

    for (const auto& pair : lrs_cts)
        std::println("{}: {}", pair.first, pair.second);

    return 0;
}
