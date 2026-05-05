#pragma once

#include "Document.hpp"
#include <string_view>
#include <vector>

class DocumentBuilder
{
  private:
    static std::string lowercase(std::string_view text);

    static bool is_word_char(char c);

  public:
    static Document build(uint64_t id, std::string name, std::string text);

    static std::vector<std::string> tokenize(std::string_view text);
};