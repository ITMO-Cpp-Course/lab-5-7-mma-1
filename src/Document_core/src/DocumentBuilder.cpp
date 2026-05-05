#include <DocumentBuilder.hpp>
#include <cctype>
#include <sstream>
#include <vector>

std::vector<std::string> DocumentBuilder::tokenize(std::string_view text)
{
    std::vector<std::string> words;
    std::string current_word;

    for (char c : text)
    {
        if (std::isalnum(static_cast<unsigned char>(c)))
        {
            current_word += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        else
        {
            if (!current_word.empty())
            {
                words.push_back(current_word);
                current_word.clear();
            }
        }
    }

    if (!current_word.empty())
    {
        words.push_back(current_word);
    }

    return words;
}

