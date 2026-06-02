#pragma once

#include "Document.hpp"
#include "Error.hpp"
#include "InvertedIndex.hpp"
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace transaction
{
class IndexStore
{
  public:
    Result<bool> add_document(const Document&);
    Result<bool> remove_document(uint64_t id);
    Result<std::unordered_set<uint64_t>> find_document(std::string_view query) const;
    Result<size_t> get_word_count_in_document(std::string_view word, uint64_t doc_id) const;

    size_t getDocumentCount() const;
    bool hasDocument(uint64_t id) const;

    class UpdateTransaction
    {
      public:
        ~UpdateTransaction();
        UpdateTransaction(UpdateTransaction&& other) noexcept;
        Result<void> add_document(Document doc);
        Result<void> remove_document(uint64_t id);
        Result<void> commit();

      private:
        friend class IndexStore;
        explicit UpdateTransaction(IndexStore& store) noexcept;
        IndexStore* store_;
        bool committed_ = false;
    };

    UpdateTransaction beginTransaction();

  private:
    InvertedIndex main_index_;

    Result<void> addInTransaction(Document doc);
    Result<void> removeInTransaction(uint64_t id);
    Result<void> commitTransaction();
    void rollbackTransaction();

    std::unordered_map<uint64_t, Document> addedInTransaction_;
    std::unordered_set<uint64_t> removedInTransaction_;
    bool inTransaction_ = false;
};
} // namespace transaction
