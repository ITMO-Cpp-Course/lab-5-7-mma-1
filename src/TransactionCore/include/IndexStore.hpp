#pragma once

#include "Error.hpp"
#include "../../Document_core/include/InvertedIndex.hpp"
#include <unordered_set>

namespace transaction {

class IndexStore {
public:
    Result<bool, ErrorCode> addDocument(const Document& doc);
    Result<bool, ErrorCode> removeDocument(uint64_t id);
    Result<std::unordered_set<uint64_t>, ErrorCode> search(const std::string& query);

    class UpdateTransaction;
    UpdateTransaction beginTransaction();

    size_t getDocumentCount() const;
    bool hasDocument(uint64_t id) const;

private:
    InvertedIndex index_;
    friend class UpdateTransaction;
};

}