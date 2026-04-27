#pragma once
#include "Document.h"
#include <unordered_map>
#include <map>

class InvertedIndex {
public:
    void add_document(Document doc);

    std::map<uint64_t, size_t> search(std::string_view query) const;

private:
    std::unordered_map<uint64_t, Document> docs_;

    std::unordered_map<std::string, std::map<uint64_t, size_t>> index_;
};