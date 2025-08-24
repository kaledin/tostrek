#include <string>
#include <iostream>

int main() {

    std::string str{};
    std::getline(std::cin, str);
    std::string arg1{};
    std::string arg2{};
    if (auto pos = str.find(' '); pos != std::string::npos) {
        arg1 = str.substr(0, pos);
        arg2 = str.substr(pos + 1);
    }
    else
        arg1 = str;
    
    std::cout << arg1 << std::endl << arg2 << std::endl;
 
    return 0;
}
