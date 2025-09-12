#!/usr/bin/env python3
"""
OpenAI Harmony Chatbot GUI App for Windows
A GUI application that demonstrates the harmony format with user prompts.
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import openai_harmony
import threading
import os

class HarmonyChatbotGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("OpenAI Harmony Chatbot - Windows")
        self.root.geometry("800x600")
        self.root.configure(bg='#f0f0f0')
        
        # Initialize harmony encoding
        self.encoding = None
        self.init_harmony()
        
        # Create main UI
        self.create_main_ui()
        
    def init_harmony(self):
        """Initialize the harmony encoding"""
        try:
            # Determine vocab_files path (check current dir, then parent dir)
            vocab_path = 'vocab_files'
            if not os.path.exists(vocab_path):
                vocab_path = '../vocab_files'
            
            # Set environment variable for local vocab files
            os.environ['TIKTOKEN_ENCODINGS_BASE'] = vocab_path
            self.encoding = openai_harmony.load_harmony_encoding("HarmonyGptOss")
            print(f"✅ Loaded harmony encoding: {self.encoding.name}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load harmony encoding: {e}")
            self.root.quit()
    
    def create_main_ui(self):
        """Create the main user interface"""
        # Title
        title_label = tk.Label(
            self.root, 
            text="🤖 OpenAI Harmony Chatbot", 
            font=("Arial", 20, "bold"),
            bg='#f0f0f0',
            fg='#2c3e50'
        )
        title_label.pack(pady=20)
        
        # Subtitle
        subtitle_label = tk.Label(
            self.root,
            text="Enter your prompt below to see harmony format in action",
            font=("Arial", 12),
            bg='#f0f0f0',
            fg='#7f8c8d'
        )
        subtitle_label.pack(pady=(0, 20))
        
        # User prompt frame
        prompt_frame = tk.Frame(self.root, bg='#f0f0f0')
        prompt_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=10)
        
        # Prompt label
        prompt_label = tk.Label(
            prompt_frame,
            text="Your Prompt:",
            font=("Arial", 14, "bold"),
            bg='#f0f0f0',
            fg='#2c3e50'
        )
        prompt_label.pack(anchor=tk.W, pady=(0, 5))
        
        # Prompt text area
        self.prompt_text = scrolledtext.ScrolledText(
            prompt_frame,
            height=8,
            font=("Consolas", 11),
            wrap=tk.WORD,
            bg='white',
            fg='#2c3e50',
            insertbackground='#3498db'
        )
        self.prompt_text.pack(fill=tk.BOTH, expand=True, pady=(0, 10))
        
        # Default prompt
        default_prompt = "Find the sum of all integer bases b>9 for which 17_b is a divisor of 97_b.\nPlease reason step by step, and put your final answer within \\boxed{}."
        self.prompt_text.insert(tk.END, default_prompt)
        
        # Button frame
        button_frame = tk.Frame(self.root, bg='#f0f0f0')
        button_frame.pack(pady=20)
        
        # Process button
        self.process_btn = tk.Button(
            button_frame,
            text="🚀 Process with Harmony Format",
            font=("Arial", 12, "bold"),
            bg='#3498db',
            fg='white',
            padx=20,
            pady=10,
            command=self.process_prompt,
            cursor='hand2'
        )
        self.process_btn.pack(side=tk.LEFT, padx=10)
        
        # Clear button
        clear_btn = tk.Button(
            button_frame,
            text="🗑️ Clear",
            font=("Arial", 12),
            bg='#95a5a6',
            fg='white',
            padx=20,
            pady=10,
            command=self.clear_prompt,
            cursor='hand2'
        )
        clear_btn.pack(side=tk.LEFT, padx=10)
        
        # Status label
        self.status_label = tk.Label(
            self.root,
            text="Ready to process harmony format",
            font=("Arial", 10),
            bg='#f0f0f0',
            fg='#27ae60'
        )
        self.status_label.pack(pady=10)
        
    def clear_prompt(self):
        """Clear the prompt text area"""
        self.prompt_text.delete(1.0, tk.END)
        self.status_label.config(text="Prompt cleared", fg='#f39c12')
        
    def process_prompt(self):
        """Process the user prompt with harmony format"""
        user_prompt = self.prompt_text.get(1.0, tk.END).strip()
        
        if not user_prompt:
            messagebox.showwarning("Warning", "Please enter a prompt first!")
            return
            
        # Disable button during processing
        self.process_btn.config(state=tk.DISABLED, text="Processing...")
        self.status_label.config(text="Creating harmony conversation...", fg='#f39c12')
        
        # Process in a separate thread to avoid blocking UI
        threading.Thread(target=self._process_harmony, args=(user_prompt,), daemon=True).start()
        
    def _process_harmony(self, user_prompt):
        """Process harmony format in background thread"""
        try:
            # Create harmony conversation (based on harmony_format_example_corrected.py)
            conversation = self.create_harmony_conversation(user_prompt)
            
            # Render to tokens
            tokens = self.encoding.render_conversation(conversation)
            
            # Parse back from tokens
            parsed_messages = self.encoding.parse_messages_from_completion_tokens(tokens, None)
            
            # Update UI in main thread
            self.root.after(0, self.show_results, tokens, parsed_messages, user_prompt)
            
        except Exception as e:
            self.root.after(0, self.show_error, str(e))
            
    def create_harmony_conversation(self, user_prompt):
        """Create harmony conversation exactly like harmony_format_example_corrected.py"""
        
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
        
        # 3. User message (from GUI input)
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
        
    def show_results(self, tokens, parsed_messages, original_prompt):
        """Show the harmony format results in a new window"""
        # Re-enable button
        self.process_btn.config(state=tk.NORMAL, text="🚀 Process with Harmony Format")
        self.status_label.config(text="Processing complete! Opening results window...", fg='#27ae60')
        
        # Create results window
        results_window = tk.Toplevel(self.root)
        results_window.title("Harmony Format Results")
        results_window.geometry("1000x700")
        results_window.configure(bg='#ecf0f1')
        
        # Make it modal
        results_window.transient(self.root)
        results_window.grab_set()
        
        # Title
        title_label = tk.Label(
            results_window,
            text="🎯 Harmony Format Processing Results",
            font=("Arial", 16, "bold"),
            bg='#ecf0f1',
            fg='#2c3e50'
        )
        title_label.pack(pady=15)
        
        # Create notebook for tabs
        notebook = ttk.Notebook(results_window)
        notebook.pack(fill=tk.BOTH, expand=True, padx=20, pady=10)
        
        # Tab 1: First 20 Tokens
        tokens_frame = tk.Frame(notebook, bg='white')
        notebook.add(tokens_frame, text="🔢 First 20 Tokens")
        
        tokens_info = tk.Label(
            tokens_frame,
            text=f"Total tokens generated: {len(tokens)} | Showing first 20:",
            font=("Arial", 12, "bold"),
            bg='white',
            fg='#2c3e50'
        )
        tokens_info.pack(pady=10)
        
        first_20_tokens = tokens[:20]
        tokens_text = scrolledtext.ScrolledText(
            tokens_frame,
            height=15,
            font=("Consolas", 11),
            bg='#f8f9fa',
            fg='#2c3e50'
        )
        tokens_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Display tokens with their decoded values
        for i, token in enumerate(first_20_tokens):
            try:
                decoded = self.encoding.decode_utf8([token])
                tokens_text.insert(tk.END, f"Token {i+1:2d}: {token:6d} → {repr(decoded)}\n")
            except:
                tokens_text.insert(tk.END, f"Token {i+1:2d}: {token:6d} → [special token]\n")
        
        tokens_text.config(state=tk.DISABLED)
        
        # Tab 2: Parsed Messages
        messages_frame = tk.Frame(notebook, bg='white')
        notebook.add(messages_frame, text="📝 Parsed Messages")
        
        messages_info = tk.Label(
            messages_frame,
            text=f"Messages parsed from tokens: {len(parsed_messages)}",
            font=("Arial", 12, "bold"),
            bg='white',
            fg='#2c3e50'
        )
        messages_info.pack(pady=10)
        
        messages_text = scrolledtext.ScrolledText(
            messages_frame,
            height=15,
            font=("Consolas", 10),
            bg='#f8f9fa',
            fg='#2c3e50'
        )
        messages_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Display parsed messages
        for i, msg in enumerate(parsed_messages):
            channel = f" [{msg.channel}]" if msg.channel else ""
            recipient = f" → {msg.recipient}" if msg.recipient else ""
            content_type = f" ({msg.content_type})" if msg.content_type else ""
            
            messages_text.insert(tk.END, f"Message {i+1}: {msg.author.role.value}{channel}{recipient}{content_type}\n")
            messages_text.insert(tk.END, f"Content: {str(msg.content[0].text if msg.content else 'N/A')[:100]}...\n")
            messages_text.insert(tk.END, "-" * 80 + "\n\n")
        
        messages_text.config(state=tk.DISABLED)
        
        # Tab 3: Full Rendered Text
        rendered_frame = tk.Frame(notebook, bg='white')
        notebook.add(rendered_frame, text="🎨 Rendered Format")
        
        rendered_info = tk.Label(
            rendered_frame,
            text="Complete harmony format output:",
            font=("Arial", 12, "bold"),
            bg='white',
            fg='#2c3e50'
        )
        rendered_info.pack(pady=10)
        
        rendered_text = scrolledtext.ScrolledText(
            rendered_frame,
            height=15,
            font=("Consolas", 9),
            bg='#f8f9fa',
            fg='#2c3e50',
            wrap=tk.WORD
        )
        rendered_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Show full rendered text
        full_rendered = self.encoding.decode_utf8(tokens)
        rendered_text.insert(tk.END, full_rendered)
        rendered_text.config(state=tk.DISABLED)
        
        # Close button
        close_btn = tk.Button(
            results_window,
            text="✅ Close Results",
            font=("Arial", 12, "bold"),
            bg='#27ae60',
            fg='white',
            padx=20,
            pady=10,
            command=results_window.destroy,
            cursor='hand2'
        )
        close_btn.pack(pady=15)
        
        # Update status
        self.status_label.config(text="Ready for next prompt", fg='#27ae60')
        
    def show_error(self, error_msg):
        """Show error message"""
        self.process_btn.config(state=tk.NORMAL, text="🚀 Process with Harmony Format")
        self.status_label.config(text="Error occurred", fg='#e74c3c')
        messagebox.showerror("Error", f"Processing failed: {error_msg}")

def main():
    """Main function to run the GUI application"""
    print("🚀 Starting OpenAI Harmony Chatbot GUI...")
    
    # Check if vocab files exist (check current dir, then parent dir)
    vocab_path = 'vocab_files'
    if not os.path.exists(vocab_path):
        vocab_path = '../vocab_files'
    
    if not os.path.exists(vocab_path):
        print("❌ vocab_files directory not found!")
        print("Please run: download_vocab_files_rust_windows.bat")
        return
    
    # Create and run GUI
    root = tk.Tk()
    app = HarmonyChatbotGUI(root)
    
    print("✅ GUI started successfully!")
    print("💡 Enter a prompt and click 'Process with Harmony Format' to see the magic!")
    
    root.mainloop()

if __name__ == "__main__":
    main()
