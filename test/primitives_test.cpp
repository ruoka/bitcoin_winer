#include <gtest/gtest.h>
#include <iostream>
#include "bitcoin/primitives.hpp"
#include "cryptic/sha2.hpp"

using namespace std::string_literals;

TEST(BitcoinPrimitives,VariableLengthInteger)
{
    auto l1 = bitcoin::variable_length_integer{1};
    EXPECT_EQ(1, static_cast<std::size_t>(l1));
    EXPECT_EQ(1, l1.uint8);
    EXPECT_EQ(0, l1.uint16);
    EXPECT_EQ(0, l1.uint32);
    EXPECT_EQ(0, l1.uint64);

    auto l2 = bitcoin::variable_length_integer{4321};
    EXPECT_EQ(4321, static_cast<std::size_t>(l2));
    EXPECT_EQ(0xFDu, l2.uint8);
    EXPECT_EQ(4321, l2.uint16);
    EXPECT_NE(4321, l2.uint32);
    EXPECT_NE(4321, l2.uint64);

    auto l3 = bitcoin::variable_length_integer{987654321};
    EXPECT_EQ(987654321, static_cast<std::size_t>(l3));
    EXPECT_EQ(0xFEu, l3.uint8);
    EXPECT_NE(987654321, l3.uint16);
    EXPECT_EQ(987654321, l3.uint32);
    EXPECT_NE(987654321, l3.uint64);

    auto l4 = bitcoin::variable_length_integer{987654321987654321};
    EXPECT_EQ(987654321987654321, static_cast<std::size_t>(l4));
    EXPECT_EQ(0xFFu, l4.uint8);
    EXPECT_NE(987654321987654321, l4.uint16);
    EXPECT_NE(987654321987654321, l4.uint32);
    EXPECT_EQ(987654321987654321, l4.uint64);
}

TEST(BitcoinPrimitives,Hash)
{
    auto sha256 = cryptic::sha256{};
    auto hash1 = bitcoin::hash{};
    auto hash2 = bitcoin::hash{};

    sha256.hash("test test test test test test test test"s);
    sha256.encode(hash1.bytes);
    hash2 = hash1;

    EXPECT_EQ(sha256.base64(), cryptic::base64::encode(gsl::make_span(hash1.bytes)));
    EXPECT_EQ(cryptic::base64::encode(gsl::make_span(hash1.bytes)), cryptic::base64::encode(gsl::make_span(hash2.bytes)));
    EXPECT_EQ(sha256.base64(), cryptic::base64::encode(gsl::make_span(hash2.bytes)));
}
