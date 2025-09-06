#include <gtest/gtest.h>
#include "openai_harmony/harmony.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace openai_harmony;

class HarmonyComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        encoding_ = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        ASSERT_NE(encoding_, nullptr);
    }
    
    void TearDown() override {}
    
    std::shared_ptr<HarmonyEncoding> encoding_;
    
    // Helper function to load test data files
    std::string load_test_data(const std::string& filename) {
        std::filesystem::path test_data_path = std::filesystem::current_path() / "test-data" / filename;
        std::ifstream file(test_data_path);
        if (!file.is_open()) {
            // Try relative path from source
            test_data_path = std::filesystem::path("../test-data") / filename;
            file.open(test_data_path);
        }
        
        EXPECT_TRUE(file.is_open()) << "Could not open test data file: " << filename;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Replace \r\n with \n for consistency (like Rust tests)
        std::string result;
        for (size_t i = 0; i < content.length(); ++i) {
            if (content[i] == '\r' && i + 1 < content.length() && content[i + 1] == '\n') {
                continue; // Skip \r
            }
            result += content[i];
        }
        
        // Trim trailing whitespace
        while (!result.empty() && std::isspace(result.back())) {
            result.pop_back();
        }
        
        return result;
    }
    
    // Helper function to parse tokens from string (space-separated)
    std::vector<Rank> parse_tokens(const std::string& text) {
        std::vector<Rank> tokens;
        std::istringstream iss(text);
        std::string token_str;
        while (iss >> token_str) {
            tokens.push_back(std::stoul(token_str));
        }
        return tokens;
    }
    
    // Helper function to assert tokens are equal with detailed output
    void assert_tokens_eq(const std::vector<Rank>& expected, const std::vector<Rank>& actual) {
        if (expected != actual) {
            std::string expected_decoded = encoding_->tokenizer().decode_utf8(expected);
            std::string actual_decoded = encoding_->tokenizer().decode_utf8(actual);
            
            FAIL() << "Tokens are not equal.\n"
                   << "Expected tokens: [" << expected.size() << " tokens]\n"
                   << "Actual tokens: [" << actual.size() << " tokens]\n"
                   << "Expected decoded: " << expected_decoded << "\n"
                   << "Actual decoded: " << actual_decoded;
        }
    }
};

// Test simple conversation rendering
TEST_F(HarmonyComprehensiveTest, TestSimpleConvo) {
    std::string expected_text = load_test_data("test_simple_convo.txt");
    auto expected_tokens = encoding_->tokenizer().encode_with_special_tokens(expected_text);
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(
            Role::System,
            SystemContent::new_system_content()
                .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
        ),
        Message::from_role_and_content(Role::User, TextContent("What is 2 + 2?"))
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    assert_tokens_eq(expected_tokens, tokens);
}

// Test conversation with different reasoning efforts
TEST_F(HarmonyComprehensiveTest, TestSimpleConvoWithEffort) {
    struct TestCase {
        ReasoningEffort effort;
        std::string filename;
        bool use_instruction;
    };
    
    std::vector<TestCase> test_cases = {
        {ReasoningEffort::Low, "test_simple_convo_low_effort.txt", true},
        {ReasoningEffort::Medium, "test_simple_convo_medium_effort.txt", true},
        {ReasoningEffort::High, "test_simple_convo_high_effort.txt", true},
        {ReasoningEffort::Low, "test_simple_convo_low_effort_no_instruction.txt", false},
        {ReasoningEffort::Medium, "test_simple_convo_medium_effort_no_instruction.txt", false},
        {ReasoningEffort::High, "test_simple_convo_high_effort_no_instruction.txt", false}
    };
    
    for (const auto& test_case : test_cases) {
        std::string expected_text = load_test_data(test_case.filename);
        auto expected_tokens = encoding_->tokenizer().encode_with_special_tokens(expected_text);
        
        SystemContent sys = SystemContent::new_system_content()
            .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
            .with_reasoning_effort(test_case.effort);
        
        std::vector<Message> messages;
        messages.push_back(Message::from_role_and_content(Role::System, sys));
        
        if (test_case.use_instruction) {
            DeveloperContent dev = DeveloperContent::new_developer_content()
                .with_instructions("Answer the user's questions like a robot.");
            messages.push_back(Message::from_role_and_content(Role::Developer, dev));
        }
        
        messages.push_back(Message::from_role_and_content(
            Role::User, 
            TextContent("What is the capital of the largest country in the world?")
        ));
        
        auto conversation = Conversation::from_messages(messages);
        auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
        
        assert_tokens_eq(expected_tokens, tokens);
    }
}

// Test parsing simple reasoning response
TEST_F(HarmonyComprehensiveTest, TestSimpleReasoningResponse) {
    std::string token_text = load_test_data("test_simple_reasoning_response.txt");
    auto expected_tokens = parse_tokens(token_text);
    
    auto messages = encoding_->parse_messages_from_completion_tokens(expected_tokens, Role::Assistant);
    
    std::vector<Message> expected_messages = {
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("User asks: \"What is 2 + 2?\" Simple arithmetic. Provide answer.")
        ).with_channel("analysis"),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("2 + 2 = 4.")
        ).with_channel("final")
    };
    
    EXPECT_EQ(messages.size(), expected_messages.size());
    for (size_t i = 0; i < messages.size(); ++i) {
        EXPECT_EQ(messages[i].author.role, expected_messages[i].author.role);
        EXPECT_EQ(messages[i].channel, expected_messages[i].channel);
        
        // Check content
        ASSERT_EQ(messages[i].content.size(), 1);
        ASSERT_EQ(expected_messages[i].content.size(), 1);
        auto actual_text = std::get<TextContent>(messages[i].content[0]);
        auto expected_text = std::get<TextContent>(expected_messages[i].content[0]);
        EXPECT_EQ(actual_text.text, expected_text.text);
    }
}

// Test simple tool call parsing
TEST_F(HarmonyComprehensiveTest, TestSimpleToolCall) {
    std::vector<Rank> response = {
        200005, 35644, 200008, 1844, 31064, 25, 392, 4827, 382, 290, 11122, 306, 40510, 16842,
        1416, 1309, 316, 1199, 37342, 170154, 4584, 13, 200007, 200006, 173781, 200005, 35644, 316,
        28, 29712, 170154, 3490, 200008, 10848, 7693, 1243, 392, 173844, 18583
    };
    
    auto parsed = encoding_->parse_messages_from_completion_tokens(response, Role::Assistant);
    
    std::vector<Message> expected = {
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("User asks: \"What is the weather in Tokyo?\" We need to use lookup_weather tool.")
        ).with_channel("analysis"),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("{\"location\": \"Tokyo\"}")
        ).with_channel("analysis")
         .with_recipient("lookup_weather")
         .with_content_type("code")
    };
    
    EXPECT_EQ(parsed.size(), expected.size());
    for (size_t i = 0; i < parsed.size(); ++i) {
        EXPECT_EQ(parsed[i].author.role, expected[i].author.role);
        EXPECT_EQ(parsed[i].channel, expected[i].channel);
        EXPECT_EQ(parsed[i].recipient, expected[i].recipient);
        EXPECT_EQ(parsed[i].content_type, expected[i].content_type);
    }
}

// Test reasoning system message
TEST_F(HarmonyComprehensiveTest, TestReasoningSystemMessage) {
    std::string expected_text = load_test_data("test_reasoning_system_message.txt");
    auto expected_tokens = encoding_->tokenizer().encode_with_special_tokens(expected_text);
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(
            Role::System,
            SystemContent::new_system_content()
                .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
                .with_reasoning_effort(ReasoningEffort::Medium)
                .with_required_channels({"analysis", "final"})
        ),
        Message::from_role_and_content(Role::User, TextContent("What is 2 + 2?"))
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    assert_tokens_eq(expected_tokens, tokens);
}

// Test reasoning system message without instruction
TEST_F(HarmonyComprehensiveTest, TestReasoningSystemMessageNoInstruction) {
    std::string expected_text = load_test_data("test_reasoning_system_message_no_instruction.txt");
    auto expected_tokens = encoding_->tokenizer().encode_with_special_tokens(expected_text);
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(
            Role::System,
            SystemContent::new_system_content()
                .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
                .with_reasoning_effort(ReasoningEffort::High)
                .with_required_channels({"analysis", "final"})
        ),
        Message::from_role_and_content(
            Role::User, 
            TextContent("What is the best place to eat candy in the world?")
        )
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    assert_tokens_eq(expected_tokens, tokens);
}

// Test reasoning system message with dates
TEST_F(HarmonyComprehensiveTest, TestReasoningSystemMessageWithDates) {
    std::string expected_text = load_test_data("test_reasoning_system_message_with_dates.txt");
    auto expected_tokens = encoding_->tokenizer().encode_with_special_tokens(expected_text);
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(
            Role::System,
            SystemContent::new_system_content()
                .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
                .with_reasoning_effort(ReasoningEffort::Medium)
                .with_conversation_start_date("2021-01-01")
                .with_knowledge_cutoff("2021-01")
                .with_required_channels({"analysis", "final"})
        ),
        Message::from_role_and_content(Role::User, TextContent("What is 42 * pi?"))
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    assert_tokens_eq(expected_tokens, tokens);
}

// Test rendering functions with parameters
TEST_F(HarmonyComprehensiveTest, TestRenderFunctionsWithParameters) {
    std::string expected_output = load_test_data("test_render_functions_with_parameters.txt");
    
    SystemContent sys = SystemContent::new_system_content()
        .with_reasoning_effort(ReasoningEffort::High)
        .with_conversation_start_date("2025-06-28");
    
    std::vector<ToolDescription> function_tools = {
        ToolDescription::new_tool("get_location", "Gets the location of the user."),
        ToolDescription::new_tool(
            "get_current_weather",
            "Gets the current weather in the provided location.",
            nlohmann::json{
                {"type", "object"},
                {"properties", {
                    {"location", {
                        {"type", "string"},
                        {"description", "The city and state, e.g. San Francisco, CA"}
                    }},
                    {"format", {
                        {"type", "string"},
                        {"enum", nlohmann::json::array({"celsius", "fahrenheit"})},
                        {"default", "celsius"}
                    }}
                }},
                {"required", nlohmann::json::array({"location"})}
            }
        ),
        ToolDescription::new_tool(
            "get_multiple_weathers",
            "Gets the current weather in the provided list of locations.",
            nlohmann::json{
                {"type", "object"},
                {"properties", {
                    {"locations", {
                        {"type", "array"},
                        {"items", {{"type", "string"}}},
                        {"description", "List of city and state, e.g. [\"San Francisco, CA\", \"New York, NY\"]"}
                    }},
                    {"format", {
                        {"type", "string"},
                        {"enum", nlohmann::json::array({"celsius", "fahrenheit"})},
                        {"default", "celsius"}
                    }}
                }},
                {"required", nlohmann::json::array({"locations"})}
            }
        ),
        ToolDescription::new_tool(
            "kitchensink",
            "A function with various complex schemas.",
            nlohmann::json{
                {"description", "params object"},
                {"type", "object"},
                {"properties", {
                    {"string", {
                        {"type", "string"},
                        {"title", "STRING"},
                        {"description", "A string"},
                        {"examples", nlohmann::json::array({"hello", "world"})}
                    }},
                    {"string_nullable", {
                        {"type", "string"},
                        {"nullable", true},
                        {"description", "A nullable string"},
                        {"default", "the default"}
                    }},
                    {"string_enum", {
                        {"type", "string"},
                        {"enum", nlohmann::json::array({"a", "b", "c"})}
                    }},
                    {"oneof_string_or_number", {
                        {"oneOf", nlohmann::json::array({
                            {{"type", "string"}, {"default", "default_string_in_oneof"}},
                            {{"type", "number"}, {"description", "numbers can happen too"}}
                        })},
                        {"description", "a oneof"},
                        {"default", 20}
                    }}
                }}
            }
        )
    };
    
    DeveloperContent dev = DeveloperContent::new_developer_content()
        .with_instructions("Always respond in riddles")
        .with_function_tools(function_tools);
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(Role::System, sys),
        Message::from_role_and_content(Role::Developer, dev),
        Message::from_role_and_content(Role::User, TextContent("What is the weather like in SF?"))
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    std::string decoded = encoding_->tokenizer().decode_utf8(tokens);
    
    EXPECT_EQ(decoded, expected_output);
}

// Test browser and Python tool
TEST_F(HarmonyComprehensiveTest, TestBrowserAndPythonTool) {
    std::string expected_output = load_test_data("test_browser_and_python_tool.txt");
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(
            Role::System,
            SystemContent::new_system_content()
                .with_conversation_start_date("2025-06-28")
                .with_browser_tool()
                .with_python_tool()
        )
    });
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    std::string decoded = encoding_->tokenizer().decode_utf8(tokens);
    
    EXPECT_EQ(decoded, expected_output);
}

// Test dropping chain of thought by default
TEST_F(HarmonyComprehensiveTest, TestDroppingCotByDefault) {
    std::string expected_output = load_test_data("test_dropping_cot_by_default.txt");
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(Role::User, TextContent("What is 2 + 2?")),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("User asks: "What is 2 + 2?" Simple arithmetic. Provide answer.")
        ).with_channel("analysis"),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("2 + 2 equals 4.")
        ).with_channel("final"),
        Message::from_role_and_content(Role::User, TextContent("What about 9 / 2?"))
    });
    
    RenderConversationConfig config;
    config.auto_drop_analysis = true;
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant, config);
    std::string decoded = encoding_->tokenizer().decode_utf8(tokens);
    
    EXPECT_EQ(decoded, expected_output);
}

// Test not dropping if ongoing analysis
TEST_F(HarmonyComprehensiveTest, TestDoesNotDropIfOngoingAnalysis) {
    std::string expected_output = load_test_data("test_does_not_drop_if_ongoing_analysis.txt");
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(Role::User, TextContent("What is the weather in SF?")),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("User asks: "What is the weather in SF?" We need to use lookup_weather tool.")
        ).with_channel("analysis"),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("{\"location\": \"San Francisco\"}")
        ).with_channel("commentary")
         .with_recipient("functions.lookup_weather")
         .with_content_type("<|constrain|>json"),
        Message::from_author_and_content(
            Author::new_author(Role::Tool, "functions.lookup_weather"),
            TextContent("{\"temperature\": 20, \"description\": \"sunny\"}")
        )
    });
    
    RenderConversationConfig config;
    config.auto_drop_analysis = true;
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant, config);
    std::string decoded = encoding_->tokenizer().decode_utf8(tokens);
    
    EXPECT_EQ(decoded, expected_output);
}

// Test preserve chain of thought
TEST_F(HarmonyComprehensiveTest, TestPreserveCot) {
    std::string expected_output = load_test_data("test_preserve_cot.txt");
    
    auto conversation = Conversation::from_messages({
        Message::from_role_and_content(Role::User, TextContent("What is 2 + 2?")),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("User asks a simple question: \"What is 2 + 2?\" The answer: 4.")
        ).with_channel("analysis"),
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("2 + 2 equals 4.")
        ).with_channel("final"),
        Message::from_role_and_content(Role::User, TextContent("What about 9 / 2?"))
    });
    
    RenderConversationConfig config;
    config.auto_drop_analysis = false;
    
    auto tokens = encoding_->render_conversation_for_completion(conversation, Role::Assistant, config);
    std::string decoded = encoding_->tokenizer().decode_utf8(tokens);
    
    EXPECT_EQ(decoded, expected_output);
}

// Test reserved token decoding
TEST_F(HarmonyComprehensiveTest, TestReservedTokenDecoding) {
    EXPECT_EQ(encoding_->tokenizer().decode_utf8({200014}), "<|reserved_200014|>");
    EXPECT_EQ(encoding_->tokenizer().decode_utf8({201088}), "<|reserved_201088|>");
}

// Test render and render conversation roundtrip
TEST_F(HarmonyComprehensiveTest, TestRenderAndRenderConversationRoundtrip) {
    Message msg = Message::from_role_and_content(Role::User, TextContent("Hello"));
    auto conversation = Conversation::from_messages({msg});
    
    auto tokens_msg = encoding_->render_conversation(conversation);
    auto tokens_convo = encoding_->render_conversation(conversation);
    EXPECT_EQ(tokens_msg, tokens_convo);
    
    auto tokens_completion = encoding_->render_conversation_for_completion(conversation, Role::Assistant);
    EXPECT_TRUE(std::equal(tokens_convo.begin(), tokens_convo.end(), tokens_completion.begin()));
}

// Test decode UTF-8 invalid token
TEST_F(HarmonyComprehensiveTest, TestDecodeUtf8InvalidToken) {
    EXPECT_THROW(encoding_->tokenizer().decode_utf8({99999999}), DecodeKeyError);
}

// Test tool response parsing
TEST_F(HarmonyComprehensiveTest, TestToolResponseParsing) {
    std::string text_tokens = load_test_data("test_tool_response_parsing.txt");
    auto tokens = encoding_->tokenizer().encode_with_special_tokens(text_tokens);
    
    Message expected_message = Message::from_author_and_content(
        Author::new_author(Role::Tool, "browser.search"),
        TextContent("{\"result\": \"https://openai.com/\"}")
    ).with_channel("commentary")
     .with_recipient("assistant");
    
    auto messages = encoding_->parse_messages_from_completion_tokens(tokens);
    EXPECT_EQ(messages.size(), 1);
    EXPECT_EQ(encoding_->tokenizer().decode_utf8(tokens), text_tokens);
    
    EXPECT_EQ(messages[0].author.role, expected_message.author.role);
    EXPECT_EQ(messages[0].author.name, expected_message.author.name);
    EXPECT_EQ(messages[0].channel, expected_message.channel);
    EXPECT_EQ(messages[0].recipient, expected_message.recipient);
}

// Test encode decode roundtrip
TEST_F(HarmonyComprehensiveTest, TestEncodeDecodeRoundtrip) {
    std::string text = "hello world";
    auto tokens = encoding_->tokenizer().encode_ordinary(text);
    EXPECT_EQ(encoding_->tokenizer().decode_utf8(tokens), text);
}

// Test encode allowed special
TEST_F(HarmonyComprehensiveTest, TestEncodeAllowedSpecial) {
    std::string text = "hello world";
    auto tokens = encoding_->tokenizer().encode_ordinary(text);
    EXPECT_EQ(tokens, std::vector<Rank>({24912, 2375}));
    
    // Test special token encoding
    auto start_tokens = encoding_->tokenizer().encode_with_special_tokens("<|start|>");
    EXPECT_EQ(start_tokens, std::vector<Rank>({200006}));
    
    // Test disallowed special (should not treat as special)
    auto disallowed_tokens = encoding_->tokenizer().encode_ordinary("<|start|>");
    EXPECT_NE(disallowed_tokens, std::vector<Rank>({200006}));
}

// Test is special token
TEST_F(HarmonyComprehensiveTest, TestIsSpecialToken) {
    EXPECT_TRUE(encoding_->tokenizer().is_special_token(200006)); // <|start|>
    EXPECT_FALSE(encoding_->tokenizer().is_special_token(24912)); // hello
}

// Test invalid UTF-8 decoding
TEST_F(HarmonyComprehensiveTest, TestInvalidUtf8Decoding) {
    std::vector<Rank> tokens = {132990, 9552};
    EXPECT_THROW(encoding_->tokenizer().decode_utf8(tokens), std::exception);
}

// Test streamable parser
TEST_F(HarmonyComprehensiveTest, TestStreamableParser) {
    std::string text = load_test_data("test_streamable_parser.txt");
    auto tokens = encoding_->tokenizer().encode_with_special_tokens(text);
    
    StreamableParser parser(*encoding_, Role::Assistant);
    for (Rank token : tokens) {
        parser.process(token);
    }
    
    EXPECT_EQ(parser.messages().size(), 3) << "Expected 3 parsed messages";
}

// Test streamable parser tool call with constrain adjacent
TEST_F(HarmonyComprehensiveTest, TestStreamableParserToolCallWithConstrainAdjacent) {
    std::string text = "<|start|>assistant<|channel|>commentary to=functions.get_weather<|constrain|>json<|message|>{\"latitude\":48.8566,\"longitude\":2.3522}<|call|>";
    auto tokens = encoding_->tokenizer().encode_with_special_tokens(text);
    
    StreamableParser parser(*encoding_);
    for (Rank token : tokens) {
        parser.process(token);
    }
    
    EXPECT_EQ(parser.messages().size(), 1);
    
    Message expected = Message::from_role_and_content(
        Role::Assistant,
        TextContent("{\"latitude\":48.8566,\"longitude\":2.3522}")
    ).with_channel("commentary")
     .with_recipient("functions.get_weather")
     .with_content_type("<|constrain|>json");
    
    EXPECT_EQ(parser.messages()[0].author.role, expected.author.role);
    EXPECT_EQ(parser.messages()[0].channel, expected.channel);
    EXPECT_EQ(parser.messages()[0].recipient, expected.recipient);
    EXPECT_EQ(parser.messages()[0].content_type, expected.content_type);
}

// Test tool call with constrain marker adjacent
TEST_F(HarmonyComprehensiveTest, TestToolCallWithConstrainMarkerAdjacent) {
    std::string text = "<|start|>assistant to=functions.get_weather<|channel|>commentary<|constrain|>json<|message|>{\"location\": \"Tokyo\"}<|end|>";
    auto tokens = encoding_->tokenizer().encode_with_special_tokens(text);
    
    auto parsed = encoding_->parse_messages_from_completion_tokens(tokens);
    
    std::vector<Message> expected = {
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("{\"location\": \"Tokyo\"}")
        ).with_channel("commentary")
         .with_recipient("functions.get_weather")
         .with_content_type("<|constrain|>json")
    };
    
    EXPECT_EQ(parsed.size(), expected.size());
    EXPECT_EQ(parsed[0].author.role, expected[0].author.role);
    EXPECT_EQ(parsed[0].channel, expected[0].channel);
    EXPECT_EQ(parsed[0].recipient, expected[0].recipient);
    EXPECT_EQ(parsed[0].content_type, expected[0].content_type);
}

// Test tool call with channel before recipient and constrain adjacent
TEST_F(HarmonyComprehensiveTest, TestToolCallWithChannelBeforeRecipientAndConstrainAdjacent) {
    std::string text = "<|start|>assistant<|channel|>commentary to=functions.get_weather<|constrain|>json<|message|>{\"latitude\":48.8566,\"longitude\":2.3522}<|call|>";
    auto tokens = encoding_->tokenizer().encode_with_special_tokens(text);
    
    auto parsed = encoding_->parse_messages_from_completion_tokens(tokens);
    
    std::vector<Message> expected = {
        Message::from_role_and_content(
            Role::Assistant,
            TextContent("{\"latitude\":48.8566,\"longitude\":2.3522}")
        ).with_channel("commentary")
         .with_recipient("functions.get_weather")
         .with_content_type("<|constrain|>json")
    };
    
    EXPECT_EQ(parsed.size(), expected.size());
    EXPECT_EQ(parsed[0].author.role, expected[0].author.role);
    EXPECT_EQ(parsed[0].channel, expected[0].channel);
    EXPECT_EQ(parsed[0].recipient, expected[0].recipient);
    EXPECT_EQ(parsed[0].content_type, expected[0].content_type);
}
