#pragma once

#include "IndexStore.hpp"
#include "unordered_map"

namespace transaction {

class IndexStore::UpdateTransaction {
public:
    explicit UpdateTransaction(IndexStore& store);
    ~UpdateTransaction();

    Result<bool, ErrorCode> addDocument(const Document& doc);
    Result<bool, ErrorCode> removeDocument(uint64_t id);
    Result<bool, ErrorCode> commit();

    UpdateTransaction(const UpdateTransaction&) = delete;
    UpdateTransaction& operator=(const UpdateTransaction&) = delete;

    UpdateTransaction(UpdateTransaction&& other) noexcept;
    UpdateTransaction& operator=(UpdateTransaction&& other) noexcept;

private:
    IndexStore& store_;
    InvertedIndex snapshot_;
    bool isActive_;
    bool isCommitted_;

    void rollback();
};

}
