#include "../include/Transaction.hpp"

namespace transaction {

IndexStore::UpdateTransaction::UpdateTransaction(IndexStore& store)
    : store_(store)
    , snapshot_(store.index_)
    , isActive_(true)
    , isCommitted_(false) {}

IndexStore::UpdateTransaction::~UpdateTransaction() {
    if (isActive_ && !isCommitted_) {
        rollback();
    }
}

Result<bool, ErrorCode> IndexStore::UpdateTransaction::addDocument(const Document& doc) {
    if (!isActive_) {
        return std::unexpected(ErrorCode::TransactionNotActive);
    }

    if (doc.get_id() == 0) {
        return std::unexpected(ErrorCode::InvalidDocumentId);
    }

    if (store_.index_.docs_.count(doc.get_id()) > 0) {
        return std::unexpected(ErrorCode::DuplicateDocumentId);
    }

    try {
        store_.index_.add_document(doc);
        return true;
    } catch (...) {
        return std::unexpected(ErrorCode::InternalError);
    }
}

Result<bool, ErrorCode> IndexStore::UpdateTransaction::removeDocument(uint64_t id) {
    if (!isActive_) {
        return std::unexpected(ErrorCode::TransactionNotActive);
    }

    if (id == 0) {
        return std::unexpected(ErrorCode::InvalidDocumentId);
    }

    if (store_.index_.docs_.count(id) == 0) {
        return std::unexpected(ErrorCode::DocumentNotFound);
    }

    try {
        store_.index_.remove_document(id);
        return true;
    } catch (...) {
        return std::unexpected(ErrorCode::InternalError);
    }
}

Result<bool, ErrorCode> IndexStore::UpdateTransaction::commit() {
    if (!isActive_) {
        return std::unexpected(ErrorCode::TransactionNotActive);
    }

    if (isCommitted_) {
        return std::unexpected(ErrorCode::TransactionAlreadyCommitted);
    }

    isCommitted_ = true;
    isActive_ = false;
    return true;
}

void IndexStore::UpdateTransaction::rollback() {
    store_.index_ = snapshot_;
}

IndexStore::UpdateTransaction::UpdateTransaction(UpdateTransaction&& other) noexcept
    : store_(other.store_)
    , snapshot_(std::move(other.snapshot_))
    , isActive_(other.isActive_)
    , isCommitted_(other.isCommitted_) {
    other.isActive_ = false;
    other.isCommitted_ = true;
}

IndexStore::UpdateTransaction& IndexStore::UpdateTransaction::operator=(UpdateTransaction&& other) noexcept {
    if (this != &other) {
        if (isActive_ && !isCommitted_) {
            rollback();
        }

        store_ = other.store_;
        snapshot_ = std::move(other.snapshot_);
        isActive_ = other.isActive_;
        isCommitted_ = other.isCommitted_;

        other.isActive_ = false;
        other.isCommitted_ = true;
    }
    return *this;
}

}
