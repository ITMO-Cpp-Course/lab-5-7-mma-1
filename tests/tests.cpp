
#include "Document.hpp"
#include "DocumentBuilder.hpp"
#include "InvertedIndex.hpp"
#include <catch2/catch_test_macros.hpp>

// ============================================
// Тесты для DocumentBuilder
// ============================================
TEST_CASE("DocumentBuilder::tokenize works correctly", "[DocumentBuilder]")
{
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

// ============================================
// Тесты для InvertedIndex
// ============================================
TEST_CASE("InvertedIndex basic functionality", "[InvertedIndex]")
{
    InvertedIndex index;

    SECTION("Search in an empty index returns empty map")
    {
        auto result = index.search("word");
        REQUIRE(result.empty());
    }

    SECTION("Search with an empty query returns empty map")
    {
        index.add_document({1, "Doc1", "some content here"});
        auto result = index.search("");
        REQUIRE(result.empty());
    }

    SECTION("Search for a missing word returns empty map")
    {
        index.add_document({1, "Doc1", "apple banana"});
        auto result = index.search("cherry");
        REQUIRE(result.empty());
    }

    SECTION("Add a single document and search for a word")
    {
        index.add_document({1, "Doc1", "hello world"});

        auto result = index.search("hello");
        REQUIRE(result.size() == 1);
        REQUIRE(result[1] == 1); // Документ 1, совпадение 1 раз
    }

    SECTION("Word appears multiple times in the same document")
    {
        index.add_document({2, "Doc2", "apple banana apple orange apple"});

        auto result = index.search("apple");
        REQUIRE(result.size() == 1);
        REQUIRE(result[2] == 3); // Документ 2, совпадение 3 раза
    }

    SECTION("Word appears in multiple different documents")
    {
        index.add_document({1, "Doc1", "hello world"});
        index.add_document({2, "Doc2", "hello universe"});
        index.add_document({3, "Doc3", "world map"});

        auto result = index.search("hello");
        REQUIRE(result.size() == 2);
        REQUIRE(result[1] == 1);                 // В Doc1 один раз
        REQUIRE(result[2] == 1);                 // В Doc2 один раз
        REQUIRE(result.find(3) == result.end()); // В Doc3 этого слова нет
    }

    SECTION("Multiple documents with multiple word occurrences")
    {
        index.add_document({1, "Doc1", "test test test"});
        index.add_document({2, "Doc2", "test data test"});

        auto result = index.search("test");
        REQUIRE(result.size() == 2);
        REQUIRE(result[1] == 3); // В Doc1 - 3 раза
        REQUIRE(result[2] == 2); // В Doc2 - 2 раза
    }
}