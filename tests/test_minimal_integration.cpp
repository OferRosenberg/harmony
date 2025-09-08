#include <gtest/gtest.h>
#include "openai_harmony/harmony.hpp"

using namespace openai_harmony;

class MinimalIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        encoding_ = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        ASSERT_NE(encoding_, nullptr);
    }
    
    void TearDown() override {}
    
    std::shared_ptr<HarmonyEncoding> encoding_;
};

TEST_F(MinimalIntegrationTest, BasicTest) {
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(Role::User, TextContent("Hello"))
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    EXPECT_FALSE(tokens.empty());
}
