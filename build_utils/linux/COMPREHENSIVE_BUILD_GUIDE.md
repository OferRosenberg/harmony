# OpenAI Harmony - Linux Build Requirements

This document outlines the requirements and setup process for building OpenAI Harmony Rust and Python components on Linux.

## 🎯 **Overview**

OpenAI Harmony can be built on Linux with both Rust and Python bindings. The build process is similar to Windows but uses Linux-specific tools and package managers.

## 📋 **System Requirements**

### **Operating System**
- Ubuntu 20.04+ / Debian 11+
- CentOS 8+ / RHEL 8+ / Fedora 35+
- Arch Linux (current)
- Other Linux distributions with equivalent package versions

### **Architecture Support**
- ✅ **x86_64** (Intel/AMD 64-bit) - Full support
- ✅ **aarch64** (ARM64) - Full support
- ✅ **armv7** (ARM 32-bit) - Should work
- ⚠️ **Other architectures** - May require additional configuration

## 🔧 **Required Dependencies**

### **1. Rust Toolchain**
```bash
# Install Rust via rustup (recommended)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source ~/.cargo/env

# Verify installation
rustc --version  # Should be 1.70.0 or later
cargo --version
```

### **2. Python Environment**
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install python3 python3-pip python3-venv python3-dev

# CentOS/RHEL/Fedora
sudo dnf install python3 python3-pip python3-devel

# Arch Linux
sudo pacman -S python python-pip

# Verify installation
python3 --version  # Should be 3.8 or later
pip3 --version
```

### **3. Build Tools**
```bash
# Ubuntu/Debian
sudo apt install build-essential pkg-config libssl-dev

# CentOS/RHEL/Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install pkg-config openssl-devel

# Arch Linux
sudo pacman -S base-devel pkg-config openssl
```

### **4. Additional Libraries (Optional)**
```bash
# For better performance and compatibility
# Ubuntu/Debian
sudo apt install curl wget git

# CentOS/RHEL/Fedora
sudo dnf install curl wget git

# Arch Linux
sudo pacman -S curl wget git
```

## 🚀 **Build Process**

### **Option 1: Using Provided Scripts (Recommended)**

#### **Step 1: Download Vocabulary Files**
```bash
./download_vocab_files_linux.sh
```

#### **Step 2: Build Rust Library**
```bash
./build_rust_linux.sh
```

#### **Step 3: Setup Python Environment**
```bash
./install_python_dependencies_linux.sh
```

#### **Step 4: Build Python Bindings**
```bash
./build_python_rust_linux.sh
```

#### **Step 5: Run Tests**
```bash
./test_rust_linux.sh
./test_python_rust_linux.sh
```

### **Option 2: Manual Build**

#### **Step 1: Clone and Setup**
```bash
git clone https://github.com/openai/harmony.git
cd harmony
```

#### **Step 2: Download Vocabulary Files**
```bash
# Create vocab_files directory
mkdir -p vocab_files

# Download required tokenizer files
curl -L -o vocab_files/o200k_base.tiktoken \
  "https://openaipublic.blob.core.windows.net/encodings/o200k_base.tiktoken"

curl -L -o vocab_files/cl100k_base.tiktoken \
  "https://openaipublic.blob.core.windows.net/encodings/cl100k_base.tiktoken"

# Verify downloads
ls -la vocab_files/
```

#### **Step 3: Build Rust Library**
```bash
# Set environment for local vocab files
export TIKTOKEN_ENCODINGS_BASE=vocab_files

# Build without reqwest dependency (uses local vocab files)
cargo build --release --no-default-features

# Run Rust tests
cargo test --release --no-default-features
```

#### **Step 4: Setup Python Environment**
```bash
# Create virtual environment
python3 -m venv .venv
source .venv/bin/activate

# Upgrade pip
pip install --upgrade pip

# Install build dependencies
pip install maturin>=1.8,<2.0
pip install pydantic>=2.11.7
pip install pytest

# Optional: Install demo dependencies
pip install uvicorn fastapi
```

#### **Step 5: Build Python Bindings**
```bash
# Activate virtual environment
source .venv/bin/activate

# Set environment for local vocab files
export TIKTOKEN_ENCODINGS_BASE=vocab_files

# Build Python wheel with maturin
maturin develop --release --features python-binding --no-default-features

# Run Python tests
pytest tests/
```

## 🔍 **Feature Configuration**

### **Default Features**
```bash
# Standard build with remote vocabulary download
cargo build --release
```

### **No Remote Dependencies (Recommended)**
```bash
# Build without reqwest, uses local vocab files
cargo build --release --no-default-features
export TIKTOKEN_ENCODINGS_BASE=vocab_files
```

### **Python Bindings Only**
```bash
# Build only Python bindings
cargo build --release --no-default-features --features python-binding
```

### **WASM Bindings**
```bash
# Build for WebAssembly
cargo build --release --no-default-features --features wasm-binding --target wasm32-unknown-unknown
```

## 📊 **Performance Considerations**

### **Compilation Optimization**
```bash
# Release build (optimized)
cargo build --release

# Debug build (faster compilation, slower runtime)
cargo build

# Specific target optimization
export RUSTFLAGS="-C target-cpu=native"
cargo build --release
```

### **Parallel Compilation**
```bash
# Use all CPU cores
export CARGO_BUILD_JOBS=$(nproc)
cargo build --release
```

### **Memory Usage**
```bash
# Reduce memory usage during compilation
export CARGO_BUILD_JOBS=1
cargo build --release
```

## 🐛 **Troubleshooting**

### **Common Issues**

#### **1. Rust Not Found**
```bash
# Add Rust to PATH
echo 'source ~/.cargo/env' >> ~/.bashrc
source ~/.bashrc
```

#### **2. Python Development Headers Missing**
```bash
# Ubuntu/Debian
sudo apt install python3-dev

# CentOS/RHEL/Fedora
sudo dnf install python3-devel
```

#### **3. OpenSSL Development Libraries Missing**
```bash
# Ubuntu/Debian
sudo apt install libssl-dev pkg-config

# CentOS/RHEL/Fedora
sudo dnf install openssl-devel pkg-config
```

#### **4. Build Tools Missing**
```bash
# Ubuntu/Debian
sudo apt install build-essential

# CentOS/RHEL/Fedora
sudo dnf groupinstall "Development Tools"
```

#### **5. Permission Issues**
```bash
# Fix cargo permissions
sudo chown -R $USER:$USER ~/.cargo

# Fix virtual environment permissions
sudo chown -R $USER:$USER .venv
```

### **Environment Variables**
```bash
# Required for local vocabulary files
export TIKTOKEN_ENCODINGS_BASE=vocab_files

# Optional: Rust compilation flags
export RUSTFLAGS="-C target-cpu=native"

# Optional: Cargo build parallelism
export CARGO_BUILD_JOBS=$(nproc)
```

## 🧪 **Testing**

### **Rust Tests**
```bash
# All tests
cargo test --release --no-default-features

# Specific test
cargo test --release --no-default-features test_name

# With output
cargo test --release --no-default-features -- --nocapture
```

### **Python Tests**
```bash
# Activate environment
source .venv/bin/activate

# All tests
pytest tests/

# Specific test file
pytest tests/test_harmony.py

# Verbose output
pytest tests/ -v
```

### **Integration Tests**
```bash
# Test both Rust and Python
./test_rust_linux.sh
./test_python_rust_linux.sh

# Or manually
cargo test --release --no-default-features
source .venv/bin/activate && pytest tests/
```

## 📦 **Distribution**

### **Create Python Wheel**
```bash
source .venv/bin/activate
export TIKTOKEN_ENCODINGS_BASE=vocab_files
maturin build --release --features python-binding --no-default-features
```

### **Install from Wheel**
```bash
pip install target/wheels/openai_harmony-*.whl
```

### **Development Installation**
```bash
source .venv/bin/activate
maturin develop --release --features python-binding --no-default-features
```

## 🎯 **Key Differences from Windows**

### **Package Managers**
- **Linux**: `apt`, `dnf`, `pacman` vs **Windows**: Manual downloads
- **Python**: `pip3` vs **Windows**: `pip`
- **Rust**: Same `rustup` and `cargo`

### **File Paths**
- **Linux**: `./script.sh`, `/usr/local/` vs **Windows**: `.\script.bat`, `C:\`
- **Virtual Environment**: `.venv/bin/activate` vs **Windows**: `.venv\Scripts\activate.bat`

### **Environment Variables**
- **Linux**: `export VAR=value` vs **Windows**: `set VAR=value`
- **PATH**: `:` separator vs **Windows**: `;` separator

### **Permissions**
- **Linux**: `chmod +x script.sh` vs **Windows**: No chmod needed
- **Sudo**: Required for system packages vs **Windows**: Administrator mode

## ✅ **Verification**

After successful build, verify installation:

```bash
# Test Rust library
cargo test --release --no-default-features

# Test Python bindings
source .venv/bin/activate
python3 -c "import openai_harmony; print('✅ Python bindings work!')"

# Test functionality
python3 -c "
import openai_harmony
encoding = openai_harmony.load_harmony_encoding('HarmonyGptOss')
print(f'✅ Encoding loaded: {encoding.name}')
tokens = encoding.encode('Hello, world!')
print(f'✅ Tokenization works: {len(tokens)} tokens')
"
```

This comprehensive setup ensures OpenAI Harmony builds successfully on Linux with both Rust and Python components.
