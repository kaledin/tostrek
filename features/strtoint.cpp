#include <print>
#include <string>

int strtoint(const std::string& str) {
    try {
        size_t pos;
        int intstr = std::stoi(str, &pos);
        if (pos != str.length()) {
            std::println("Invalid input: not a number.");
        }
        return intstr;

    } catch (const std::invalid_argument&) {
        std::println("Invalid input: not a number.");
    } catch (const std::out_of_range&) {
        std::println("Invalid input: number out of range.");
    }
}

int main() {

    std::string teststr = "42";
    std::println("{}", strtoint(teststr));
    teststr = "-42";
    std::println("{}", strtoint(teststr));


    return 0;
}
