#include "../include/IndexStore.hpp"

namespace transaction {

Result<bool, ErrorCode> IndexStore::addDocument(const Document& doc) {
    if (doc.get_id() == 0) {
        return std::unexpected(ErrorCode::InvalidDocumentId);
    }

    if (index_.docs_.count(doc.get_id()) > 0) {
        return std::unexpected(ErrorCode::DuplicateDocumentId);
    }

    try {
        index_.add_document(doc);
        return true;
    } catch (...) {
        return std::unexpected(ErrorCode::InternalError);
    }
}

Result<bool, ErrorCode> IndexStore::removeDocument(uint64_t id) {
    if (id == 0) {
        return std::unexpected(ErrorCode::InvalidDocumentId);
    }

    if (index_.docs_.count(id) == 0) {
        return std::unexpected(ErrorCode::DocumentNotFound);
    }

    try {
        index_.remove_document(id);
        return true;
    } catch (...) {
        return std::unexpected(ErrorCode::InternalError);
    }
}

Result<std::unordered_set<uint64_t>, ErrorCode> IndexStore::search(const std::string& query) {
    if (query.empty()) {
        return std::unexpected(ErrorCode::EmptySearchQuery);
    }

    try {
        return index_.find_document(query);
    } catch (...) {
        return std::unexpected(ErrorCode::InternalError);
    }
}

size_t IndexStore::getDocumentCount() const {
    return index_.docs_.size();
}

bool IndexStore::hasDocument(uint64_t id) const {
    return index_.docs_.count(id) > 0;
}

IndexStore::UpdateTransaction IndexStore::beginTransaction() {
    return UpdateTransaction(*this);
}

}