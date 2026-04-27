#pragma once
#include <cstdint>
#include <string>

struct Document
{
    uint64_t id;
    std::string name;
    std::string content;
};