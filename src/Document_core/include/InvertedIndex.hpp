#pragma once
#include "Document.hpp"
#include "map"
#include "string_view"
#include "unordered_map"
#include "unordered_set"

class InvertedIndex
{
public:
    bool contains(uint64_t id) const
    {
        return docs_.find(id) != docs_.end();
    }

    size_t size() const
    {
        return docs_.size();
    }

    void add_document(Document doc);

    void remove_document(uint64_t id);

    std::unordered_set<uint64_t> find_document(std::string_view query) const;

    size_t get_word_count_in_document(std::string_view word, uint64_t doc_id) const;

private:
    std::unordered_map<uint64_t, Document> docs_;

    std::unordered_map<std::string, std::map<uint64_t, size_t>> index_;
};
