#include "Document.hpp"
#include "DocumentBuilder.hpp"
#include "InvertedIndex.hpp"
#include "catch2/catch_test_macros.hpp"
#include "../src/TransactionCore/include/Error.hpp"
#include "../src/TransactionCore/include/IndexStore.hpp"

TEST_CASE("DocumentBuilder::tokenize works correctly", "[DocumentBuilder]")
{
    SECTION("Punctuation marks are ignored and treated as delimiters")
    {
        auto tokens = DocumentBuilder::tokenize("Hello, Yaroslav! (This: is-a test?);");

        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens[0] == "hello");
        REQUIRE(tokens[1] == "yaroslav");
        REQUIRE(tokens[2] == "this");
        REQUIRE(tokens[3] == "is-a");
        REQUIRE(tokens[4] == "test");
    }

    SECTION("Words with mixed punctuation")
    {
        auto tokens = DocumentBuilder::tokenize("...artem, masha... maksim!!!");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0] == "artem");
        REQUIRE(tokens[1] == "masha");
        REQUIRE(tokens[2] == "maksim");
    }

    SECTION("Empty string returns empty vector")
    {
        auto tokens = DocumentBuilder::tokenize("");
        REQUIRE(tokens.empty());
    }

    SECTION("Single word is parsed correctly")
    {
        auto tokens = DocumentBuilder::tokenize("hello");
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "hello");
    }

    SECTION("Multiple words separated by space")
    {
        auto tokens = DocumentBuilder::tokenize("hello world");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "hello");
        REQUIRE(tokens[1] == "world");
    }

    SECTION("Multiple spaces between words are ignored")
    {
        auto tokens = DocumentBuilder::tokenize("hello    world");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "hello");
        REQUIRE(tokens[1] == "world");
    }
}

TEST_CASE("InvertedIndex basic functionality", "[InvertedIndex]")
{
    InvertedIndex index;

    SECTION("Search in an empty index returns empty set")
    {
        auto result = index.find_document("word");
        REQUIRE(result.empty());
    }

    SECTION("Search with an empty query returns empty set")
    {
        index.add_document(Document(1, "Doc1", "some content here"));
        auto result = index.find_document("");
        REQUIRE(result.empty());
    }

    SECTION("Search for a missing word returns empty set")
    {
        index.add_document(Document(1, "Doc1", "apple banana"));
        auto result = index.find_document("cherry");
        REQUIRE(result.empty());
    }

    SECTION("Add a single document and check results")
    {
        index.add_document(Document(1, "Doc1", "hello world"));

        auto result = index.find_document("hello");
        REQUIRE(result.size() == 1);
        REQUIRE(result.contains(1));

        REQUIRE(index.get_word_count_in_document("hello", 1) == 1);
    }

    SECTION("Word appears multiple times in the same document")
    {
        index.add_document(Document(2, "Doc2", "apple banana apple orange apple"));

        auto result = index.find_document("apple");
        REQUIRE(result.size() == 1);
        REQUIRE(result.contains(2));

        REQUIRE(index.get_word_count_in_document("apple", 2) == 3);
    }

    SECTION("Word appears in multiple different documents")
    {
        index.add_document(Document(1, "Doc1", "hello world"));
        index.add_document(Document(2, "Doc2", "hello universe"));
        index.add_document(Document(3, "Doc3", "world map"));

        auto result = index.find_document("hello");

        REQUIRE(result.size() == 2);
        REQUIRE(result.contains(1));
        REQUIRE(result.contains(2));
        REQUIRE_FALSE(result.contains(3));

        REQUIRE(index.get_word_count_in_document("hello", 1) == 1);
        REQUIRE(index.get_word_count_in_document("hello", 2) == 1);
    }

    SECTION("Multiple documents with multiple word occurrences")
    {
        index.add_document(Document(1, "Doc1", "test test test"));
        index.add_document(Document(2, "Doc2", "test data test"));

        auto result = index.find_document("test");
        REQUIRE(result.size() == 2);

        REQUIRE(index.get_word_count_in_document("test", 1) == 3);
        REQUIRE(index.get_word_count_in_document("test", 2) == 2);
    }

    SECTION("Removing a document")
    {
        index.add_document(Document(1, "Doc1", "apple banana"));
        index.remove_document(1);

        auto result = index.find_document("apple");
        REQUIRE(result.empty());
        REQUIRE(index.get_word_count_in_document("apple", 1) == 0);
    }
}


TEST_CASE("ErrorCode to string conversion", "[Error]") {
    REQUIRE(errorCodeToString(ErrorCode::Success) == "Success");
    REQUIRE(errorCodeToString(ErrorCode::InvalidDocumentId) == "Invalid document ID (must be > 0)");
    REQUIRE(errorCodeToString(ErrorCode::EmptySearchQuery) == "Empty search query");
    REQUIRE(errorCodeToString(ErrorCode::DocumentNotFound) == "Document not found");
    REQUIRE(errorCodeToString(ErrorCode::DuplicateDocumentId) == "Document with this ID already exists");
    REQUIRE(errorCodeToString(ErrorCode::TransactionNotActive) == "No active transaction");
}

TEST_CASE("Format error with context", "[Error]") {
    std::string msg = formatError(ErrorCode::DocumentNotFound, "doc_id=42");
    REQUIRE(msg.find("Document not found") != std::string::npos);
    REQUIRE(msg.find("doc_id=42") != std::string::npos);
}

TEST_CASE("Add document error handling", "[IndexStore]") {
    IndexStore store;

    SECTION("Invalid document ID") {
        Document invalidDoc(0, "Invalid", "Content");
        auto res = store.addDocument(invalidDoc);
        REQUIRE_FALSE(res.has_value());
        REQUIRE(res.error() == ErrorCode::InvalidDocumentId);
    }

    SECTION("Valid document") {
        Document validDoc(1, "Valid", "Content");
        auto res = store.addDocument(validDoc);
        REQUIRE(res.has_value());
        REQUIRE(res.value() == true);
    }

    SECTION("Duplicate document ID") {
        store.addDocument(Document(1, "First", "Content"));
        Document duplicateDoc(1, "Duplicate", "Other");
        auto res = store.addDocument(duplicateDoc);
        REQUIRE_FALSE(res.has_value());
        REQUIRE(res.error() == ErrorCode::DuplicateDocumentId);
    }
}

TEST_CASE("Remove document error handling", "[IndexStore]") {
    IndexStore store;
    store.addDocument(Document(1, "Test", "Content"));

    SECTION("Invalid ID") {
        auto res = store.removeDocument(0);
        REQUIRE_FALSE(res.has_value());
        REQUIRE(res.error() == ErrorCode::InvalidDocumentId);
    }

    SECTION("Document not found") {
        auto res = store.removeDocument(999);
        REQUIRE_FALSE(res.has_value());
        REQUIRE(res.error() == ErrorCode::DocumentNotFound);
    }

    SECTION("Successful removal") {
        auto res = store.removeDocument(1);
        REQUIRE(res.has_value());
        REQUIRE(res.value() == true);
    }
}

TEST_CASE("Search error handling", "[IndexStore]") {
    IndexStore store;
    store.addDocument(Document(1, "Hello", "Hello world"));

    SECTION("Empty query") {
        auto res = store.search("");
        REQUIRE_FALSE(res.has_value());
        REQUIRE(res.error() == ErrorCode::EmptySearchQuery);
    }

    SECTION("Successful search") {
        auto res = store.search("Hello");
        REQUIRE(res.has_value());
        REQUIRE(res.value().size() == 1);
        REQUIRE(res.value().count(1) == 1);
    }
}

TEST_CASE("Get document count", "[IndexStore]") {
    IndexStore store;
    REQUIRE(store.getDocumentCount() == 0);

    store.addDocument(Document(1, "Doc1", "Content1"));
    REQUIRE(store.getDocumentCount() == 1);

    store.addDocument(Document(2, "Doc2", "Content2"));
    REQUIRE(store.getDocumentCount() == 2);

    store.removeDocument(1);
    REQUIRE(store.getDocumentCount() == 1);
}

TEST_CASE("Has document", "[IndexStore]") {
    IndexStore store;
    REQUIRE_FALSE(store.hasDocument(1));

    store.addDocument(Document(1, "Doc1", "Content"));
    REQUIRE(store.hasDocument(1));

    store.removeDocument(1);
    REQUIRE_FALSE(store.hasDocument(1));
}
TEST_CASE("Transaction rollback (RAII)", "[Transaction]") {
    IndexStore store;
    store.addDocument(Document(1, "Original", "Content"));
    store.addDocument(Document(2, "Keep", "Data"));

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(3, "Temp", "Should be rolled back"));
        tx.removeDocument(1);
    }

    REQUIRE(store.hasDocument(1));
    REQUIRE(store.hasDocument(2));
    REQUIRE_FALSE(store.hasDocument(3));
    REQUIRE(store.getDocumentCount() == 2);
}

TEST_CASE("Transaction commit", "[Transaction]") {
    IndexStore store;
    store.addDocument(Document(1, "Original", "Content"));

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(2, "New", "Data"));
        tx.removeDocument(1);
        tx.commit();
    }

    REQUIRE_FALSE(store.hasDocument(1));
    REQUIRE(store.hasDocument(2));
    REQUIRE(store.getDocumentCount() == 1);
}

TEST_CASE("Error inside transaction does not break rollback", "[Transaction]") {
    IndexStore store;
    store.addDocument(Document(1, "Original", "Content"));

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(2, "Temp", "Data"));

        auto res = tx.addDocument(Document(1, "Duplicate", "Fail"));
        REQUIRE_FALSE(res.has_value());
        REQUIRE(res.error() == ErrorCode::DuplicateDocumentId);
    }

    REQUIRE(store.hasDocument(1));
    REQUIRE_FALSE(store.hasDocument(2));
    REQUIRE(store.getDocumentCount() == 1);
}

TEST_CASE("Operations after commit are forbidden", "[Transaction]") {
    IndexStore store;

    auto tx = store.beginTransaction();
    tx.addDocument(Document(1, "Doc", "Content"));
    tx.commit();

    auto res1 = tx.addDocument(Document(2, "Another", "Data"));
    REQUIRE_FALSE(res1.has_value());
    REQUIRE(res1.error() == ErrorCode::TransactionNotActive);

    auto res2 = tx.removeDocument(1);
    REQUIRE_FALSE(res2.has_value());
    REQUIRE(res2.error() == ErrorCode::TransactionNotActive);
}

TEST_CASE("Multiple operations in transaction", "[Transaction]") {
    IndexStore store;
    store.addDocument(Document(1, "Doc1", "A"));
    store.addDocument(Document(2, "Doc2", "B"));

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(3, "Doc3", "C"));
        tx.removeDocument(1);
        tx.addDocument(Document(4, "Doc4", "D"));
        tx.removeDocument(2);
        tx.commit();
    }

    REQUIRE_FALSE(store.hasDocument(1));
    REQUIRE_FALSE(store.hasDocument(2));
    REQUIRE(store.hasDocument(3));
    REQUIRE(store.hasDocument(4));
    REQUIRE(store.getDocumentCount() == 2);
}

TEST_CASE("Transaction with no changes", "[Transaction]") {
    IndexStore store;
    store.addDocument(Document(1, "Doc1", "Content"));

    {
        auto tx = store.beginTransaction();
        tx.commit();
    }

    REQUIRE(store.hasDocument(1));
    REQUIRE(store.getDocumentCount() == 1);
}

TEST_CASE("Transaction move semantics", "[Transaction]") {
    IndexStore store;

    auto tx1 = store.beginTransaction();
    tx1.addDocument(Document(1, "Doc1", "Content"));

    auto tx2 = std::move(tx1);
    tx2.commit();

    REQUIRE(store.hasDocument(1));
}

TEST_CASE("Search after transaction rollback", "[Transaction][Search]") {
    IndexStore store;
    store.addDocument(Document(1, "Apple", "Apple fruit"));
    store.addDocument(Document(2, "Banana", "Banana fruit"));

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(3, "Cherry", "Cherry fruit"));
        tx.removeDocument(1);
    }

    auto results = store.search("fruit");
    REQUIRE(results.has_value());
    REQUIRE(results.value().size() == 1);
    REQUIRE(results.value().count(2) == 1);
}
TEST_CASE("Search after transaction commit", "[Transaction][Search]") {
    IndexStore store;
    store.addDocument(Document(1, "Apple", "Apple fruit"));

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(2, "Banana", "Banana fruit"));
        tx.commit();
    }

    auto results = store.search("fruit");
    REQUIRE(results.has_value());
    REQUIRE(results.value().size() == 2);
}

TEST_CASE("Multiple transactions sequentially", "[Transaction]") {
    IndexStore store;

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(1, "Doc1", "First"));
        tx.commit();
    }

    {
        auto tx = store.beginTransaction();
        tx.addDocument(Document(2, "Doc2", "Second"));
        tx.commit();
    }

    {
        auto tx = store.beginTransaction();
        tx.removeDocument(1);
        tx.commit();
    }

    REQUIRE_FALSE(store.hasDocument(1));
    REQUIRE(store.hasDocument(2));
    REQUIRE(store.getDocumentCount() == 1);
}

TEST_CASE("Large number of documents", "[IndexStore][Performance]") {
    IndexStore store;

    for (int i = 1; i <= 100; i++) {
        Document doc(i, "Doc" + std::to_string(i), "Content " + std::to_string(i));
        store.addDocument(doc);
    }

    REQUIRE(store.getDocumentCount() == 100);

    {
        auto tx = store.beginTransaction();
        for (int i = 51; i <= 100; i++) {
            tx.removeDocument(i);
        }
        tx.commit();
    }

    REQUIRE(store.getDocumentCount() == 50);

    for (int i = 1; i <= 50; i++) {
        REQUIRE(store.hasDocument(i));
    }
}

TEST_CASE("Transaction double commit", "[Transaction]") {
    IndexStore store;

    auto tx = store.beginTransaction();
    tx.addDocument(Document(1, "Doc", "Content"));

    auto res1 = tx.commit();
    REQUIRE(res1.has_value());

    auto res2 = tx.commit();
    REQUIRE_FALSE(res2.has_value());
    REQUIRE(res2.error() == ErrorCode::TransactionNotActive);
}