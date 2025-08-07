#include <print>
#include <string>
#include <sstream>


void wrapstring(std::string& str, int size) {
   int counter = 0;
   for (auto& ch : str) {
       if (counter < size) {
           std::print("{}", ch);
           counter++;
       }
       else {
           std::println("");
           std::print("{}", ch);
           counter = 1;
       }
    }
    std::println("");
}

void wrapspace(std::string& str, int size) {
    int counter = 0;
    std::istringstream iss(str);
    std::string word{};

    while (iss >> word) {
        counter += word.length() + 1;
        if (counter < size)
            std::print("{} ", word);
        else {
            std::println("");
            std::print("{} ", word);
            counter = word.length() + 1;
        }
    }
    std::println("");
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

int main() {
    std::string test = "\tYou stand in a low-ceilinged, angular corridor lit by pulsating green bioluminescent panels recessed into the walls. The air carries a sterile, ozonic tang, chilled to precisely 14°C. \n\tThe obsidian-black deck plating is inlaid with faintly glowing blue circuitry patterns that thrum with power beneath your boots. Slanted doorways - each marked with a crimson raptor sigil of the Romulan Star Empire - slide open with a hydraulic hiss. Security panels flicker with Tal Shiar encryption codes, their interfaces responding only to authorized crew neural imprints. Every surface reflects the Romulan obsession with controlled precision: no warmth, no wasted space, only predatory efficiency.You are in the ship's boom corridor. The door forward leades to the bridge, aft leads to engineering.";
 
    std::println("{}", test);

    wrapstring(test,80);
    wrapspace(test,80);
    wrapchar(test,80);    
}
