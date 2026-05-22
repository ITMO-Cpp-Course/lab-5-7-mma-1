#include "InvertedIndex.hpp"
#include "DocumentBuilder.hpp"
#include "utility"

void InvertedIndex::add_document(Document doc)
{
    remove_document(doc.get_id());

    auto words = DocumentBuilder::tokenize(doc.get_content());
    uint64_t id = doc.get_id();

    for (const auto& word : words)
    {
        index_[word][id]++;
    }

    docs_.emplace(id, std::move(doc));
}

void InvertedIndex::remove_document(uint64_t id)
{
    auto doc_it = docs_.find(id);
    if (doc_it == docs_.end())
    {
        return;
    }

    auto words = DocumentBuilder::tokenize(doc_it->second.get_content());

    for (const auto& word : words)
    {
        auto word_it = index_.find(word);
        if (word_it != index_.end())
        {
            auto& doc_map = word_it->second;

            doc_map[id]--;

            if (doc_map[id] == 0)
            {
                doc_map.erase(id);
            }

            if (doc_map.empty())
            {
                index_.erase(word_it);
            }
        }
    }

    docs_.erase(doc_it);
}

std::unordered_set<uint64_t> InvertedIndex::find_document(std::string_view query) const
{
    auto tokens = DocumentBuilder::tokenize(query);

    if (tokens.empty())
    {
        return {};
    }

    const std::string& word = tokens[0];

    auto it = index_.find(word);
    if (it == index_.end())
    {
        return {};
    }

    std::unordered_set<uint64_t> result;
    for (const auto& [doc_id, count] : it->second)
    {
        result.insert(doc_id);
    }

    return result;
}

size_t InvertedIndex::get_word_count_in_document(std::string_view word, uint64_t doc_id) const
{
    auto tokens = DocumentBuilder::tokenize(word);
    if (tokens.empty())
    {
        return 0;
    }

    const std::string& normalized_word = tokens[0];

    auto word_it = index_.find(normalized_word);
    if (word_it == index_.end())
    {
        return 0;
    }

    auto doc_it = word_it->second.find(doc_id);
    if (doc_it == word_it->second.end())
    {
        return 0;
    }

    return doc_it->second;
}