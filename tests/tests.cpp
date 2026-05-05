#include "Document.hpp"
#include <DocumentBuilder.hpp>
#include <InvertedIndex.hpp>
#include <catch2/catch_test_macros.hpp>

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