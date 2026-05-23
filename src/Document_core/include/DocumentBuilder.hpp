#pragma once

#include "Document.hpp"
#include "string_view"
#include "vector"

class DocumentBuilder
{
private:
    static bool is_word_char(char c);

public:
    static std::string lowercase(std::string_view text);

    static Document build(uint64_t id, std::string name, std::string text);

    static std::vector<std::string> tokenize(std::string_view text);
};
