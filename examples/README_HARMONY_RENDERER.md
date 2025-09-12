# OpenAI Harmony Command-Line Renderer

A powerful command-line tool that renders user prompts using the OpenAI Harmony format and saves the generated tokens to files. This tool uses the same harmony message composition as the GUI chatbot but provides a scriptable, automation-friendly interface.

## 🎯 **What It Does**

The Harmony Renderer:

1. **Takes a user prompt** (from command line or file)
2. **Creates harmony conversation** with system, developer, user, and assistant messages
3. **Renders to tokens** using the Harmony format renderer
4. **Saves tokens to file** in multiple formats (JSON, text, or binary)
5. **Provides detailed output** with optional token display and verification

## 🚀 **Quick Start**

### **Windows**
```cmd
# Easy way - use the launcher
run_harmony_renderer_windows.bat "What is 2+2?" -o output.json

# Or directly
python harmony_renderer.py "What is 2+2?" -o output.json
```

### **Linux**
```bash
# Easy way - use the launcher (make executable first)
chmod +x run_harmony_renderer_linux.sh
./run_harmony_renderer_linux.sh "What is 2+2?" -o output.json

# Or directly
python3 harmony_renderer.py "What is 2+2?" -o output.json
```

## 📋 **Prerequisites**

### **Windows**
```cmd
# 1. Download vocabulary files
download_vocab_files_rust_windows.bat

# 2. Install Python dependencies
install_python_dependencies_windows.bat

# 3. Build Python bindings
build_python_rust_windows.bat
```

### **Linux**
```bash
# 1. Download vocabulary files
./download_vocab_files_linux.sh

# 2. Install Python dependencies
./install_python_dependencies_linux.sh

# 3. Build Python bindings
./build_python_rust_linux.sh
```

## 🔧 **Usage**

### **Basic Syntax**
```bash
python harmony_renderer.py [PROMPT] -o OUTPUT_FILE [OPTIONS]
```

### **Input Methods**

#### **1. Direct Prompt**
```bash
# Simple prompt
python harmony_renderer.py "Hello, world!" -o output.json

# Multi-word prompt (use quotes)
python harmony_renderer.py "What is the meaning of life?" -o result.txt

# Complex prompt with special characters
python harmony_renderer.py "Find the sum of all integer bases b>9 for which 17_b is a divisor of 97_b." -o math.json
```

#### **2. From File**
```bash
# Read prompt from file
python harmony_renderer.py --input prompt.txt -o output.json
python harmony_renderer.py -i my_question.txt -o result.txt
```

### **Output Formats**

**The renderer ALWAYS saves both JSON and binary formats regardless of the specified extension:**

#### **Dual Output (Always Generated)**
```bash
python harmony_renderer.py "Hello" -o output
```

**Generates:**
- `output.json` - Complete JSON with metadata, tokens, and rendered text
- `output.bin` - Raw binary tokens as 32-bit unsigned integers (little-endian)

#### **JSON Format (output.json)**
```json
{
  "metadata": {
    "timestamp": "2025-09-10T17:30:00.123456",
    "encoding_name": "HarmonyGptOss",
    "total_tokens": 156,
    "user_prompt": "Hello",
    "harmony_renderer_version": "1.0.0"
  },
  "tokens": [100257, 9125, 100264, ...],
  "rendered_text": "<|start|>system<|message|>You are ChatGPT..."
}
```

#### **Binary Format (output.bin)**
- Raw tokens stored as 32-bit unsigned integers
- Little-endian byte order
- 4 bytes per token
- No headers or metadata (pure token data)
- File size = token_count × 4 bytes

#### **Optional Text Format (.txt)**
```bash
python harmony_renderer.py "Hello" -o output.txt
```

**Also generates output.txt with:**
- Metadata header with timestamp, encoding, token count
- Complete rendered harmony format text
- Detailed token list with decoded values

### **Advanced Options**

#### **Verbose Output**
```bash
python harmony_renderer.py "Explain AI" -o result.json --verbose
```

Shows detailed processing information:
- User prompt preview
- Encoding details
- Conversation creation status
- Token rendering progress
- Verification results

#### **Show Tokens**
```bash
python harmony_renderer.py "Hello" -o output.json --show-tokens 10
```

Displays first N tokens to console:
```
🔢 First 10 tokens:
  Token  1: 100257 → '<|start|>'
  Token  2:   9125 → 'system'
  Token  3: 100264 → '<|message|>'
  ...
```

#### **Help**
```bash
python harmony_renderer.py --help
```

## 🎨 **Harmony Message Composition**

The renderer creates the same harmony conversation structure as the GUI:

### **1. System Message**
```python
system_content = (
    openai_harmony.SystemContent.new()
    .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
    .with_knowledge_cutoff("2024-06")
    .with_conversation_start_date("2025-08-05")
    .with_reasoning_effort(openai_harmony.ReasoningEffort.MEDIUM)
    .with_required_channels(["analysis", "commentary", "final"])
)
```

### **2. Developer Message**
```python
developer_content = (
    openai_harmony.DeveloperContent.new()
    .with_instructions("You are a helpful assistant that can answer questions and help with tasks.")
)
```

### **3. User Message**
```python
user_message = openai_harmony.Message.from_role_and_content(
    openai_harmony.Role.USER,
    user_prompt  # Your input prompt
)
```

### **4. Assistant Message**
```python
assistant_message = openai_harmony.Message.from_role_and_content(
    openai_harmony.Role.ASSISTANT,
    "I'll analyze this step by step and provide a comprehensive answer."
).with_channel("final")
```

## 📊 **Example Workflows**

### **1. Research Analysis**
```bash
# Create prompt file
echo "Analyze the impact of artificial intelligence on modern education systems." > research_prompt.txt

# Render with verbose output
python harmony_renderer.py -i research_prompt.txt -o research_analysis.json --verbose --show-tokens 20

# View results
cat research_analysis.json
```

### **2. Mathematical Problem**
```bash
# Render math problem
python harmony_renderer.py "Find the derivative of f(x) = x^3 + 2x^2 - 5x + 1" -o math_problem.txt

# Check token count
grep "Total Tokens:" math_problem.txt
```

### **3. Batch Processing**
```bash
# Process multiple prompts
for prompt in "Hello" "Goodbye" "Thank you"; do
    python harmony_renderer.py "$prompt" -o "${prompt,,}.json"
done
```

### **4. Token Analysis**
```bash
# Save raw tokens for analysis
python harmony_renderer.py "Complex reasoning task" -o tokens.bin

# Check file sizes
ls -la tokens.bin tokens.bin.meta
```

## 🔍 **Output Analysis**

### **Token Statistics**
```bash
# Get token count from JSON
python -c "import json; data=json.load(open('output.json')); print(f'Tokens: {data[\"metadata\"][\"total_tokens\"]}')"

# Get token count from text file
grep "Total Tokens:" output.txt
```

### **Rendered Format Inspection**
```bash
# Extract just the rendered text
python -c "import json; data=json.load(open('output.json')); print(data['rendered_text'])"

# View token breakdown
grep "Token" output.txt | head -20
```

## 🛠️ **Integration Examples**

### **Shell Script Integration**
```bash
#!/bin/bash
# process_prompts.sh

PROMPTS_DIR="prompts"
OUTPUT_DIR="rendered"

mkdir -p "$OUTPUT_DIR"

for prompt_file in "$PROMPTS_DIR"/*.txt; do
    basename=$(basename "$prompt_file" .txt)
    echo "Processing: $basename"
    
    python3 harmony_renderer.py \
        --input "$prompt_file" \
        --output "$OUTPUT_DIR/${basename}_rendered.json" \
        --verbose
done

echo "Batch processing complete!"
```

### **Python Script Integration**
```python
#!/usr/bin/env python3
import subprocess
import json
import sys

def render_prompt(prompt, output_file):
    """Render a prompt using harmony renderer"""
    cmd = [
        sys.executable, "harmony_renderer.py",
        prompt,
        "-o", output_file,
        "--verbose"
    ]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        # Load and return the rendered data
        with open(output_file, 'r') as f:
            return json.load(f)
    else:
        raise Exception(f"Rendering failed: {result.stderr}")

# Example usage
data = render_prompt("What is machine learning?", "ml_output.json")
print(f"Rendered {data['metadata']['total_tokens']} tokens")
```

## 🐛 **Troubleshooting**

### **Common Errors**

#### **1. "openai_harmony package not found"**
```bash
# Solution: Build Python bindings
# Windows:
build_python_rust_windows.bat

# Linux:
./build_python_rust_linux.sh
```

#### **2. "vocab_files directory not found"**
```bash
# Solution: Download vocabulary files
# Windows:
download_vocab_files_rust_windows.bat

# Linux:
./download_vocab_files_linux.sh
```

#### **3. "Failed to load harmony encoding"**
```bash
# Check if vocab files exist
ls -la vocab_files/

# Should contain:
# o200k_base.tiktoken
# cl100k_base.tiktoken
```

#### **4. "Permission denied" (Linux)**
```bash
# Make script executable
chmod +x run_harmony_renderer_linux.sh

# Or run directly
python3 harmony_renderer.py "prompt" -o output.json
```

### **Environment Issues**
```bash
# Check Python environment
python --version  # or python3 --version

# Check if in virtual environment
echo $VIRTUAL_ENV  # Linux
echo %VIRTUAL_ENV%  # Windows

# Activate virtual environment if needed
source .venv/bin/activate  # Linux
.venv\Scripts\activate.bat  # Windows
```

## 📈 **Performance Notes**

### **Typical Processing Times**
- **Simple prompt** (1-10 words): < 1 second
- **Medium prompt** (50-100 words): 1-2 seconds  
- **Complex prompt** (500+ words): 2-5 seconds

### **Memory Usage**
- **Base memory**: ~50-100 MB
- **Per token**: ~4 bytes (binary format)
- **JSON overhead**: ~2-3x token size

### **File Sizes**
- **JSON format**: Largest (includes metadata + readable tokens)
- **Text format**: Medium (formatted for readability)
- **Binary format**: Smallest (raw tokens only)

## 🎯 **Use Cases**

### **Development & Testing**
- Test harmony format rendering with different prompts
- Generate test data for AI model development
- Validate token sequences and parsing

### **Research & Analysis**
- Study tokenization patterns across different prompt types
- Analyze harmony format overhead and efficiency
- Compare token distributions for various inputs

### **Automation & Integration**
- Batch process multiple prompts
- Integrate with CI/CD pipelines
- Generate training data for downstream tasks

### **Educational**
- Learn how harmony format works
- Understand tokenization process
- Explore AI model input formatting

## 🎉 **Summary**

The OpenAI Harmony Command-Line Renderer provides:

- ✅ **Complete harmony format support** - Same as GUI chatbot
- ✅ **Multiple output formats** - JSON, text, binary
- ✅ **Flexible input methods** - Direct prompt or file
- ✅ **Cross-platform support** - Windows and Linux
- ✅ **Automation friendly** - Scriptable and integrable
- ✅ **Detailed analysis** - Token display and verification
- ✅ **Professional tooling** - Error handling and validation

**Perfect for developers, researchers, and anyone working with OpenAI Harmony format!**
