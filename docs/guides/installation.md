# Installation Guide

This guide provides detailed instructions for installing and setting up the Neural Network Visualizer on various platforms.

## Table of Contents

- [System Requirements](#system-requirements)
- [Quick Installation](#quick-installation)
- [Platform-Specific Instructions](#platform-specific-instructions)
  - [Ubuntu/Debian](#ubuntudebian)
  - [Windows](#windows)
  - [macOS](#macos)
  - [Arch Linux](#arch-linux)
- [Building from Source](#building-from-source)
- [Dependency Management](#dependency-management)
- [Troubleshooting](#troubleshooting)
- [Verification](#verification)

---

## System Requirements

### Minimum Requirements

- **Operating System**:
  - Linux (Ubuntu 18.04+, Debian 10+, Arch Linux)
  - Windows 10 (version 1903+)
  - macOS 10.15 (Catalina) or later
- **Compiler**: C++17 support required
  - GCC 7.0+ or Clang 6.0+ (Linux/macOS)
  - MSVC 2017+ (Windows)
- **Memory**: 4 GB RAM minimum, 8 GB recommended
- **Storage**: 2 GB free disk space
- **Graphics**: OpenGL 3.3 support

### Recommended Requirements

- **CPU**: Multi-core processor (4+ cores recommended)
- **Memory**: 16 GB RAM for large networks
- **Graphics**: Dedicated GPU with OpenGL 4.0+ support
- **Storage**: SSD for better performance

### Required Software

- **CMake**: 3.16 or later
- **Git**: For cloning the repository
- **vcpkg**: Package manager (will be set up automatically)

---

## Quick Installation

For most users, the quick installation process will work:

```bash
# Clone the repository
git clone https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

# Install system dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y build-essential cmake git pkg-config \
    libx11-dev libxi-dev libxrandr-dev libxcursor-dev libudev-dev \
    libgl1-mesa-dev libglu1-mesa-dev

# Set up vcpkg (if not already installed)
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg && ./bootstrap-vcpkg.sh
export VCPKG_ROOT=$HOME/vcpkg

# Build the project
cd ~/nnVisualiser
cmake --preset vcpkg
cmake --build build/vcpkg --config Release

# Run the application
./build/vcpkg/src/NeuralNetworkVisualizer
```

---

## Platform-Specific Instructions

### Ubuntu/Debian

#### Step 1: Install System Dependencies

```bash
# Update package manager
sudo apt-get update

# Install build tools
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    ninja-build

# Install graphics and system libraries
sudo apt-get install -y \
    libx11-dev \
    libxi-dev \
    libxrandr-dev \
    libxcursor-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libopengl0 \
    libglfw3-dev

# Optional: Install additional development tools
sudo apt-get install -y \
    valgrind \
    gdb \
    clang-format \
    doxygen \
    graphviz
```

#### Step 2: Set Up vcpkg

```bash
# Clone vcpkg to a permanent location
cd $HOME
git clone https://github.com/Microsoft/vcpkg.git

# Bootstrap vcpkg
cd vcpkg
./bootstrap-vcpkg.sh

# Add vcpkg to your environment
echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.bashrc
echo 'export PATH=$VCPKG_ROOT:$PATH' >> ~/.bashrc
source ~/.bashrc
```

#### Step 3: Build Neural Network Visualizer

```bash
# Clone the project
git clone https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

# Configure and build
cmake --preset vcpkg
cmake --build build/vcpkg --config Release -j$(nproc)

# Optional: Build in debug mode for development
cmake --preset debug
cmake --build build/debug --config Debug -j$(nproc)
```

---

### Windows

#### Prerequisites

1. **Visual Studio 2019 or later** with C++ development tools
2. **Git for Windows**
3. **CMake** (3.16+) - Can be installed via Visual Studio Installer

#### Step 1: Install vcpkg

Open PowerShell as Administrator:

```powershell
# Navigate to C: drive
cd C:\

# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git

# Bootstrap vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat

# Set environment variable
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "User")

# Refresh environment
$env:VCPKG_ROOT = "C:\vcpkg"
```

#### Step 2: Build the Project

```powershell
# Clone the project
git clone https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

# Configure with vcpkg
cmake --preset vcpkg

# Build the project
cmake --build build/vcpkg --config Release
```

#### Alternative: Using Visual Studio

1. Open Visual Studio
2. Select "Clone a repository"
3. Enter: `https://github.com/keshav/nnVisualiser.git`
4. Open the folder
5. Select the vcpkg preset from the configuration dropdown
6. Build -> Build All

---

### macOS

#### Step 1: Install Xcode Command Line Tools

```bash
xcode-select --install
```

#### Step 2: Install Homebrew (if not already installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### Step 3: Install Dependencies

```bash
# Install build tools
brew install cmake ninja pkg-config

# Install system libraries
brew install glfw3 opengl

# Optional: Install development tools
brew install llvm doxygen graphviz
```

#### Step 4: Set Up vcpkg

```bash
# Clone vcpkg
cd ~
git clone https://github.com/Microsoft/vcpkg.git

# Bootstrap vcpkg
cd vcpkg
./bootstrap-vcpkg.sh

# Add to shell profile
echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.zshrc
source ~/.zshrc
```

#### Step 5: Build the Project

```bash
# Clone and build
git clone https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

cmake --preset vcpkg
cmake --build build/vcpkg --config Release -j$(sysctl -n hw.ncpu)
```

---

### Arch Linux

#### Step 1: Install Dependencies

```bash
# Update system
sudo pacman -Syu

# Install build tools
sudo pacman -S base-devel cmake ninja git pkg-config

# Install graphics libraries
sudo pacman -S libx11 libxi libxrandr libxcursor mesa glfw-x11

# Optional development tools
sudo pacman -S valgrind gdb clang doxygen graphviz
```

#### Step 2: Install vcpkg

```bash
# Clone vcpkg
cd ~
git clone https://github.com/Microsoft/vcpkg.git

# Bootstrap
cd vcpkg
./bootstrap-vcpkg.sh

# Add to environment
echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.bashrc
source ~/.bashrc
```

#### Step 3: Build Project

```bash
git clone https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

cmake --preset vcpkg
cmake --build build/vcpkg --config Release -j$(nproc)
```

---

## Building from Source

### Development Build

For development with debugging symbols and faster compilation:

```bash
# Configure for debug
cmake --preset debug

# Build with debug info
cmake --build build/debug --config Debug

# Enable address sanitizer (optional)
cmake --preset debug -DCMAKE_CXX_FLAGS="-fsanitize=address"
cmake --build build/debug --config Debug
```

### Release Build

For optimized production builds:

```bash
# Configure for release
cmake --preset release

# Build optimized version
cmake --build build/release --config Release

# Optional: Enable link-time optimization
cmake --preset release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
cmake --build build/release --config Release
```

### Custom Build Configuration

```bash
# Configure with custom options
cmake -B build/custom \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DUSE_CUDA=OFF \
    -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build/custom --config Release
```

---

## Dependency Management

### vcpkg Dependencies

The project automatically manages these dependencies through vcpkg:

- **SFML**: Graphics and windowing
- **ImGui**: User interface
- **Eigen3**: Linear algebra
- **nlohmann/json**: JSON parsing
- **spdlog**: Logging
- **fmt**: String formatting
- **yaml-cpp**: YAML configuration

### Manual Dependency Installation

If you prefer to manage dependencies manually:

#### Ubuntu/Debian

```bash
sudo apt-get install -y \
    libsfml-dev \
    libeigen3-dev \
    nlohmann-json3-dev \
    libspdlog-dev \
    libfmt-dev \
    libyaml-cpp-dev
```

#### Disable vcpkg

```bash
cmake -B build/system \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_VCPKG=OFF \
    -DUSE_SYSTEM_LIBS=ON

cmake --build build/system --config Release
```

---

## Troubleshooting

### Common Issues and Solutions

#### 1. vcpkg Not Found

**Error**: `CMake Error: CMAKE_TOOLCHAIN_FILE not found`

**Solution**:
```bash
# Ensure VCPKG_ROOT is set correctly
echo $VCPKG_ROOT

# If empty, set it:
export VCPKG_ROOT=/path/to/your/vcpkg

# Verify vcpkg works
$VCPKG_ROOT/vcpkg version
```

#### 2. Missing System Libraries

**Error**: `fatal error: GL/gl.h: No such file or directory`

**Solution** (Ubuntu/Debian):
```bash
sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
```

#### 3. Compiler Not Found

**Error**: `No CMAKE_CXX_COMPILER could be found`

**Solution**:
```bash
# Install compiler
sudo apt-get install -y build-essential

# Or specify compiler explicitly
cmake --preset vcpkg -DCMAKE_CXX_COMPILER=g++
```

#### 4. Ninja Not Found

**Error**: `CMake was unable to find a build program corresponding to "Ninja"`

**Solution**:
```bash
# Install ninja
sudo apt-get install -y ninja-build

# Or use make instead
cmake --preset vcpkg -G "Unix Makefiles"
```

#### 5. SFML Build Errors

**Error**: SFML compilation fails with system library errors

**Solution**:
```bash
# Install SFML system dependencies
sudo apt-get install -y \
    libx11-dev \
    libxi-dev \
    libxrandr-dev \
    libxcursor-dev \
    libudev-dev \
    libgl1-mesa-dev

# Clear vcpkg cache and rebuild
rm -rf build/
cmake --preset vcpkg
```

#### 6. Permission Errors

**Error**: Permission denied when building

**Solution**:
```bash
# Ensure you have write permissions to build directory
chmod -R 755 build/

# Or build in a different location
cmake -B ~/build/nnVisualiser --preset vcpkg
cmake --build ~/build/nnVisualiser --config Release
```

### Debugging Build Issues

#### Enable Verbose Output

```bash
# CMake verbose output
cmake --preset vcpkg --verbose

# Build verbose output
cmake --build build/vcpkg --config Release --verbose

# vcpkg verbose output
cmake --preset vcpkg -DVCPKG_VERBOSE=ON
```

#### Check Dependencies

```bash
# List vcpkg installed packages
$VCPKG_ROOT/vcpkg list

# Check for specific package
$VCPKG_ROOT/vcpkg list | grep sfml

# Reinstall problematic package
$VCPKG_ROOT/vcpkg remove sfml
$VCPKG_ROOT/vcpkg install sfml
```

---

## Verification

### Test Installation

After successful compilation, verify your installation:

```bash
# Run the main application
./build/vcpkg/src/NeuralNetworkVisualizer

# Run unit tests (if built)
ctest --test-dir build/vcpkg --verbose

# Run example programs
./build/vcpkg/examples/simple_xor_demo
./build/vcpkg/examples/complete_demo
```

### Performance Check

```bash
# Build and run performance tests
cmake --build build/vcpkg --target benchmark
./build/vcpkg/tests/benchmark
```

### Expected Output

The application should start and display:
- Main window with neural network visualization area
- Menu bar with File, Edit, View, and Help options
- Control panels for network configuration
- Status bar showing application information

---

## Next Steps

After successful installation:

1. **Read the [Getting Started Guide](getting-started.md)** to learn basic usage
2. **Explore [Tutorials](../tutorials/)** for hands-on learning
3. **Check out [Examples](../../examples/)** for practical implementations
4. **Review [API Documentation](../api/)** for development

---

## Getting Help

If you encounter issues not covered in this guide:

1. **Check existing [GitHub Issues](https://github.com/keshav/nnVisualiser/issues)**
2. **Create a new issue** with:
   - Your operating system and version
   - Compiler version
   - Complete error messages
   - Steps to reproduce the problem
3. **Join our [Discord community](https://discord.gg/nnvisualizer)** for real-time help

---

**Note**: This installation guide is regularly updated. If you find any outdated information or have suggestions for improvement, please [contribute](contributing.md) to the documentation.