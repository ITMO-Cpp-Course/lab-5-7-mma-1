#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <unordered_set>
#include "InvertedIndex.hpp"
#include "Document.hpp"

namespace transaction
{
    enum class ErrorCode
    {
        Success = 0,
        InvalidDocumentId,
        EmptySearchQuery,
        EmptyDocumentContent,
        DocumentNotFound,
        DuplicateDocumentId,
        TransactionNotActive,
        TransactionCommitFailed,
        TransactionAlreadyCommitted,
        InternalError,
    };

    template <typename T>
    using Result = std::expected<T, ErrorCode>;

    std::string_view errorCodeToString(ErrorCode code) noexcept;
    std::string formatError(ErrorCode code, std::string_view context = {});

    class IndexStore
    {
    public:
        Result<bool> addDocument(const Document& doc);
        Result<bool> removeDocument(uint64_t id);
        Result<std::unordered_set<uint64_t>> search(std::string_view query) const;
        Result<size_t> getWordCountInDocument(std::string_view word, uint64_t doc_id) const;

        size_t getDocumentCount() const;
        bool hasDocument(uint64_t id) const;

        class UpdateTransaction
        {
        public:
            ~UpdateTransaction();

            UpdateTransaction(const UpdateTransaction&) = delete;
            UpdateTransaction& operator=(const UpdateTransaction&) = delete;


            UpdateTransaction(UpdateTransaction&& other) noexcept;
            UpdateTransaction& operator=(UpdateTransaction&& other) noexcept;

            Result<bool> addDocument(const Document& doc);
            Result<bool> removeDocument(uint64_t id);
            Result<bool> commit();

        private:
            friend class IndexStore;
            explicit UpdateTransaction(IndexStore& store);
            void rollback();

            IndexStore* store_;
            InvertedIndex snapshot_;
            bool isActive_;
            bool isCommitted_;
        };

        UpdateTransaction beginTransaction();

    private:
        InvertedIndex index_;
        friend class UpdateTransaction;
    };
} // namespace transaction
