#include <gtest/gtest.h>
#include "openai_harmony/registry.hpp"
#include "openai_harmony/tiktoken_ext.hpp"

using namespace openai_harmony;

class EncodingLoadTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test loading all available encodings (equivalent to Rust test_load_encodings)
TEST_F(EncodingLoadTest, TestLoadEncodings) {
    // Test loading HarmonyGptOss encoding
    auto harmony_encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    EXPECT_NE(harmony_encoding, nullptr);
    EXPECT_NO_THROW(harmony_encoding->tokenizer());
    
    // Test that the tokenizer works
    auto& tokenizer = harmony_encoding->tokenizer();
    
    // Test basic encoding/decoding
    std::string test_text = "Hello, world!";
    auto tokens = tokenizer.encode_ordinary(test_text);
    EXPECT_FALSE(tokens.empty());
    
    std::string decoded = tokenizer.decode_utf8(tokens);
    EXPECT_EQ(decoded, test_text);
    
    // Test special tokens
    EXPECT_TRUE(tokenizer.is_special_token(200006)); // <|start|>
    EXPECT_TRUE(tokenizer.is_special_token(200007)); // <|end|>
    EXPECT_TRUE(tokenizer.is_special_token(200008)); // <|message|>
    
    // Test reserved tokens
    EXPECT_EQ(tokenizer.decode_utf8({200014}), "<|reserved_200014|>");
    EXPECT_EQ(tokenizer.decode_utf8({201088}), "<|reserved_201088|>");
}

// Test encoding with special tokens
TEST_F(EncodingLoadTest, TestSpecialTokenEncoding) {
    auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    ASSERT_NE(encoding, nullptr);
    
    auto& tokenizer = encoding->tokenizer();
    
    // Test encoding with special tokens
    std::string harmony_text = "<|start|>user<|message|>Hello<|end|>";
    auto tokens = tokenizer.encode_with_special_tokens(harmony_text);
    
    // Should contain special tokens
    EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), 200006) != tokens.end()); // <|start|>
    EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), 200007) != tokens.end()); // <|end|>
    EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), 200008) != tokens.end()); // <|message|>
    
    // Decode back should match
    std::string decoded = tokenizer.decode_utf8(tokens);
    EXPECT_EQ(decoded, harmony_text);
}

// Test error handling for invalid tokens
TEST_F(EncodingLoadTest, TestInvalidTokenHandling) {
    auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    ASSERT_NE(encoding, nullptr);
    
    auto& tokenizer = encoding->tokenizer();
    
    // Test invalid token decoding
    std::vector<Rank> invalid_tokens = {999999999};
    EXPECT_THROW(tokenizer.decode_utf8(invalid_tokens), DecodeKeyError);
}

// Test tokenizer consistency
TEST_F(EncodingLoadTest, TestTokenizerConsistency) {
    auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    ASSERT_NE(encoding, nullptr);
    
    auto& tokenizer = encoding->tokenizer();
    
    // Test that multiple encodings of the same text produce the same result
    std::string text = "This is a test message with various tokens.";
    
    auto tokens1 = tokenizer.encode_ordinary(text);
    auto tokens2 = tokenizer.encode_ordinary(text);
    
    EXPECT_EQ(tokens1, tokens2);
    
    // Test that decoding is consistent
    std::string decoded1 = tokenizer.decode_utf8(tokens1);
    std::string decoded2 = tokenizer.decode_utf8(tokens2);
    
    EXPECT_EQ(decoded1, decoded2);
    EXPECT_EQ(decoded1, text);
}

// Test special token identification
TEST_F(EncodingLoadTest, TestSpecialTokenIdentification) {
    auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    ASSERT_NE(encoding, nullptr);
    
    auto& tokenizer = encoding->tokenizer();
    
    // Test known special tokens
    std::vector<Rank> special_tokens = {
        200006, // <|start|>
        200007, // <|end|>
        200008, // <|message|>
        200005, // <|channel|>
        200003, // <|constrain|>
        200012, // <|call|>
    };
    
    for (Rank token : special_tokens) {
        EXPECT_TRUE(tokenizer.is_special_token(token)) 
            << "Token " << token << " should be identified as special";
    }
    
    // Test regular tokens (should not be special)
    std::vector<Rank> regular_tokens = {
        24912, // "hello"
        2375,  // " world"
        1234,  // some other regular token
    };
    
    for (Rank token : regular_tokens) {
        EXPECT_FALSE(tokenizer.is_special_token(token))
            << "Token " << token << " should not be identified as special";
    }
}

// Test reserved token range
TEST_F(EncodingLoadTest, TestReservedTokenRange) {
    auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    ASSERT_NE(encoding, nullptr);
    
    auto& tokenizer = encoding->tokenizer();
    
    // Test a few reserved tokens in the range
    std::vector<Rank> reserved_tokens = {200014, 200100, 200500, 201000, 201088};
    
    for (Rank token : reserved_tokens) {
        std::string decoded = tokenizer.decode_utf8({token});
        std::string expected = "<|reserved_" + std::to_string(token) + "|>";
        EXPECT_EQ(decoded, expected);
    }
}

// Test encoding performance (basic benchmark)
TEST_F(EncodingLoadTest, TestEncodingPerformance) {
    auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
    ASSERT_NE(encoding, nullptr);
    
    auto& tokenizer = encoding->tokenizer();
    
    // Create a moderately large text
    std::string large_text;
    for (int i = 0; i < 1000; ++i) {
        large_text += "This is sentence number " + std::to_string(i) + ". ";
    }
    
    // Time the encoding
    auto start = std::chrono::high_resolution_clock::now();
    auto tokens = tokenizer.encode_ordinary(large_text);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete in reasonable time (less than 1 second)
    EXPECT_LT(duration.count(), 1000);
    EXPECT_FALSE(tokens.empty());
    
    // Test decoding performance
    start = std::chrono::high_resolution_clock::now();
    std::string decoded = tokenizer.decode_utf8(tokens);
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
    EXPECT_EQ(decoded, large_text);
}
