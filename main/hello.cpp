#include "hello.h"

#include <iostream>
#include <unordered_map>
#include <string>

void SayHello() {
    std::unordered_map<std::string, int> mapping{{"a", 1}, {"b", 2}, {"c", 3}};

    // Destructure by reference.
    for (const auto& [key, value] : mapping) {
        std::cout << "hello from cpp17" << key << std::endl;
    }
}