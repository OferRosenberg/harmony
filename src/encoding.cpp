#include "openai_harmony/encoding.hpp"
#include "openai_harmony/utils.hpp"
#include <sstream>
#include <algorithm>
#include <regex>

namespace openai_harmony {

// HarmonyEncoding implementation
HarmonyEncoding::HarmonyEncoding(const std::string& name,
                               size_t n_ctx,
                               size_t max_message_tokens,
                               size_t max_action_length,
                               const std::string& tokenizer_name,
                               std::shared_ptr<CoreBPE> tokenizer,
                               const std::unordered_map<FormattingToken, std::string>& format_token_mapping,
                               const std::unordered_set<FormattingToken>& stop_formatting_tokens,
                               const std::unordered_set<FormattingToken>& stop_formatting_tokens_for_assistant_actions)
    : name_(name),
      n_ctx_(n_ctx),
      max_message_tokens_(max_message_tokens),
      max_action_length_(max_action_length),
      tokenizer_name_(tokenizer_name),
      tokenizer_(std::move(tokenizer)),
      format_token_mapping_(format_token_mapping),
      stop_formatting_tokens_(stop_formatting_tokens),
      stop_formatting_tokens_for_assistant_actions_(stop_formatting_tokens_for_assistant_actions) {
}

std::vector<Rank> HarmonyEncoding::render_conversation_for_completion(
    const Conversation& conversation, 
    Role next_turn_role,
    const std::optional<RenderConversationConfig>& config) const {
    
    std::vector<Rank> tokens;
    
    // Render each message in the conversation
    for (const auto& message : conversation.messages) {
        auto message_tokens = render_message(message, config);
        tokens.insert(tokens.end(), message_tokens.begin(), message_tokens.end());
    }
    
    // Add start token for next turn
    tokens.push_back(get_special_token("<|start|>"));
    tokens.push_back(get_special_token(role_to_special_token(next_turn_role)));
    
    return tokens;
}

std::vector<Rank> HarmonyEncoding::render_conversation(
    const Conversation& conversation,
    const std::optional<RenderConversationConfig>& config) const {
    
    std::vector<Rank> tokens;
    
    for (const auto& message : conversation.messages) {
        auto message_tokens = render_message(message, config);
        tokens.insert(tokens.end(), message_tokens.begin(), message_tokens.end());
    }
    
    return tokens;
}

std::vector<Message> HarmonyEncoding::parse_messages_from_completion_tokens(
    const std::vector<Rank>& tokens,
    const std::optional<Role>& /* role */) const {
    
    std::vector<Message> messages;
    
    // Convert tokens back to text first
    std::string text;
    try {
        text = tokenizer_->decode_utf8(tokens);
    } catch (const std::exception&) {
        return messages;
    }
    
    // Parse the harmony format
    messages = parse_harmony_format(text);
    
    return messages;
}

// Stub implementations for missing methods
std::vector<Rank> HarmonyEncoding::render_conversation_for_training(
    const Conversation& conversation,
    const std::optional<RenderConversationConfig>& config) const {
    // For now, same as regular render
    return render_conversation(conversation, config);
}

std::vector<Rank> HarmonyEncoding::render(
    const Message& message,
    const std::optional<RenderOptions>& render_options) const {
    std::vector<Rank> tokens;
    render_into(message, tokens, render_options);
    return tokens;
}

void HarmonyEncoding::render_into(
    const Message& message,
    std::vector<Rank>& into,
    const std::optional<RenderOptions>& /* render_options */) const {
    
    // Simplified rendering - just encode the message content
    for (const auto& content : message.content) {
        if (std::holds_alternative<TextContent>(content)) {
            const auto& text_content = std::get<TextContent>(content);
            auto tokens = tokenizer_->encode_ordinary(text_content.text);
            into.insert(into.end(), tokens.begin(), tokens.end());
        }
    }
}

void HarmonyEncoding::render_conversation_into(
    const Conversation& conversation,
    std::vector<Rank>& into,
    const std::optional<RenderConversationConfig>& config) const {
    
    auto tokens = render_conversation(conversation, config);
    into.insert(into.end(), tokens.begin(), tokens.end());
}

void HarmonyEncoding::render_conversation_for_completion_into(
    const Conversation& conversation,
    Role next_turn_role,
    std::vector<Rank>& into,
    const std::optional<RenderConversationConfig>& config) const {
    
    auto tokens = render_conversation_for_completion(conversation, next_turn_role, config);
    into.insert(into.end(), tokens.begin(), tokens.end());
}

std::unordered_set<Rank> HarmonyEncoding::stop_tokens() const {
    std::unordered_set<Rank> result;
    // Add stop tokens based on formatting tokens
    return result;
}

std::unordered_set<Rank> HarmonyEncoding::stop_tokens_for_assistant_actions() const {
    std::unordered_set<Rank> result;
    // Add assistant action stop tokens
    return result;
}

// Missing methods that are called in the implementation
std::vector<Rank> HarmonyEncoding::render_message(
    const Message& message, 
    const std::optional<RenderConversationConfig>& /* config */) const {
    
    std::vector<Rank> tokens;
    
    // Start message with proper harmony format
    tokens.push_back(get_special_token("<|start|>"));
    tokens.push_back(get_special_token(role_to_special_token(message.author.role)));
    
    // Add channel if present
    if (message.channel) {
        tokens.push_back(get_special_token("<|channel|>"));
        auto channel_tokens = tokenizer_->encode_ordinary(*message.channel);
        tokens.insert(tokens.end(), channel_tokens.begin(), channel_tokens.end());
    }
    
    // Add recipient if present
    if (message.recipient) {
        std::string recipient_marker = " to=" + *message.recipient;
        auto recipient_tokens = tokenizer_->encode_ordinary(recipient_marker);
        tokens.insert(tokens.end(), recipient_tokens.begin(), recipient_tokens.end());
    }
    
    // Add content type constraint if present
    if (message.content_type) {
        tokens.push_back(get_special_token("<|constrain|>"));
        auto content_type_tokens = tokenizer_->encode_ordinary(*message.content_type);
        tokens.insert(tokens.end(), content_type_tokens.begin(), content_type_tokens.end());
    }
    
    // Add message content
    tokens.push_back(get_special_token("<|message|>"));
    
    for (const auto& content : message.content) {
        if (std::holds_alternative<TextContent>(content)) {
            const auto& text_content = std::get<TextContent>(content);
            auto content_tokens = tokenizer_->encode_ordinary(text_content.text);
            tokens.insert(tokens.end(), content_tokens.begin(), content_tokens.end());
        } else if (std::holds_alternative<SystemContent>(content)) {
            const auto& system_content = std::get<SystemContent>(content);
            std::stringstream ss;
            
            if (system_content.model_identity) {
                ss << "Model: " << *system_content.model_identity << "\n";
            }
            
            if (system_content.reasoning_effort) {
                ss << "Reasoning effort: " << reasoning_effort_to_string(*system_content.reasoning_effort) << "\n";
            }
            
            if (system_content.knowledge_cutoff) {
                ss << "Knowledge cutoff: " << *system_content.knowledge_cutoff << "\n";
            }
            
            if (system_content.conversation_start_date) {
                ss << "Current date: " << *system_content.conversation_start_date << "\n";
            }
            
            if (system_content.tools) {
                ss << "\nAvailable tools:\n";
                for (const auto& [namespace_name, ns_config] : *system_content.tools) {
                    ss << "# " << ns_config.name;
                    if (ns_config.description) {
                        ss << "\n" << *ns_config.description;
                    }
                    ss << "\n";
                    
                    for (const auto& tool : ns_config.tools) {
                        ss << "## " << tool.name << "\n";
                        ss << tool.description << "\n";
                        if (tool.parameters) {
                            ss << "Parameters: " << tool.parameters->dump() << "\n";
                        }
                        ss << "\n";
                    }
                }
            }
            
            if (system_content.channel_config) {
                const auto& channel_config = *system_content.channel_config;
                if (channel_config.channel_required && !channel_config.valid_channels.empty()) {
                    ss << "\nRequired channels: ";
                    for (size_t i = 0; i < channel_config.valid_channels.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << channel_config.valid_channels[i];
                    }
                    ss << "\n";
                }
            }
            
            auto system_tokens = tokenizer_->encode_ordinary(ss.str());
            tokens.insert(tokens.end(), system_tokens.begin(), system_tokens.end());
        } else if (std::holds_alternative<DeveloperContent>(content)) {
            const auto& dev_content = std::get<DeveloperContent>(content);
            std::stringstream ss;
            
            if (dev_content.instructions) {
                ss << *dev_content.instructions << "\n";
            }
            
            if (dev_content.tools) {
                ss << "\nDeveloper tools:\n";
                for (const auto& [namespace_name, ns_config] : *dev_content.tools) {
                    ss << "# " << ns_config.name;
                    if (ns_config.description) {
                        ss << "\n" << *ns_config.description;
                    }
                    ss << "\n";
                    
                    for (const auto& tool : ns_config.tools) {
                        ss << "## " << tool.name << "\n";
                        ss << tool.description << "\n";
                        if (tool.parameters) {
                            ss << "Parameters: " << tool.parameters->dump() << "\n";
                        }
                        ss << "\n";
                    }
                }
            }
            
            auto dev_tokens = tokenizer_->encode_ordinary(ss.str());
            tokens.insert(tokens.end(), dev_tokens.begin(), dev_tokens.end());
        }
    }
    
    // End message
    tokens.push_back(get_special_token("<|end|>"));
    
    return tokens;
}

Rank HarmonyEncoding::get_special_token(const std::string& token) const {
    auto special_tokens = tokenizer_->special_tokens();
    if (special_tokens.count(token) > 0) {
        // Find the rank for this special token
        auto tokens = tokenizer_->encode_with_special_tokens(token);
        if (!tokens.empty()) {
            return tokens[0];
        }
    }
    throw std::runtime_error("Special token not found: " + token);
}

std::string HarmonyEncoding::role_to_special_token(Role role) const {
    switch (role) {
        case Role::System: return "<|system|>";
        case Role::User: return "<|user|>";
        case Role::Assistant: return "<|assistant|>";
        case Role::Developer: return "<|developer|>";
        case Role::Tool: return "<|tool|>";
        default: throw std::invalid_argument("Unknown role");
    }
}

std::vector<Message> HarmonyEncoding::parse_harmony_format(const std::string& text) const {
    std::vector<Message> messages;
    
    if (text.empty()) {
        return messages;
    }
    
    // Simple state-based parsing for harmony format
    size_t pos = 0;
    
    while (pos < text.length()) {
        // Look for start token
        size_t start_pos = text.find("<|start|>", pos);
        if (start_pos == std::string::npos) break;
        
        pos = start_pos + 9; // Skip "<|start|>"
        
        // Safety check
        if (pos >= text.length()) break;
        
        // Find role token - could be <|role|> or just role text
        Role role = Role::User; // Default
        
        if (pos + 10 <= text.length() && text.substr(pos, 10) == "<|system|>") {
            role = Role::System;
            pos += 10;
        } else if (pos + 8 <= text.length() && text.substr(pos, 8) == "<|user|>") {
            role = Role::User;
            pos += 8;
        } else if (pos + 13 <= text.length() && text.substr(pos, 13) == "<|assistant|>") {
            role = Role::Assistant;
            pos += 13;
        } else if (pos + 12 <= text.length() && text.substr(pos, 12) == "<|developer|>") {
            role = Role::Developer;
            pos += 12;
        } else if (pos + 8 <= text.length() && text.substr(pos, 8) == "<|tool|>") {
            role = Role::Tool;
            pos += 8;
        } else if (pos + 6 <= text.length() && text.substr(pos, 6) == "system") {
            role = Role::System;
            pos += 6;
        } else if (pos + 4 <= text.length() && text.substr(pos, 4) == "user") {
            role = Role::User;
            pos += 4;
        } else if (pos + 9 <= text.length() && text.substr(pos, 9) == "assistant") {
            role = Role::Assistant;
            pos += 9;
        } else if (pos + 9 <= text.length() && text.substr(pos, 9) == "developer") {
            role = Role::Developer;
            pos += 9;
        } else if (pos + 4 <= text.length() && text.substr(pos, 4) == "tool") {
            role = Role::Tool;
            pos += 4;
        } else {
            // Skip to next potential role token
            pos++;
            continue;
        }
        
        // Create message
        Message message(Author(role), {});
        
        // Parse optional components (channel, recipient, content type) with bounds checking
        while (pos < text.length()) {
            if (pos + 11 <= text.length() && text.substr(pos, 11) == "<|channel|>") {
                pos += 11;
                if (pos >= text.length()) break;
                
                size_t channel_end = text.find("<|", pos);
                size_t space_end = text.find(" ", pos);
                
                // Channel ends at the first of: next token, space, or end of string
                size_t actual_end = channel_end;
                if (space_end != std::string::npos && (actual_end == std::string::npos || space_end < actual_end)) {
                    actual_end = space_end;
                }
                if (actual_end == std::string::npos) {
                    actual_end = text.length();
                }
                
                if (actual_end > pos) {
                    message.channel = text.substr(pos, actual_end - pos);
                }
                pos = actual_end;
            } else if (pos + 12 <= text.length() && text.substr(pos, 12) == "<|constrain|>") {
                pos += 12;
                if (pos >= text.length()) break;
                
                size_t constrain_end = text.find("<|", pos);
                if (constrain_end != std::string::npos && constrain_end > pos) {
                    message.content_type = text.substr(pos, constrain_end - pos);
                    pos = constrain_end;
                }
            } else if (pos + 11 <= text.length() && text.substr(pos, 11) == "<|message|>") {
                pos += 11;
                break;
            } else if (pos + 4 <= text.length() && text.substr(pos, 4) == " to=") {
                pos += 4;
                if (pos >= text.length()) break;
                
                size_t recipient_end = text.find("<|", pos);
                if (recipient_end != std::string::npos && recipient_end > pos) {
                    message.recipient = text.substr(pos, recipient_end - pos);
                    pos = recipient_end;
                } else {
                    // Look for space or end of string
                    size_t space_pos = text.find(" ", pos);
                    size_t end_pos = (space_pos != std::string::npos) ? space_pos : text.length();
                    if (end_pos > pos) {
                        message.recipient = text.substr(pos, end_pos - pos);
                    }
                    pos = end_pos;
                }
            } else {
                pos++;
            }
        }
        
        // Find message content
        size_t content_start = pos;
        size_t content_end = text.find("<|end|>", content_start);
        if (content_end == std::string::npos) {
            // No end token, take rest of string
            content_end = text.length();
        }
        
        std::string content_text = text.substr(content_start, content_end - content_start);
        message.content.push_back(TextContent(content_text));
        
        messages.push_back(message);
        
        // Move past the end token
        pos = content_end + 7; // Skip "<|end|>"
    }
    
    return messages;
}

// StreamableParser implementation
StreamableParser::StreamableParser(const HarmonyEncoding& encoding, const std::optional<Role>& role)
    : encoding_(encoding), next_role_(role), state_(StreamState::ExpectStart) {
}

StreamableParser& StreamableParser::process(Rank token) {
    // Store token
    tokens_.push_back(token);
    
    // Decode this single token
    std::string token_text;
    try {
        token_text = encoding_.tokenizer().decode_utf8({token});
    } catch (const std::exception&) {
        // Skip invalid tokens
        return *this;
    }

    switch (state_) {
        case StreamState::ExpectStart:
            if (token_text == "<|start|>") {
                state_ = StreamState::Header;
            }
            break;
            
        case StreamState::Header: {
            auto role_opt = parse_role_from_token(token_text);
            if (role_opt) {
                current_role_ = *role_opt;
            } else if (token_text == "<|channel|>") {
                state_ = StreamState::Channel;
            } else if (token_text == "<|message|>") {
                state_ = StreamState::Content;
            } else {
                // Accumulate role name from individual character tokens
                if (!current_role_) {
                    // Build up role name character by character
                    std::string accumulated_role;
                    if (current_channel_) {
                        accumulated_role = *current_channel_;
                    }
                    accumulated_role += token_text;
                    
                    // Check if we have a complete role name
                    if (accumulated_role == "assistant") {
                        current_role_ = Role::Assistant;
                        current_channel_.reset(); // Clear temporary storage
                    } else if (accumulated_role == "user") {
                        current_role_ = Role::User;
                        current_channel_.reset();
                    } else if (accumulated_role == "system") {
                        current_role_ = Role::System;
                        current_channel_.reset();
                    } else if (accumulated_role == "developer") {
                        current_role_ = Role::Developer;
                        current_channel_.reset();
                    } else if (accumulated_role == "tool") {
                        current_role_ = Role::Tool;
                        current_channel_.reset();
                    } else {
                        // Store partial role name temporarily in current_channel_
                        current_channel_ = accumulated_role;
                    }
                }
            }
            break;
        }
        
        case StreamState::Channel:
            if (token_text == "<|message|>") {
                state_ = StreamState::Content;
            } else {
                // Accumulate channel name (could be multiple tokens)
                if (current_channel_) {
                    *current_channel_ += token_text;
                } else {
                    current_channel_ = token_text;
                }
            }
            break;

        case StreamState::Content:
            if (token_text == "<|end|>") {
                finalize_current_message();
                state_ = StreamState::ExpectStart;
            } else {
                // Regular content
                current_content_ += token_text;
                last_content_delta_ = token_text;
            }
            break;
    }
    
    return *this;
}

StreamableParser& StreamableParser::process_eos() {
    if (state_ == StreamState::Content && !current_content_.empty()) {
        finalize_current_message();
    }
    return *this;
}

std::string StreamableParser::current_content() const {
    return current_content_;
}

std::optional<Role> StreamableParser::current_role() const {
    return current_role_;
}

std::optional<std::string> StreamableParser::current_channel() const {
    return current_channel_;
}

std::optional<std::string> StreamableParser::current_recipient() const {
    return current_recipient_;
}

std::optional<std::string> StreamableParser::current_content_type() const {
    return current_content_type_;
}

std::optional<std::string> StreamableParser::last_content_delta() const {
    return last_content_delta_;
}

nlohmann::json StreamableParser::state_json() const {
    nlohmann::json j;
    j["state"] = static_cast<int>(state_);
    j["tokens"] = tokens_;
    j["messages"] = messages_;
    if (current_role_) {
        j["current_role"] = *current_role_;
    }
    if (current_channel_) {
        j["current_channel"] = *current_channel_;
    }
    if (current_recipient_) {
        j["current_recipient"] = *current_recipient_;
    }
    if (current_content_type_) {
        j["current_content_type"] = *current_content_type_;
    }
    j["current_content"] = current_content_;
    return j;
}

// Private helper methods for StreamableParser
std::optional<Role> StreamableParser::parse_role_from_token(const std::string& token) const {
    if (token == "<|system|>") return Role::System;
    if (token == "<|user|>") return Role::User;
    if (token == "<|assistant|>") return Role::Assistant;
    if (token == "<|developer|>") return Role::Developer;
    if (token == "<|tool|>") return Role::Tool;
    return std::nullopt;
}

void StreamableParser::finalize_current_message() {
    if (current_role_) {
        Message message(Author(*current_role_), {TextContent(current_content_)});
        
        if (current_channel_) {
            message.channel = *current_channel_;
        }
        if (current_recipient_) {
            message.recipient = *current_recipient_;
        }
        if (current_content_type_) {
            message.content_type = *current_content_type_;
        }
        
        messages_.push_back(message);
        
        // Reset state
        current_content_.clear();
        current_role_.reset();
        current_channel_.reset();
        current_recipient_.reset();
        current_content_type_.reset();
    }
}

} // namespace openai_harmony
