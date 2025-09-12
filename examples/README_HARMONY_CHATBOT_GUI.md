# OpenAI Harmony Chatbot GUI for Windows

A Windows GUI application that demonstrates the OpenAI Harmony format with interactive user prompts.

## 🎯 **What This App Does**

The Harmony Chatbot GUI provides a visual interface to:

1. **Accept user prompts** in a text input window
2. **Compose harmony messages** using the prompt and system components
3. **Render to tokens** using the Harmony format renderer
4. **Display first 20 tokens** in a results window
5. **Parse tokens back** to original message components
6. **Show parsed components** in the GUI
7. **Return to prompt window** for the next interaction

## 🚀 **Quick Start**

### **Prerequisites**
Make sure you have completed the setup:
```cmd
# 1. Download vocabulary files
download_vocab_files_rust_windows.bat

# 2. Install Python dependencies
install_python_dependencies_windows.bat

# 3. Build Python bindings (if not done already)
build_python_rust_windows.bat
```

### **Run the Chatbot GUI**
```cmd
# Easy way - use the launcher
run_harmony_chatbot_gui.bat

# Or manually
python harmony_chatbot_gui.py
```

## 🖥️ **GUI Features**

### **Main Window**
- **Title**: "🤖 OpenAI Harmony Chatbot"
- **Prompt Input**: Large text area for entering user prompts
- **Default Prompt**: Pre-loaded with the mathematical problem from `input-prompt.txt`
- **Process Button**: "🚀 Process with Harmony Format"
- **Clear Button**: "🗑️ Clear" to reset the prompt
- **Status Bar**: Shows current processing status

### **Results Window (Modal)**
Opens after processing with three tabs:

#### **Tab 1: 🔢 First 20 Tokens**
- Shows total token count
- Displays first 20 tokens with their numeric values
- Shows decoded text representation for each token
- Format: `Token 1: 123456 → "Hello"`

#### **Tab 2: 📝 Parsed Messages**
- Shows number of messages parsed from tokens
- Lists each message with:
  - Role (system, developer, user, assistant)
  - Channel (if applicable)
  - Recipient (if applicable)
  - Content preview (first 100 characters)

#### **Tab 3: 🎨 Rendered Format**
- Complete harmony format output
- Shows the full rendered text with all special tokens
- Demonstrates the actual format sent to/from AI models

## 🔧 **How It Works**

### **1. User Input Processing**
```python
# User enters prompt in GUI
user_prompt = "Your question here..."

# App creates harmony conversation
conversation = create_harmony_conversation(user_prompt)
```

### **2. Harmony Message Composition**
Based on `harmony_format_example_corrected.py`:

```python
# System message with reasoning configuration
system_content = SystemContent.new()
    .with_model_identity("You are ChatGPT...")
    .with_knowledge_cutoff("2024-06")
    .with_reasoning_effort(ReasoningEffort.MEDIUM)
    .with_required_channels(["analysis", "commentary", "final"])

# Developer message with instructions (no tools)
developer_content = DeveloperContent.new()
    .with_instructions("You are a helpful assistant...")

# User message from GUI input
user_message = Message.from_role_and_content(Role.USER, user_prompt)

# Assistant response (simulated)
assistant_message = Message.from_role_and_content(
    Role.ASSISTANT, 
    "I'll analyze this step by step..."
).with_channel("final")
```

### **3. Token Rendering**
```python
# Render conversation to tokens
tokens = encoding.render_conversation(conversation)

# Show first 20 tokens in GUI
first_20_tokens = tokens[:20]
```

### **4. Token Parsing**
```python
# Parse tokens back to messages
parsed_messages = encoding.parse_messages_from_completion_tokens(tokens, None)

# Display in GUI with role, channel, content info
```

## 📋 **Example Workflow**

1. **Start the app**: `run_harmony_chatbot_gui.bat`
2. **Enter prompt**: Type your question (default math problem provided)
3. **Click Process**: "🚀 Process with Harmony Format"
4. **View results**: Results window opens with three tabs
5. **Examine tokens**: See first 20 tokens and their decoded values
6. **Check parsing**: View how tokens are parsed back to messages
7. **See format**: View complete harmony format output
8. **Close results**: Click "✅ Close Results"
9. **Next prompt**: Enter new prompt and repeat

## 🎨 **GUI Design**

### **Color Scheme**
- **Background**: Light gray (`#f0f0f0`)
- **Primary**: Blue (`#3498db`)
- **Success**: Green (`#27ae60`)
- **Warning**: Orange (`#f39c12`)
- **Error**: Red (`#e74c3c`)
- **Text**: Dark blue (`#2c3e50`)

### **Fonts**
- **Titles**: Arial Bold
- **Code/Tokens**: Consolas (monospace)
- **UI Text**: Arial

### **Layout**
- **Main window**: 800x600 pixels
- **Results window**: 1000x700 pixels (modal)
- **Responsive**: Expands with window resizing
- **Tabbed interface**: Easy navigation between result views

## 🔍 **Technical Details**

### **Threading**
- Uses background threads for processing to avoid UI freezing
- Thread-safe UI updates with `root.after()`
- Proper error handling in background threads

### **Error Handling**
- Checks for vocabulary files on startup
- Validates harmony encoding loading
- Shows user-friendly error messages
- Graceful degradation on failures

### **Memory Management**
- Efficient token handling for large conversations
- Proper cleanup of GUI resources
- Modal windows for focused interaction

## 📊 **Example Output**

### **Sample Tokens (First 20)**
```
Token  1: 100257 → "<|start|>"
Token  2: 9125   → "system"
Token  3: 100264 → "<|message|>"
Token  4: 2675   → "You"
Token  5: 527    → " are"
...
```

### **Sample Parsed Messages**
```
Message 1: system
Content: You are ChatGPT, a large language model trained by OpenAI. Knowledge cutoff: 2024-06...

Message 2: developer  
Content: You are a helpful assistant that can answer questions and help with tasks...

Message 3: user
Content: Find the sum of all integer bases b>9 for which 17_b is a divisor of 97_b...

Message 4: assistant [final]
Content: I'll analyze this step by step and provide a comprehensive answer...
```

## 🎯 **Use Cases**

### **Educational**
- Learn how harmony format works
- Understand tokenization process
- See message parsing in action

### **Development**
- Test harmony format rendering
- Debug conversation structures
- Validate token sequences

### **Research**
- Analyze token efficiency
- Study format overhead
- Compare different prompts

## 🛠️ **Customization**

### **Modify Default Prompt**
Edit `harmony_chatbot_gui.py`:
```python
default_prompt = "Your custom default prompt here..."
```

### **Change Window Sizes**
```python
self.root.geometry("800x600")  # Main window
results_window.geometry("1000x700")  # Results window
```

### **Add More Tabs**
Extend the results window with additional analysis tabs.

## 🎉 **Features Demonstrated**

- ✅ **Complete harmony format** - System, developer, user, assistant messages
- ✅ **Channel usage** - Final channel for assistant responses
- ✅ **Token rendering** - Full conversation to token sequence
- ✅ **Token parsing** - Tokens back to message components
- ✅ **Visual interface** - User-friendly Windows GUI
- ✅ **Real-time processing** - Interactive prompt handling
- ✅ **Error handling** - Graceful failure management
- ✅ **Professional UI** - Modern, clean design

This GUI application provides a complete demonstration of the OpenAI Harmony format in an interactive, visual way that's perfect for learning, testing, and development on Windows.
