#include <iostream>
#include <thread>
#include <chrono>
#include <format>   // C++20 or C++23 for std::format

int main() {
    using namespace std::chrono_literals;

    for (int i = 10; i >= 0; --i) {
        std::cout << std::format("\rCountdown: {:2}", i) << std::flush;
        std::this_thread::sleep_for(0.1s);
    }

    std::cout << "\nDone.\n";
    return 0;
}
