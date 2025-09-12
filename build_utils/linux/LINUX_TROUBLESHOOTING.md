# Linux Build Troubleshooting Guide

Comprehensive troubleshooting solutions for building OpenAI Harmony on Linux systems.

## 🚨 **Common Build Issues**

### **1. Rust Installation Problems**

#### **Permission Denied Error**
```
error: could not amend shell profile: '/usr2/oferr/.profile': could not write rcfile file: '/usr2/oferr/.profile': Permission denied (os error 13)
```

**Solution: Manual Installation with Custom Directory**
```bash
# Download rustup installer
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs -o rustup-init.sh

# Run installer with custom options (no profile modification)
chmod +x rustup-init.sh
./rustup-init.sh --no-modify-path

# Manually add to PATH for current session
export PATH="$HOME/.cargo/bin:$PATH"

# Verify installation
rustc --version
cargo --version
```

#### **Disk Quota Exceeded Error**
```
unable to sync download to disk: Disk quota exceeded (os error 122)
```

**Solution: Use System Package Manager**
```bash
# Ubuntu/Debian
sudo apt update && sudo apt install rustc cargo

# CentOS/RHEL/Fedora
sudo dnf install rust cargo

# Verify installation
rustc --version
cargo --version
```

### **2. Missing Build Dependencies**

#### **Build Tools Missing**
```bash
# Error: "gcc not found" or "make not found"
# Solution:
sudo apt install build-essential  # Ubuntu/Debian
sudo dnf groupinstall "Development Tools"  # CentOS/RHEL/Fedora
sudo pacman -S base-devel  # Arch Linux
```

#### **Python Development Headers Missing**
```bash
# Error: "Python.h not found"
# Solution:
sudo apt install python3-dev  # Ubuntu/Debian
sudo dnf install python3-devel  # CentOS/RHEL/Fedora
sudo pacman -S python  # Arch Linux (includes headers)
```

#### **OpenSSL Development Libraries Missing**
```bash
# Error: "openssl not found" or "failed to run custom build command for openssl-sys"
# Solution:
sudo apt install libssl-dev pkg-config  # Ubuntu/Debian
sudo dnf install openssl-devel pkg-config  # CentOS/RHEL/Fedora
sudo pacman -S openssl pkg-config  # Arch Linux
```

### **3. Script Permission Issues**

#### **Permission Denied When Running Scripts**
```bash
# Error: "./download_vocab_files_linux.sh: Permission denied"
# Solution:
chmod +x *.sh
./download_vocab_files_linux.sh
```

#### **Alternative Script Execution Methods**
```bash
# Method 1: Direct execution (requires chmod +x)
chmod +x download_vocab_files_linux.sh
./download_vocab_files_linux.sh

# Method 2: Using bash (no chmod needed)
bash download_vocab_files_linux.sh

# Method 3: Using source (no chmod needed)
source download_vocab_files_linux.sh
```

## 🔧 **Advanced Rust Installation Solutions**

### **Custom Rust Installation Directory**
```bash
# Set custom Rust installation directory
export RUSTUP_HOME="$HOME/rust-toolchain"
export CARGO_HOME="$HOME/.cargo"

# Install Rust to custom location
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- --no-modify-path

# Add to PATH
export PATH="$CARGO_HOME/bin:$PATH"

# Verify
rustc --version
```

### **Temporary Installation (For Disk Quota Issues)**
```bash
# Install to /tmp (temporary, will be lost on reboot)
export RUSTUP_HOME="/tmp/rust-$(whoami)"
export CARGO_HOME="/tmp/cargo-$(whoami)"
mkdir -p "$RUSTUP_HOME" "$CARGO_HOME"

# Download and install
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs -o /tmp/rustup-init.sh
chmod +x /tmp/rustup-init.sh
/tmp/rustup-init.sh --no-modify-path

# Add to PATH
export PATH="$CARGO_HOME/bin:$PATH"
```

### **Using Existing System Rust**
```bash
# Check if Rust is already available
which rustc
which cargo

# Check version (need 1.60+ minimum, 1.70+ recommended)
rustc --version
cargo --version

# If available and sufficient version, skip Rust installation
```

## 🧹 **Disk Space Management**

### **Check Disk Usage**
```bash
# Check your home directory usage
du -sh ~/*

# Check current directory usage
du -sh ./*

# Check available space
df -h ~

# Check disk quota (if available)
quota -u $(whoami) 2>/dev/null || echo "Quota tools not available"
```

### **Clean Up Space**
```bash
# Clean temporary files
rm -rf ~/.cache/* 2>/dev/null
rm -rf /tmp/tmp.* 2>/dev/null
rm -rf ~/.local/share/Trash/* 2>/dev/null

# Clean existing Rust installations
rm -rf ~/.cargo 2>/dev/null
rm -rf ~/.rustup 2>/dev/null

# Clean build artifacts
find ~ -name "target" -type d -exec rm -rf {} + 2>/dev/null
find ~ -name "node_modules" -type d -exec rm -rf {} + 2>/dev/null

# Clean package caches
pip cache purge 2>/dev/null
```

## 🐛 **Build Process Troubleshooting**

### **Vocabulary Files Download Issues**
```bash
# Error: "Failed to download vocabulary files"
# Check internet connection
curl -I https://openaipublic.blob.core.windows.net/encodings/o200k_base.tiktoken

# Manual download
mkdir -p vocab_files
curl -L -o vocab_files/o200k_base.tiktoken \
  "https://openaipublic.blob.core.windows.net/encodings/o200k_base.tiktoken"
curl -L -o vocab_files/cl100k_base.tiktoken \
  "https://openaipublic.blob.core.windows.net/encodings/cl100k_base.tiktoken"
```

### **Rust Compilation Issues**
```bash
# Error: "linker `cc` not found"
sudo apt install gcc  # Ubuntu/Debian
sudo dnf install gcc  # CentOS/RHEL/Fedora

# Error: "failed to run custom build command"
# Set environment for local vocab files
export TIKTOKEN_ENCODINGS_BASE=vocab_files

# Build without reqwest dependency
cargo build --release --no-default-features
```

### **Python Virtual Environment Issues**
```bash
# Error: "No module named 'venv'"
sudo apt install python3-venv  # Ubuntu/Debian
sudo dnf install python3-venv  # CentOS/RHEL/Fedora

# Error: "pip not found"
sudo apt install python3-pip  # Ubuntu/Debian
sudo dnf install python3-pip  # CentOS/RHEL/Fedora

# Error: "maturin not found"
source .venv/bin/activate
pip install --upgrade pip
pip install maturin>=1.8,<2.0
```

## 📝 **Environment Setup Script**

Create a comprehensive setup script for your environment:

```bash
# Create setup-env.sh
cat > setup-env.sh << 'EOF'
#!/bin/bash
# OpenAI Harmony Environment Setup

echo "Setting up OpenAI Harmony build environment..."

# Add Rust to PATH if installed via rustup
if [ -d "$HOME/.cargo/bin" ]; then
    export PATH="$HOME/.cargo/bin:$PATH"
    echo "✅ Rust found in ~/.cargo/bin"
elif command -v rustc >/dev/null 2>&1; then
    echo "✅ System Rust found"
else
    echo "❌ Rust not found - please install Rust first"
    exit 1
fi

# Set vocabulary files location
export TIKTOKEN_ENCODINGS_BASE=vocab_files
echo "✅ Vocabulary files location set"

# Optional: Rust compilation optimizations
export RUSTFLAGS="-C target-cpu=native"
export CARGO_BUILD_JOBS=$(nproc)
echo "✅ Rust optimization flags set"

# Verify tools
echo ""
echo "Environment Status:"
echo "Rust: $(rustc --version 2>/dev/null || echo 'Not found')"
echo "Cargo: $(cargo --version 2>/dev/null || echo 'Not found')"
echo "Python: $(python3 --version 2>/dev/null || echo 'Not found')"
echo "GCC: $(gcc --version 2>/dev/null | head -1 || echo 'Not found')"

echo ""
echo "Environment configured for OpenAI Harmony build"
EOF

# Make executable and use
chmod +x setup-env.sh
source setup-env.sh
```

## 🔍 **Diagnostic Commands**

### **System Information**
```bash
# Check OS and architecture
uname -a
lsb_release -a 2>/dev/null || cat /etc/os-release

# Check available tools
which gcc g++ make cmake pkg-config
which python3 pip3
which rustc cargo

# Check versions
gcc --version 2>/dev/null | head -1
python3 --version
rustc --version 2>/dev/null
cargo --version 2>/dev/null
```

### **Build Environment Check**
```bash
# Check required environment variables
echo "TIKTOKEN_ENCODINGS_BASE: $TIKTOKEN_ENCODINGS_BASE"
echo "PATH: $PATH"
echo "RUSTFLAGS: $RUSTFLAGS"
echo "CARGO_BUILD_JOBS: $CARGO_BUILD_JOBS"

# Check vocabulary files
ls -la vocab_files/ 2>/dev/null || echo "Vocabulary files not found"

# Test basic compilation
echo 'fn main() { println!("Hello, Rust!"); }' > test.rs
rustc test.rs && ./test && rm test test.rs
```

## ✅ **Complete Troubleshooting Workflow**

### **Step 1: Verify System Dependencies**
```bash
# Check and install build tools
sudo apt install build-essential pkg-config libssl-dev python3 python3-pip python3-venv python3-dev curl wget git

# Or for CentOS/RHEL/Fedora:
sudo dnf groupinstall "Development Tools"
sudo dnf install pkg-config openssl-devel python3 python3-pip python3-devel curl wget git
```

### **Step 2: Install Rust (Choose Best Method)**
```bash
# Method 1: System package manager (recommended for quota issues)
sudo apt install rustc cargo  # Ubuntu/Debian
sudo dnf install rust cargo   # CentOS/RHEL/Fedora

# Method 2: Rustup (if no permission/quota issues)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Method 3: Manual rustup (for permission issues)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs -o rustup-init.sh
chmod +x rustup-init.sh
./rustup-init.sh --no-modify-path
export PATH="$HOME/.cargo/bin:$PATH"
```

### **Step 3: Verify Installation**
```bash
rustc --version  # Should be 1.60.0+ (1.70.0+ recommended)
cargo --version
python3 --version  # Should be 3.8+
gcc --version
```

### **Step 4: Run Build Process**
```bash
# Make scripts executable
chmod +x *.sh

# Set environment
export TIKTOKEN_ENCODINGS_BASE=vocab_files

# Run build sequence
./download_vocab_files_linux.sh
./install_python_dependencies_linux.sh
./build_rust_linux.sh
./build_python_rust_linux.sh
./test_rust_linux.sh
./test_python_rust_linux.sh
```

## 🎉 **Summary**

**Most common issues and solutions:**

1. **Rust permission errors** → Use `--no-modify-path` and manual PATH setup
2. **Disk quota exceeded** → Use system package manager instead of rustup
3. **Missing build tools** → Install build-essential/Development Tools
4. **Script permissions** → Run `chmod +x *.sh` first
5. **Missing headers** → Install python3-dev and libssl-dev

**For persistent issues, contact your system administrator or use the diagnostic commands above to gather information for support.**
