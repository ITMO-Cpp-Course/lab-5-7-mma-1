#include "../include/Transaction.hpp"

namespace transaction
{
    std::string_view errorCodeToString(ErrorCode code) noexcept
    {
        using enum ErrorCode;
        switch (code)
        {
        case Success: return "Success";
        case InvalidDocumentId: return "Invalid document ID (must be > 0)";
        case EmptySearchQuery: return "Empty search query";
        case EmptyDocumentContent: return "Empty document content";
        case DocumentNotFound: return "Document not found";
        case DuplicateDocumentId: return "Document with this ID already exists";
        case TransactionNotActive: return "No active transaction";
        case TransactionCommitFailed: return "Failed to commit transaction";
        case TransactionAlreadyCommitted: return "Transaction already committed";
        case InternalError: return "Internal error occurred";
        default: return "Unknown error";
        }
    }

    std::string formatError(ErrorCode code, std::string_view context)
    {
        std::string result(errorCodeToString(code));
        if (!context.empty())
        {
            result += " [context: ";
            result += context;
            result += ']';
        }
        return result;
    }


    Result<bool> IndexStore::addDocument(const Document& doc)
    {
        if (doc.get_id() == 0) return std::unexpected(ErrorCode::InvalidDocumentId);
        if (index_.contains(doc.get_id())) return std::unexpected(ErrorCode::DuplicateDocumentId);

        try
        {
            index_.add_document(doc);
            return true;
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<bool> IndexStore::removeDocument(uint64_t id)
    {
        if (id == 0) return std::unexpected(ErrorCode::InvalidDocumentId);
        if (!index_.contains(id)) return std::unexpected(ErrorCode::DocumentNotFound);

        try
        {
            index_.remove_document(id);
            return true;
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<std::unordered_set<uint64_t>> IndexStore::search(std::string_view query) const
    {
        if (query.empty()) return std::unexpected(ErrorCode::EmptySearchQuery);

        try
        {
            return index_.find_document(query);
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<size_t> IndexStore::getWordCountInDocument(std::string_view word, uint64_t doc_id) const
    {
        try
        {
            return index_.get_word_count_in_document(word, doc_id);
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    size_t IndexStore::getDocumentCount() const
    {
        return index_.size();
    }

    bool IndexStore::hasDocument(uint64_t id) const
    {
        return index_.contains(id);
    }

    IndexStore::UpdateTransaction IndexStore::beginTransaction()
    {
        return UpdateTransaction(*this);
    }


    IndexStore::UpdateTransaction::UpdateTransaction(IndexStore& store)
        : store_(&store), snapshot_(store.index_), isActive_(true), isCommitted_(false)
    {
    }

    IndexStore::UpdateTransaction::~UpdateTransaction()
    {
        if (isActive_ && !isCommitted_ && store_)
        {
            rollback();
        }
    }

    IndexStore::UpdateTransaction::UpdateTransaction(UpdateTransaction&& other) noexcept
        : store_(other.store_), snapshot_(std::move(other.snapshot_)),
          isActive_(other.isActive_), isCommitted_(other.isCommitted_)
    {
        other.isActive_ = false;
        other.store_ = nullptr;
    }

    IndexStore::UpdateTransaction& IndexStore::UpdateTransaction::operator=(UpdateTransaction&& other) noexcept
    {
        if (this != &other)
        {
            if (isActive_ && !isCommitted_ && store_)
            {
                rollback();
            }

            store_ = other.store_;
            snapshot_ = std::move(other.snapshot_);
            isActive_ = other.isActive_;
            isCommitted_ = other.isCommitted_;

            other.isActive_ = false;
            other.store_ = nullptr;
        }
        return *this;
    }

    Result<bool> IndexStore::UpdateTransaction::addDocument(const Document& doc)
    {
        if (!isActive_) return std::unexpected(ErrorCode::TransactionNotActive);
        if (doc.get_id() == 0) return std::unexpected(ErrorCode::InvalidDocumentId);
        if (store_->index_.contains(doc.get_id())) return std::unexpected(ErrorCode::DuplicateDocumentId);

        try
        {
            store_->index_.add_document(doc);
            return true;
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<bool> IndexStore::UpdateTransaction::removeDocument(uint64_t id)
    {
        if (!isActive_) return std::unexpected(ErrorCode::TransactionNotActive);
        if (id == 0) return std::unexpected(ErrorCode::InvalidDocumentId);
        if (!store_->index_.contains(id)) return std::unexpected(ErrorCode::DocumentNotFound);

        try
        {
            store_->index_.remove_document(id);
            return true;
        }
        catch (...)
        {
            return std::unexpected(ErrorCode::InternalError);
        }
    }

    Result<bool> IndexStore::UpdateTransaction::commit()
    {
        if (!isActive_) return std::unexpected(ErrorCode::TransactionNotActive);
        if (isCommitted_) return std::unexpected(ErrorCode::TransactionAlreadyCommitted);

        isCommitted_ = true;
        isActive_ = false;
        return true;
    }

    void IndexStore::UpdateTransaction::rollback()
    {
        if (store_)
        {
            store_->index_ = std::move(snapshot_);
        }
    }
} // namespace transaction
