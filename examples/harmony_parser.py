#!/usr/bin/env python3
"""
OpenAI Harmony Command-Line Parser
A command-line tool that reads binary token files and parses them back to harmony messages.
"""

import argparse
import sys
import os
import struct
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


def read_binary_tokens(binary_file):
    """Read tokens from binary file (32-bit integers, little-endian)"""
    tokens = []
    
    try:
        with open(binary_file, 'rb') as f:
            while True:
                # Read 4 bytes for each 32-bit integer
                data = f.read(4)
                if not data:
                    break
                
                # Unpack as little-endian unsigned 32-bit integer
                token = struct.unpack('<I', data)[0]
                tokens.append(token)
        
        return tokens
    
    except Exception as e:
        print(f"❌ Error reading binary file '{binary_file}': {e}")
        sys.exit(1)


def parse_tokens_to_messages(tokens, encoding):
    """Parse tokens back to harmony messages"""
    try:
        # Use the harmony parser to convert tokens back to messages
        parsed_messages = encoding.parse_messages_from_completion_tokens(tokens, None)
        return parsed_messages
    
    except Exception as e:
        print(f"❌ Error parsing tokens: {e}")
        sys.exit(1)


def print_message_details(message, index):
    """Print detailed information about a message"""
    print(f"\n{'='*60}")
    print(f"MESSAGE {index + 1}")
    print(f"{'='*60}")
    
    # Basic message info
    print(f"Role: {message.author.role.value}")
    
    if message.channel:
        print(f"Channel: {message.channel}")
    
    if message.recipient:
        print(f"Recipient: {message.recipient}")
    
    if message.content_type:
        print(f"Content Type: {message.content_type}")
    
    # Message content
    print(f"\nContent:")
    print("-" * 40)
    
    if message.content:
        for i, content_item in enumerate(message.content):
            if hasattr(content_item, 'text'):
                # Text content
                text = content_item.text
                if len(text) > 500:  # Truncate very long content
                    print(f"{text[:500]}...")
                    print(f"[Content truncated - total length: {len(text)} characters]")
                else:
                    print(text)
            else:
                # Other content types
                print(f"[Content item {i + 1}: {type(content_item).__name__}]")
    else:
        print("[No content]")


def print_summary(messages, tokens, encoding):
    """Print summary information"""
    print(f"\n{'='*60}")
    print("PARSING SUMMARY")
    print(f"{'='*60}")
    
    print(f"Total tokens processed: {len(tokens)}")
    print(f"Total messages parsed: {len(messages)}")
    print(f"Encoding used: {encoding.name}")
    
    # Message breakdown by role
    role_counts = {}
    for msg in messages:
        role = msg.author.role.value
        role_counts[role] = role_counts.get(role, 0) + 1
    
    print(f"\nMessage breakdown by role:")
    for role, count in role_counts.items():
        print(f"  {role}: {count}")
    
    # Channel breakdown
    channel_counts = {}
    for msg in messages:
        channel = msg.channel if msg.channel else "none"
        channel_counts[channel] = channel_counts.get(channel, 0) + 1
    
    if any(ch != "none" for ch in channel_counts.keys()):
        print(f"\nMessage breakdown by channel:")
        for channel, count in channel_counts.items():
            print(f"  {channel}: {count}")


def print_tokens_preview(tokens, encoding, num_tokens=20):
    """Print preview of first N tokens"""
    print(f"\n{'='*60}")
    print(f"TOKEN PREVIEW (First {min(num_tokens, len(tokens))} tokens)")
    print(f"{'='*60}")
    
    for i, token in enumerate(tokens[:num_tokens]):
        try:
            decoded = encoding.decode_utf8([token])
            print(f"Token {i+1:3d}: {token:6d} → {repr(decoded)}")
        except:
            print(f"Token {i+1:3d}: {token:6d} → [special token]")
    
    if len(tokens) > num_tokens:
        print(f"... and {len(tokens) - num_tokens} more tokens")


def main():
    """Main function"""
    parser = argparse.ArgumentParser(
        description="OpenAI Harmony Command-Line Parser",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Parse binary token file
  python harmony_parser.py tokens.bin
  
  # Parse with verbose output
  python harmony_parser.py tokens.bin --verbose
  
  # Show token preview
  python harmony_parser.py tokens.bin --show-tokens 10
  
  # Compact output (messages only)
  python harmony_parser.py tokens.bin --compact
        """
    )
    
    # Input file
    parser.add_argument(
        "binary_file",
        help="Binary file containing 32-bit integer tokens"
    )
    
    # Output options
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Show detailed parsing information"
    )
    
    parser.add_argument(
        "--compact", "-c",
        action="store_true",
        help="Show compact output (messages only, no details)"
    )
    
    parser.add_argument(
        "--show-tokens",
        type=int,
        metavar="N",
        help="Show first N tokens with decoded values"
    )
    
    parser.add_argument(
        "--summary-only", "-s",
        action="store_true",
        help="Show only summary information"
    )
    
    args = parser.parse_args()
    
    # Check if binary file exists
    if not os.path.exists(args.binary_file):
        print(f"❌ Error: Binary file '{args.binary_file}' not found!")
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
    
    print("🚀 OpenAI Harmony Command-Line Parser")
    print("=" * 50)
    
    if args.verbose:
        print(f"📁 Binary file: {args.binary_file}")
        file_size = os.path.getsize(args.binary_file)
        print(f"📊 File size: {file_size} bytes")
        expected_tokens = file_size // 4
        print(f"📊 Expected tokens: {expected_tokens} (assuming 4 bytes per token)")
    
    # Initialize harmony encoding
    print("🔧 Initializing harmony encoding...")
    encoding = init_harmony_encoding()
    
    if args.verbose:
        print(f"✅ Loaded encoding: {encoding.name}")
    
    # Read binary tokens
    print("📖 Reading binary tokens...")
    tokens = read_binary_tokens(args.binary_file)
    print(f"✅ Read {len(tokens)} tokens from binary file")
    
    # Show token preview if requested
    if args.show_tokens:
        print_tokens_preview(tokens, encoding, args.show_tokens)
    
    # Parse tokens to messages
    print("🔍 Parsing tokens to messages...")
    try:
        messages = parse_tokens_to_messages(tokens, encoding)
        print(f"✅ Parsed {len(messages)} messages from tokens")
    except Exception as e:
        print(f"❌ Error during parsing: {e}")
        sys.exit(1)
    
    # Output results based on options
    if args.summary_only:
        # Show only summary
        print_summary(messages, tokens, encoding)
    
    elif args.compact:
        # Show compact message list
        print(f"\n{'='*60}")
        print("PARSED MESSAGES (COMPACT)")
        print(f"{'='*60}")
        
        for i, msg in enumerate(messages):
            channel = f" [{msg.channel}]" if msg.channel else ""
            recipient = f" → {msg.recipient}" if msg.recipient else ""
            
            # Get content preview
            content_preview = ""
            if msg.content and msg.content[0] and hasattr(msg.content[0], 'text'):
                text = msg.content[0].text
                content_preview = text[:100] + "..." if len(text) > 100 else text
                content_preview = content_preview.replace('\n', ' ')
            
            print(f"{i+1:2d}. {msg.author.role.value}{channel}{recipient}")
            if content_preview:
                print(f"    {content_preview}")
    
    else:
        # Show detailed message information
        print(f"\n{'='*60}")
        print("PARSED MESSAGES (DETAILED)")
        print(f"{'='*60}")
        
        for i, message in enumerate(messages):
            print_message_details(message, i)
        
        # Show summary at the end
        print_summary(messages, tokens, encoding)
    
    print(f"\n🎉 Parsing complete!")
    
    if args.verbose:
        # Additional verification info
        try:
            # Try to render messages back to tokens for verification
            conversation = openai_harmony.Conversation.from_messages(messages)
            re_rendered_tokens = encoding.render_conversation(conversation)
            
            if len(re_rendered_tokens) == len(tokens):
                print(f"✅ Verification: Re-rendered to same number of tokens ({len(tokens)})")
            else:
                print(f"⚠️  Verification: Re-rendered to different number of tokens ({len(re_rendered_tokens)} vs {len(tokens)})")
        
        except Exception as e:
            print(f"⚠️  Verification failed: {e}")


if __name__ == "__main__":
    main()
