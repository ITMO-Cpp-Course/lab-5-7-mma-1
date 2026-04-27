#include "InvertedIndex.hpp"
#include "DocumentBuilder.hpp"

void InvertedIndex::add_document(Document doc)
{
    uint64_t id = doc.id;

    DocumentBuilder builder;
    auto words = builder.tokenize(doc.content);

    for (const auto& word : words)
    {
        index_[word][id]++;
    }

    docs_.emplace(id, std::move(doc));
}

std::map<uint64_t, size_t> InvertedIndex::search(std::string_view query) const
{
    DocumentBuilder builder;
    auto tokens = builder.tokenize(query);

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