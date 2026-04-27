#include "DocumentBuilder.hpp"
#include <cctype>

Document DocumentBuilder::build(uint64_t id, std::string name, std::string text)
{

    return {id, std::move(name), std::move(text)};
}

std::vector<std::string> DocumentBuilder::tokenize(std::string_view text)
{
    std::vector<std::string> words;
    std::string current;

    for (char c : text)
    {
        if (std::isalpha(static_cast<unsigned char>(c)))
        {
            current += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        else if (!current.empty())
        {
            words.push_back(current);
            current.clear();
        }
    }

    if (!current.empty())
    {
        words.push_back(current);
    }

    return words;
}
