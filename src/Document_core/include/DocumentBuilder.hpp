#pragma once
#include "Document.hpp"
#include <string_view>
#include <vector>

class DocumentBuilder
{
  public:
    static std::vector<std::string> tokenize(std::string_view text);
};