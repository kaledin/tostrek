#include <print>

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

int main() {

    std::string s = "HeLlO";
    std::println("{}", s);
    
    std::println("{}", str_tolower(s));
    std::println("{}", s);
 
    return 0;
}


