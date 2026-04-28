#include <DocumentBuilder.hpp>
#include <InvertedIndex.hpp>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <utility>
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

void InvertedIndex::add_document(Document doc)
{
    uint64_t id = doc.id;

    auto words = DocumentBuilder::tokenize(doc.content);

    for (const auto& word : words)
    {
        index_[word][id]++;
    }

    docs_.emplace(id, std::move(doc));
}

std::map<uint64_t, size_t> InvertedIndex::search(std::string_view query) const
{
    auto tokens = DocumentBuilder::tokenize(query);

    if (tokens.empty())
    {
        return {};
    }

    std::string word = tokens[0];

    auto it = index_.find(word);
    if (it != index_.end())
    {
        return it->second;
    }

    return {};
}