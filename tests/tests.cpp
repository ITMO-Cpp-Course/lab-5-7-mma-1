#include "Document.hpp"
#include "InvertedIndex.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InvertedIndex - Base Operations", "[InvertedIndex]")
{
    InvertedIndex index;

    SECTION("Search in empty index")
    {
        auto res = index.search("computer");
        REQUIRE(res.empty());
    }

    SECTION("Search with empty query")
    {
        auto res = index.search("");
        REQUIRE(res.empty());
    }
}

TEST_CASE("InvertedIndex - Single Document Tracking", "[InvertedIndex]")
{
    InvertedIndex index;
    Document doc{1, "doc1.txt", "hello beautiful world and hello again"};
    index.add_document(doc);

    SECTION("Find existing word")
    {
        auto res = index.search("world");
        REQUIRE(res.size() == 1);
        REQUIRE(res.at(1) == 1); // В документе id=1 встретилось 1 раз
    }

    SECTION("Word frequency verification")
    {
        auto res = index.search("hello");
        REQUIRE(res.size() == 1);
        REQUIRE(res.at(1) == 2); // Слово 'hello' встретилось ровно 2 раза!
    }

    SECTION("Find missing word")
    {
        auto res = index.search("cplusplus");
        REQUIRE(res.empty());
    }
}

TEST_CASE("InvertedIndex - Multiple Documents Integration", "[InvertedIndex]")
{
    InvertedIndex index;
    index.add_document({10, "doc10.txt", "apple banana orange"});
    index.add_document({20, "doc20.txt", "banana kiwi"});
    index.add_document({30, "doc30.txt", "apple kiwi banana orange"});

    SECTION("Word present in all documents")
    {
        auto res = index.search("banana");
        REQUIRE(res.size() == 3);
        REQUIRE(res.at(10) == 1);
        REQUIRE(res.at(20) == 1);
        REQUIRE(res.at(30) == 1);
    }

    SECTION("Word present in some documents")
    {
        auto res = index.search("apple");
        REQUIRE(res.size() == 2);
        REQUIRE(res.at(10) == 1);
        REQUIRE(res.at(30) == 1);
        REQUIRE(res.find(20) == res.end()); // Во 2 доке его нет
    }

    SECTION("Word present in only one document")
    {
        auto res = index.search("kiwi");
        REQUIRE(res.size() == 2);
        REQUIRE(res.at(20) == 1);
        REQUIRE(res.at(30) == 1);
    }
}

TEST_CASE("InvertedIndex - Case Insensitivity", "[InvertedIndex][DocumentBuilder]")
{
    InvertedIndex index;
    index.add_document({40, "test.txt", "HELLO world Hello"});

    SECTION("Search lowercase representation")
    {
        auto res = index.search("hello");
        REQUIRE(res.size() == 1);
        REQUIRE(res.at(40) == 2); // Нашел два раза, проигнорировав регистр
    }
}