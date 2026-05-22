#include "DocumentBuilder.hpp"
#include "cctype"
#include "utility"

Document DocumentBuilder::build(uint64_t id, std::string name, std::string text)
{
    return Document(id, std::move(name), std::move(text));
}

std::vector<std::string> DocumentBuilder::tokenize(std::string_view text)
{
    std::vector<std::string> words;
    std::string current_word;

    for (char c : text)
    {
        if (is_word_char(c))
        {
            current_word += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        else
        {
            if (!current_word.empty())
            {
                size_t start = 0;
                size_t end = current_word.size();

                while (start < end && current_word[start] == '-')
                {
                    ++start;
                }
                while (end > start && current_word[end - 1] == '-')
                {
                    --end;
                }

                if (start < end)
                {
                    std::string clean_word = current_word.substr(start, end - start);
                    if (!clean_word.empty() && clean_word != "-")
                    {
                        words.push_back(std::move(clean_word));
                    }
                }
                current_word.clear();
            }
        }
    }

    if (!current_word.empty())
    {
        size_t start = 0;
        size_t end = current_word.size();

        while (start < end && current_word[start] == '-')
        {
            ++start;
        }
        while (end > start && current_word[end - 1] == '-')
        {
            --end;
        }

        if (start < end)
        {
            std::string clean_word = current_word.substr(start, end - start);
            if (!clean_word.empty() && clean_word != "-")
            {
                words.push_back(std::move(clean_word));
            }
        }
    }

    return words;
}

std::string DocumentBuilder::lowercase(std::string_view text)
{
    std::string result;
    result.reserve(text.size());
    for (char c : text)
    {
        result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

bool DocumentBuilder::is_word_char(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) || c == '-';
}