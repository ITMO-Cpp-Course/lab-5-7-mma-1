#include "../include/IndexStore.hpp"
#include "Document.hpp"
#include "../include/Error.hpp"
#include <unordered_set>
#include <cstdint>
#include <expected>
#include <stdexcept>

namespace transaction
{
    // ----------------------------------------------------------------------
    // add_document
    // ----------------------------------------------------------------------
    Result<bool> IndexStore::add_document(const Document& doc)
    {
        if (doc.get_id() == 0)
        {
            return std::unexpected(ErrorCode::InvalidDocumentId);
        }

        if (main_index_.contains(doc.get_id()))
        {
            return std::unexpected(ErrorCode::DuplicateDocumentId);
        }

        try
        {
            main_index_.add_document(doc);
            return true; // успех
        }
        catch (const std::exception&)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<bool> IndexStore::remove_document(uint64_t id)
    {
        if (id == 0)
        {
            return std::unexpected(ErrorCode::InvalidDocumentId);
        }

        if (!main_index_.contains(id))
        {
            return std::unexpected(ErrorCode::DocumentNotFound);
        }

        try
        {
            main_index_.remove_document(id);
            return true;
        }
        catch (const std::exception&)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<std::unordered_set<uint64_t>> IndexStore::find_document(std::string_view query) const
    {
        if (query.empty())
        {
            return std::unexpected(ErrorCode::EmptySearchQuery);
        }

        try
        {
            return main_index_.find_document(query);
        }
        catch (const std::exception&)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<size_t> IndexStore::get_word_count_in_document(std::string_view word, uint64_t doc_id) const
    {
        try
        {
            auto count = main_index_.get_word_count_in_document(word, doc_id);
            return count;
        }
        catch (const std::exception&)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    size_t IndexStore::getDocumentCount() const
    {
        return main_index_.size(); // используем публичный метод InvertedIndex
    }

    bool IndexStore::hasDocument(uint64_t id) const
    {
        return main_index_.contains(id);
    }

    IndexStore::UpdateTransaction IndexStore::beginTransaction()
    {
        return UpdateTransaction(*this);
    }

    IndexStore::UpdateTransaction::UpdateTransaction(IndexStore& store) noexcept
        : store_(&store), committed_(false)
    {
    }

    IndexStore::UpdateTransaction::UpdateTransaction(UpdateTransaction&& other) noexcept
        : store_(other.store_), committed_(other.committed_)
    {
        other.store_ = nullptr;
        other.committed_ = true;
    }

    IndexStore::UpdateTransaction::~UpdateTransaction()
    {
        if (store_ && !committed_)
        {
            store_->rollbackTransaction();
        }
    }

    Result<void> IndexStore::UpdateTransaction::add_document(Document doc)
    {
        if (!store_) return std::unexpected(ErrorCode::TransactionNotActive);
        return store_->addInTransaction(std::move(doc));
    }

    Result<void> IndexStore::UpdateTransaction::remove_document(uint64_t id)
    {
        if (!store_) return std::unexpected(ErrorCode::TransactionNotActive);
        return store_->removeInTransaction(id);
    }

    Result<void> IndexStore::UpdateTransaction::commit()
    {
        if (!store_) return std::unexpected(ErrorCode::TransactionNotActive);
        if (committed_) return {};
        auto res = store_->commitTransaction();
        if (res)
        {
            committed_ = true;
            store_ = nullptr;
        }
        return res;
    }


    Result<void> IndexStore::addInTransaction(Document doc)
    {
        uint64_t id = doc.get_id();
        if (id == 0) return std::unexpected(ErrorCode::InvalidDocumentId);

        if (main_index_.contains(id) && !removedInTransaction_.contains(id))
        {
            return std::unexpected(ErrorCode::DuplicateDocumentId);
        }
        if (addedInTransaction_.contains(id))
        {
            return std::unexpected(ErrorCode::DuplicateDocumentId);
        }

        addedInTransaction_.emplace(id, std::move(doc));
        removedInTransaction_.erase(id);

        return {};
    }


    Result<void> IndexStore::removeInTransaction(uint64_t id)
    {
        if (id == 0) return std::unexpected(ErrorCode::InvalidDocumentId);

        bool existsInMain = main_index_.contains(id);
        bool existsInAdded = addedInTransaction_.contains(id);

        if (!existsInMain && !existsInAdded)
        {
            return std::unexpected(ErrorCode::DocumentNotFound);
        }
        if (removedInTransaction_.contains(id))
        {
            return std::unexpected(ErrorCode::DocumentNotFound);
        }

        if (existsInAdded)
        {
            addedInTransaction_.erase(id);
        }
        else
        {
            removedInTransaction_.insert(id);
        }

        return {};
    }

    Result<void> IndexStore::commitTransaction()
    {
        try
        {
            for (uint64_t id : removedInTransaction_)
            {
                main_index_.remove_document(id);
            }

            for (auto& [id, doc] : addedInTransaction_)
            {
                main_index_.add_document(std::move(doc));
            }

            addedInTransaction_.clear();
            removedInTransaction_.clear();
            return {};
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::TransactionCommitFailed);
        }
    }

    void IndexStore::rollbackTransaction()
    {
        addedInTransaction_.clear();
        removedInTransaction_.clear();
    }
};

// namespace transaction
