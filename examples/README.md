# OpenAI Harmony Examples

Example applications and tools demonstrating the OpenAI Harmony format.

## Contents

### Command-Line Tools
- **`harmony_renderer.py`** - Convert prompts to harmony format tokens
- **`harmony_parser.py`** - Parse binary token files back to messages  
- **`harmony_chatbot_gui.py`** - Interactive GUI for harmony format exploration

### Platform Launchers
- **Windows**: `run_harmony_*_windows.bat` files
- **Linux**: `run_harmony_*_linux.sh` files (make executable with `chmod +x *.sh`)


### Documentation
- **`README_HARMONY_*.md`** - Complete guides for each tool

## Quick Start

### Prerequisites
1. Build the project using scripts in `../build_utils/`
2. Download vocabulary files and install dependencies (see build_utils)

### Usage Examples

```bash
# Windows
run_harmony_renderer_windows.bat "What is AI?" -o analysis
run_harmony_parser_windows.bat analysis.bin
run_harmony_chatbot_gui.bat

# Linux  
./run_harmony_renderer_linux.sh "What is AI?" -o analysis
./run_harmony_parser_linux.sh analysis.bin
./run_harmony_chatbot_gui_linux.sh
# GUI requires: sudo apt install python3-tk
```

## Learning Path

1. **Start with GUI** - `run_harmony_chatbot_gui.*` for interactive exploration
2. **Try command-line tools** - Renderer and parser for automation
3. **Read tool documentation** - `README_HARMONY_*.md` for detailed usage

These examples demonstrate the complete workflow: prompts → tokens → parsed messages.
