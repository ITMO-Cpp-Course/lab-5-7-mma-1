#pragma once
#include "Document.h"
#include <vector>
#include <string_view>

class DocumentBuilder {
public:
    static Document build(uint64_t id, std::string name, std::string text);

    static std::vector<std::string> tokenize(std::string_view text);
};