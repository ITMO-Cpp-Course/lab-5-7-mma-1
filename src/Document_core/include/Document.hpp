#pragma once
#include <string>
#include <cstdint>

struct Document {
    uint64_t id;
    std::string name;
    std::string content;
};