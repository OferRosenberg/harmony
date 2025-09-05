#include <openai_harmony/harmony.hpp>
#include <iostream>
#include <string>

using namespace openai_harmony;

int main() {
    std::cout << "OpenAI Harmony Interactive Example" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Enter your message (or 'quit' to exit): ";
    
    try {
        auto encoding = load_harmony_encoding(HarmonyEncodingName::HarmonyGptOss);
        
        std::string user_input;
        while (std::getline(std::cin, user_input)) {
            if (user_input == "quit" || user_input == "exit") {
                break;
            }
            
            if (user_input.empty()) {
                std::cout << "Enter your message (or 'quit' to exit): ";
                continue;
            }
            
            // Create a conversation with system message and user input
            auto conversation = Conversation::from_messages({
                Message::from_role_and_content(
                    Role::System,
                    SystemContent::new_system_content()
                        .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
                        .with_reasoning_effort(ReasoningEffort::Medium)
                        .with_required_channels({"analysis", "final"})
                ),
                Message::from_role_and_content(Role::User, TextContent(user_input))
            });
            
            // Render for completion
            auto tokens = encoding->render_conversation_for_completion(conversation, Role::Assistant);
            
            // Decode back to text to see the formatted output
            std::string formatted = encoding->tokenizer().decode_utf8(tokens);
            
            std::cout << "\n--- Rendered Harmony Format ---" << std::endl;
            std::cout << formatted << std::endl;
            std::cout << "\n--- Token Count: " << tokens.size() << " ---" << std::endl;
            
            // Show individual tokens for debugging
            std::cout << "\n--- Individual Tokens ---" << std::endl;
            for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); ++i) {
                std::string token_text = encoding->tokenizer().decode_utf8({tokens[i]});
                std::cout << "Token " << i << ": [" << tokens[i] << "] = \"" << token_text << "\"" << std::endl;
            }
            if (tokens.size() > 10) {
                std::cout << "... and " << (tokens.size() - 10) << " more tokens" << std::endl;
            }
            
            // Demonstrate round-trip: Parse the tokens back using StreamableParser
            std::cout << "\n--- Round-trip Parsing with StreamableParser ---" << std::endl;
            
            try {
                StreamableParser parser(*encoding, Role::Assistant);
                
                // Process each token through the parser
                for (Rank token : tokens) {
                    parser.process(token);
                }
                
                // Finalize parsing
                parser.process_eos();
                
                // Display parser results
                std::cout << "Parser State: ";
                switch (parser.state()) {
                    case StreamState::ExpectStart: std::cout << "ExpectStart"; break;
                    case StreamState::Header: std::cout << "Header"; break;
                    case StreamState::Content: std::cout << "Content"; break;
                }
                std::cout << std::endl;
                
                if (parser.current_role()) {
                    std::cout << "Current Role: " << role_to_string(*parser.current_role()) << std::endl;
                }
                
                if (parser.current_channel()) {
                    std::cout << "Current Channel: " << *parser.current_channel() << std::endl;
                }
                
                if (parser.current_recipient()) {
                    std::cout << "Current Recipient: " << *parser.current_recipient() << std::endl;
                }
                
                if (parser.current_content_type()) {
                    std::cout << "Current Content Type: " << *parser.current_content_type() << std::endl;
                }
                
                std::cout << "Current Content: \"" << parser.current_content() << "\"" << std::endl;
                
                // Show parsed messages
                const auto& parsed_messages = parser.messages();
                std::cout << "Parsed Messages Count: " << parsed_messages.size() << std::endl;
                
                for (size_t i = 0; i < parsed_messages.size(); ++i) {
                    const auto& msg = parsed_messages[i];
                    std::cout << "Message " << i << ":" << std::endl;
                    std::cout << "  Role: " << role_to_string(msg.author.role) << std::endl;
                    
                    if (msg.author.name) {
                        std::cout << "  Name: " << *msg.author.name << std::endl;
                    }
                    
                    if (msg.channel) {
                        std::cout << "  Channel: " << *msg.channel << std::endl;
                    }
                    
                    if (msg.recipient) {
                        std::cout << "  Recipient: " << *msg.recipient << std::endl;
                    }
                    
                    if (msg.content_type) {
                        std::cout << "  Content Type: " << *msg.content_type << std::endl;
                    }
                    
                    std::cout << "  Content: ";
                    for (const auto& content : msg.content) {
                        if (std::holds_alternative<TextContent>(content)) {
                            const auto& text_content = std::get<TextContent>(content);
                            std::cout << "\"" << text_content.text << "\" ";
                        } else if (std::holds_alternative<SystemContent>(content)) {
                            std::cout << "[SystemContent] ";
                        } else if (std::holds_alternative<DeveloperContent>(content)) {
                            std::cout << "[DeveloperContent] ";
                        }
                    }
                    std::cout << std::endl;
                }
                
                // Test alternative parsing method
                std::cout << "\n--- Alternative: Direct Token Parsing ---" << std::endl;
                auto parsed_messages_direct = encoding->parse_messages_from_completion_tokens(tokens, Role::Assistant);
                std::cout << "Direct Parsed Messages Count: " << parsed_messages_direct.size() << std::endl;
                
                for (size_t i = 0; i < parsed_messages_direct.size(); ++i) {
                    const auto& msg = parsed_messages_direct[i];
                    std::cout << "Direct Message " << i << ": " << role_to_string(msg.author.role);
                    if (!msg.content.empty() && std::holds_alternative<TextContent>(msg.content[0])) {
                        const auto& text_content = std::get<TextContent>(msg.content[0]);
                        std::cout << " - \"" << text_content.text.substr(0, 50);
                        if (text_content.text.length() > 50) std::cout << "...";
                        std::cout << "\"";
                    }
                    std::cout << std::endl;
                }
                
            } catch (const std::exception& parse_error) {
                std::cerr << "Parsing error: " << parse_error.what() << std::endl;
            }
            
            std::cout << "\nEnter your message (or 'quit' to exit): ";
        }
        
        std::cout << "Interactive example finished." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
