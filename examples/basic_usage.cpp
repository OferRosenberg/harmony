#include <openai_harmony/harmony.hpp>
#include <iostream>
#include <vector>

using namespace openai_harmony;

void basic_conversation_example() {
    std::cout << "=== Basic Conversation Example ===" << std::endl;
    
    try {
        // Load the harmony encoding
        auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        
        // Create a simple conversation
        auto conversation = Conversation::from_messages({
            Message::from_role_and_content(
                Role::System,
                SystemContent::new_system_content()
                    .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
            ),
            Message::from_role_and_content(Role::User, TextContent("What is 2 + 2?"))
        });
        
        // Render for completion
        auto tokens = encoding->render_conversation_for_completion(conversation, Role::Assistant);
        
        // Print token count
        std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
        
        // Decode back to text to see the formatted output
        std::string formatted = encoding->tokenizer().decode_utf8(tokens);
        std::cout << "Formatted conversation:\n" << formatted << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void reasoning_example() {
    std::cout << "\n=== Reasoning Example ===" << std::endl;
    
    try {
        auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        
        // Create conversation with reasoning effort
        auto system_content = SystemContent::new_system_content()
            .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
            .with_reasoning_effort(ReasoningEffort::High)
            .with_required_channels({"analysis", "final"});
        
        auto conversation = Conversation::from_messages({
            Message::from_role_and_content(Role::System, system_content),
            Message::from_role_and_content(Role::User, TextContent("Explain quantum computing"))
        });
        
        auto tokens = encoding->render_conversation_for_completion(conversation, Role::Assistant);
        std::string formatted = encoding->tokenizer().decode_utf8(tokens);
        
        std::cout << "Reasoning conversation format:\n" << formatted << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void tools_example() {
    std::cout << "\n=== Tools Example ===" << std::endl;
    
    try {
        auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        
        // Create system content with built-in tools
        auto system_content = SystemContent::new_system_content()
            .with_reasoning_effort(ReasoningEffort::Medium)
            .with_browser_tool()
            .with_python_tool();
        
        // Create developer content with custom function tools
        auto dev_content = DeveloperContent::new_developer_content()
            .with_instructions("Always be helpful and accurate")
            .with_function_tools({
                ToolDescription::new_tool(
                    "get_weather",
                    "Gets the current weather for a location",
                    nlohmann::json{
                        {"type", "object"},
                        {"properties", {
                            {"location", {{"type", "string"}, {"description", "City and state, e.g. San Francisco, CA"}}},
                            {"format", {{"type", "string"}, {"enum", nlohmann::json::array({"celsius", "fahrenheit"})}, {"default", "celsius"}}}
                        }},
                        {"required", nlohmann::json::array({"location"})}
                    }
                ),
                ToolDescription::new_tool(
                    "calculate",
                    "Performs mathematical calculations",
                    nlohmann::json{
                        {"type", "object"},
                        {"properties", {
                            {"expression", {{"type", "string"}, {"description", "Mathematical expression to evaluate"}}}
                        }},
                        {"required", nlohmann::json::array({"expression"})}
                    }
                )
            });
        
        auto conversation = Conversation::from_messages({
            Message::from_role_and_content(Role::System, system_content),
            Message::from_role_and_content(Role::Developer, dev_content),
            Message::from_role_and_content(Role::User, TextContent("What's the weather like in New York and what's 15 * 23?"))
        });
        
        auto tokens = encoding->render_conversation_for_completion(conversation, Role::Assistant);
        std::string formatted = encoding->tokenizer().decode_utf8(tokens);
        
        std::cout << "Tools conversation format:\n" << formatted << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void streaming_example() {
    std::cout << "\n=== Streaming Parser Example ===" << std::endl;
    
    try {
        auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        
        // Simulate some assistant response tokens
        // This would typically come from a model's streaming output
        std::string sample_response = "<|start|>assistant<|channel|>analysis<|message|>The user is asking about math. Let me calculate 2+2.<|end|><|start|>assistant<|channel|>final<|message|>2 + 2 = 4<|end|>";
        auto response_tokens = encoding->tokenizer().encode_with_special_tokens(sample_response);
        
        // Create streaming parser
        StreamableParser parser(*encoding, Role::Assistant);
        
        std::cout << "Processing tokens one by one..." << std::endl;
        
        // Process tokens one by one (simulating streaming)
        for (size_t i = 0; i < response_tokens.size(); ++i) {
            parser.process(response_tokens[i]);
            
            // Show current state
            if (auto content = parser.current_content(); !content.empty()) {
                std::cout << "Current content: " << content << std::endl;
            }
            
            if (auto role = parser.current_role()) {
                std::cout << "Current role: " << role_to_string(*role) << std::endl;
            }
            
            if (auto channel = parser.current_channel()) {
                std::cout << "Current channel: " << *channel << std::endl;
            }
        }
        
        // Finalize parsing
        parser.process_eos();
        
        // Show all parsed messages
        std::cout << "\nParsed " << parser.messages().size() << " messages:" << std::endl;
        for (const auto& message : parser.messages()) {
            std::cout << "- " << role_to_string(message.author.role);
            if (message.channel) {
                std::cout << " [" << *message.channel << "]";
            }
            std::cout << ": ";
            
            // Print first text content
            for (const auto& content : message.content) {
                if (std::holds_alternative<TextContent>(content)) {
                    std::cout << std::get<TextContent>(content).text;
                    break;
                }
            }
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void parsing_example() {
    std::cout << "\n=== Parsing Example ===" << std::endl;
    
    try {
        auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        
        // Create some tokens representing an assistant response
        std::string response_text = "<|start|>assistant<|channel|>commentary to=get_weather<|constrain|>json<|message|>{\"location\": \"San Francisco, CA\"}<|call|>";
        auto tokens = encoding->tokenizer().encode_with_special_tokens(response_text);
        
        std::cout << "Parsing tokens back to messages..." << std::endl;
        std::cout << "Original text: " << response_text << std::endl;
        
        // Parse tokens back to messages
        auto messages = encoding->parse_messages_from_completion_tokens(tokens);
        
        std::cout << "Parsed " << messages.size() << " message(s):" << std::endl;
        
        for (const auto& message : messages) {
            std::cout << "Role: " << role_to_string(message.author.role) << std::endl;
            if (message.channel) {
                std::cout << "Channel: " << *message.channel << std::endl;
            }
            if (message.recipient) {
                std::cout << "Recipient: " << *message.recipient << std::endl;
            }
            if (message.content_type) {
                std::cout << "Content Type: " << *message.content_type << std::endl;
            }
            
            std::cout << "Content: ";
            for (const auto& content : message.content) {
                if (std::holds_alternative<TextContent>(content)) {
                    std::cout << std::get<TextContent>(content).text;
                }
            }
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void json_serialization_example() {
    std::cout << "\n=== JSON Serialization Example ===" << std::endl;
    
    try {
        // Create a message
        auto message = Message::from_role_and_content(
            Role::User, 
            TextContent("Hello, world!")
        ).with_channel("final");
        
        // Serialize to JSON
        nlohmann::json j = message;
        std::cout << "Message as JSON:\n" << j.dump(2) << std::endl;
        
        // Deserialize from JSON
        Message restored_message(Author(Role::User), {});
        from_json(j, restored_message);
        
        std::cout << "Restored message:" << std::endl;
        std::cout << "Role: " << role_to_string(restored_message.author.role) << std::endl;
        if (restored_message.channel) {
            std::cout << "Channel: " << *restored_message.channel << std::endl;
        }
        
        // Create a conversation and serialize it
        auto conversation = Conversation::from_messages({
            Message::from_role_and_content(Role::System, SystemContent::new_system_content()),
            Message::from_role_and_content(Role::User, TextContent("Test message"))
        });
        
        nlohmann::json conv_json = conversation;
        std::cout << "\nConversation as JSON:\n" << conv_json.dump(2) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


int main() {
    std::cout << "OpenAI Harmony C++ Library Examples" << std::endl;
    std::cout << "====================================" << std::endl;
    
    basic_conversation_example();
    reasoning_example();
    tools_example();
    streaming_example();
    parsing_example();
    json_serialization_example();
    
    std::cout << "\nAll examples completed!" << std::endl;
    return 0;
}
