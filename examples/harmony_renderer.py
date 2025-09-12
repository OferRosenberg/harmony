#!/usr/bin/env python3
"""
OpenAI Harmony Command-Line Renderer
A command-line tool that renders user prompts using the harmony format and saves tokens to a file.
"""

import argparse
import sys
import os
import json
from datetime import datetime
from pathlib import Path

try:
    import openai_harmony
except ImportError:
    print("❌ Error: openai_harmony package not found!")
    print("Please install it first:")
    print("  Windows: build_python_rust_windows.bat")
    print("  Linux: ./build_python_rust_linux.sh")
    sys.exit(1)


def init_harmony_encoding():
    """Initialize the harmony encoding"""
    try:
        # Determine vocab_files path (check current dir, then parent dir)
        vocab_path = 'vocab_files'
        if not os.path.exists(vocab_path):
            vocab_path = '../vocab_files'
        
        # Set environment variable for local vocab files
        os.environ['TIKTOKEN_ENCODINGS_BASE'] = vocab_path
        encoding = openai_harmony.load_harmony_encoding("HarmonyGptOss")
        return encoding
    except Exception as e:
        print(f"❌ Error: Failed to load harmony encoding: {e}")
        print("Make sure vocab_files directory exists and contains the vocabulary files.")
        print("Run download_vocab_files script first.")
        sys.exit(1)


def create_harmony_conversation(user_prompt):
    """Create harmony conversation with the same components as GUI"""
    
    # 1. System message
    system_content = (
        openai_harmony.SystemContent.new()
        .with_model_identity("You are ChatGPT, a large language model trained by OpenAI.")
        .with_knowledge_cutoff("2024-06")
        .with_conversation_start_date("2025-08-05")
        .with_reasoning_effort(openai_harmony.ReasoningEffort.MEDIUM)
        .with_required_channels(["analysis", "commentary", "final"])
    )
    
    system_message = openai_harmony.Message.from_role_and_content(
        openai_harmony.Role.SYSTEM,
        system_content
    )
    
    # 2. Developer message
    developer_content = (
        openai_harmony.DeveloperContent.new()
        .with_instructions("You are a helpful assistant that can answer questions and help with tasks.")
    )
    
    developer_message = openai_harmony.Message.from_role_and_content(
        openai_harmony.Role.DEVELOPER,
        developer_content
    )
    
    # 3. User message (from command line input)
    user_message = openai_harmony.Message.from_role_and_content(
        openai_harmony.Role.USER,
        user_prompt
    )
    
    # 4. Assistant response (simulated)
    assistant_message = openai_harmony.Message.from_role_and_content(
        openai_harmony.Role.ASSISTANT,
        "I'll analyze this step by step and provide a comprehensive answer."
    ).with_channel("final")
    
    # Create conversation
    conversation = openai_harmony.Conversation.from_messages([
        system_message,
        developer_message,
        user_message,
        assistant_message
    ])
    
    return conversation


def save_tokens_to_file(tokens, output_file, user_prompt, encoding):
    """Save tokens and metadata to file in both JSON and binary formats"""
    
    # Prepare data to save
    data = {
        "metadata": {
            "timestamp": datetime.now().isoformat(),
            "encoding_name": encoding.name,
            "total_tokens": len(tokens),
            "user_prompt": user_prompt,
            "harmony_renderer_version": "1.0.0"
        },
        "tokens": tokens,
        "rendered_text": encoding.decode_utf8(tokens)
    }
    
    # Determine base filename and extension
    output_path = Path(output_file)
    base_name = output_path.stem
    base_dir = output_path.parent
    
    # Always save JSON format
    json_file = base_dir / f"{base_name}.json"
    with open(json_file, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    print(f"💾 Saved tokens as JSON to: {json_file}")
    
    # Always save binary format (32-bit integers)
    bin_file = base_dir / f"{base_name}.bin"
    with open(bin_file, 'wb') as f:
        # Write tokens as binary data (4 bytes per token, little-endian)
        for token in tokens:
            f.write(token.to_bytes(4, byteorder='little', signed=False))
    print(f"💾 Saved tokens as binary (32-bit integers) to: {bin_file}")
    
    # If user requested text format, also save that
    if output_path.suffix.lower() == '.txt':
        txt_file = base_dir / f"{base_name}.txt"
        with open(txt_file, 'w', encoding='utf-8') as f:
            f.write("# OpenAI Harmony Renderer Output\n")
            f.write(f"# Timestamp: {data['metadata']['timestamp']}\n")
            f.write(f"# Encoding: {data['metadata']['encoding_name']}\n")
            f.write(f"# Total Tokens: {data['metadata']['total_tokens']}\n")
            f.write(f"# User Prompt: {user_prompt}\n")
            f.write("#" + "="*60 + "\n\n")
            f.write("# RENDERED HARMONY FORMAT:\n")
            f.write(data['rendered_text'])
            f.write("\n\n# TOKEN LIST:\n")
            for i, token in enumerate(tokens):
                try:
                    decoded = encoding.decode_utf8([token])
                    f.write(f"Token {i+1:4d}: {token:6d} → {repr(decoded)}\n")
                except:
                    f.write(f"Token {i+1:4d}: {token:6d} → [special token]\n")
        print(f"💾 Saved tokens as text to: {txt_file}")
    
    # Save binary format info
    print(f"📊 Binary format: {len(tokens)} tokens × 4 bytes = {len(tokens) * 4} bytes")
    print(f"📊 Each token stored as 32-bit unsigned integer (little-endian)")


def main():
    """Main function"""
    parser = argparse.ArgumentParser(
        description="OpenAI Harmony Command-Line Renderer",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Render a prompt and save as JSON
  python harmony_renderer.py "What is 2+2?" -o output.json
  
  # Render from file and save as text
  python harmony_renderer.py --input prompt.txt -o output.txt
  
  # Save raw tokens
  python harmony_renderer.py "Hello world" -o tokens.bin
  
  # Verbose output
  python harmony_renderer.py "Explain AI" -o result.json --verbose
        """
    )
    
    # Input options (mutually exclusive)
    input_group = parser.add_mutually_exclusive_group(required=True)
    input_group.add_argument(
        "prompt", 
        nargs='?', 
        help="User prompt to render (use quotes for multi-word prompts)"
    )
    input_group.add_argument(
        "--input", "-i", 
        type=str, 
        help="Read prompt from file"
    )
    
    # Output options
    parser.add_argument(
        "--output", "-o", 
        type=str, 
        required=True,
        help="Output file (.json, .txt, or .bin for raw tokens)"
    )
    
    # Additional options
    parser.add_argument(
        "--verbose", "-v", 
        action="store_true", 
        help="Show detailed processing information"
    )
    
    parser.add_argument(
        "--show-tokens", 
        type=int, 
        metavar="N",
        help="Display first N tokens to console (default: don't show)"
    )
    
    args = parser.parse_args()
    
    # Get user prompt
    if args.prompt:
        user_prompt = args.prompt
    else:
        try:
            with open(args.input, 'r', encoding='utf-8') as f:
                user_prompt = f.read().strip()
        except Exception as e:
            print(f"❌ Error reading input file '{args.input}': {e}")
            sys.exit(1)
    
    if not user_prompt:
        print("❌ Error: Empty prompt provided")
        sys.exit(1)
    
    # Check if vocab files exist (check current dir, then parent dir)
    vocab_path = 'vocab_files'
    if not os.path.exists(vocab_path):
        vocab_path = '../vocab_files'
    
    if not os.path.exists(vocab_path):
        print("❌ Error: vocab_files directory not found!")
        print("Please run the vocabulary download script first:")
        print("  Windows: download_vocab_files_rust_windows.bat")
        print("  Linux: ./download_vocab_files_linux.sh")
        sys.exit(1)
    
    print("🚀 OpenAI Harmony Command-Line Renderer")
    print("=" * 50)
    
    if args.verbose:
        print(f"📝 User prompt: {user_prompt[:100]}{'...' if len(user_prompt) > 100 else ''}")
        print(f"💾 Output file: {args.output}")
    
    # Initialize harmony encoding
    print("🔧 Initializing harmony encoding...")
    encoding = init_harmony_encoding()
    
    if args.verbose:
        print(f"✅ Loaded encoding: {encoding.name}")
    
    # Create harmony conversation
    print("🏗️  Creating harmony conversation...")
    try:
        conversation = create_harmony_conversation(user_prompt)
        if args.verbose:
            print("✅ Conversation created with system, developer, user, and assistant messages")
    except Exception as e:
        print(f"❌ Error creating conversation: {e}")
        sys.exit(1)
    
    # Render to tokens
    print("🎨 Rendering conversation to tokens...")
    try:
        tokens = encoding.render_conversation(conversation)
        print(f"✅ Rendered to {len(tokens)} tokens")
    except Exception as e:
        print(f"❌ Error rendering conversation: {e}")
        sys.exit(1)
    
    # Show first N tokens if requested
    if args.show_tokens:
        print(f"\n🔢 First {args.show_tokens} tokens:")
        for i, token in enumerate(tokens[:args.show_tokens]):
            try:
                decoded = encoding.decode_utf8([token])
                print(f"  Token {i+1:2d}: {token:6d} → {repr(decoded)}")
            except:
                print(f"  Token {i+1:2d}: {token:6d} → [special token]")
    
    # Save tokens to file
    print("💾 Saving tokens to file...")
    try:
        save_tokens_to_file(tokens, args.output, user_prompt, encoding)
    except Exception as e:
        print(f"❌ Error saving to file: {e}")
        sys.exit(1)
    
    print("\n🎉 Harmony rendering complete!")
    
    if args.verbose:
        # Parse tokens back to verify
        try:
            parsed_messages = encoding.parse_messages_from_completion_tokens(tokens, None)
            print(f"✅ Verification: Parsed {len(parsed_messages)} messages from tokens")
        except Exception as e:
            print(f"⚠️  Warning: Could not parse tokens back: {e}")


if __name__ == "__main__":
    main()
