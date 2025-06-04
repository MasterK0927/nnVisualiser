# Building from Source

This guide covers building the Neural Network Visualizer from source code, including advanced build configurations, cross-platform considerations, and development setups.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Build System Overview](#build-system-overview)
- [Basic Build Process](#basic-build-process)
- [Build Configurations](#build-configurations)
- [Cross-Platform Building](#cross-platform-building)
- [Advanced Build Options](#advanced-build-options)
- [Continuous Integration](#continuous-integration)
- [Packaging and Distribution](#packaging-and-distribution)
- [Troubleshooting Build Issues](#troubleshooting-build-issues)

---

## Prerequisites

### Required Tools

```bash
# Core build tools
cmake --version          # 3.16 or later required
git --version           # For cloning and submodules
ninja --version         # Recommended build system (or make)

# Compiler support
g++ --version           # GCC 7+ (Linux)
clang++ --version       # Clang 6+ (Linux/macOS)
# MSVC 2017+ (Windows)

# Package managers
vcpkg version           # Recommended
# OR
conan --version         # Alternative
```

### System Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    libx11-dev \
    libxi-dev \
    libxrandr-dev \
    libxcursor-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

#### Fedora/CentOS/RHEL
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    ninja-build \
    git \
    pkgconfig \
    libX11-devel \
    libXi-devel \
    libXrandr-devel \
    libXcursor-devel \
    systemd-devel \
    mesa-libGL-devel \
    mesa-libGLU-devel
```

#### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Install dependencies via Homebrew
brew install cmake ninja pkg-config
```

#### Windows
- Visual Studio 2019 or later with C++ development tools
- CMake 3.16 or later
- Git for Windows

---

## Build System Overview

The Neural Network Visualizer uses CMake as its primary build system with the following structure:

```
Project Root
├── CMakeLists.txt              # Main CMake configuration
├── CMakePresets.json           # Build presets for different configurations
├── cmake/                      # CMake helper scripts
│   ├── FindSFML.cmake
│   ├── CompilerFlags.cmake
│   └── Dependencies.cmake
├── vcpkg.json                  # vcpkg dependency manifest
├── conanfile.txt              # Conan dependency manifest (alternative)
├── src/                       # Source code with per-module CMakeLists.txt
├── include/                   # Public headers
├── tests/                     # Unit tests with CMakeLists.txt
├── examples/                  # Example programs with CMakeLists.txt
└── external/                  # Third-party dependencies
```

### CMake Presets

The project includes several build presets for common scenarios:

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "default",
            "displayName": "Default Debug",
            "description": "Debug build with all features",
            "generator": "Ninja"
        },
        {
            "name": "release",
            "displayName": "Release",
            "description": "Optimized release build",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        },
        {
            "name": "vcpkg",
            "displayName": "vcpkg Dependencies",
            "description": "Build using vcpkg package manager",
            "toolchainFile": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        }
    ]
}
```

---

## Basic Build Process

### Step 1: Clone the Repository

```bash
# Clone with submodules
git clone --recursive https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

# If you forgot --recursive, initialize submodules
git submodule update --init --recursive
```

### Step 2: Set Up Dependencies

#### Option A: Using vcpkg (Recommended)

```bash
# Clone and bootstrap vcpkg
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg && ./bootstrap-vcpkg.sh

# Set environment variable
export VCPKG_ROOT=$HOME/vcpkg

# Dependencies will be installed automatically during CMake configure
```

#### Option B: Using System Packages

```bash
# Ubuntu/Debian
sudo apt-get install -y \
    libsfml-dev \
    libeigen3-dev \
    nlohmann-json3-dev \
    libspdlog-dev \
    libfmt-dev \
    libyaml-cpp-dev

# Build without package managers
cmake -B build -DUSE_VCPKG=OFF -DUSE_SYSTEM_LIBS=ON
```

### Step 3: Configure and Build

```bash
# Configure using preset
cmake --preset vcpkg

# Build
cmake --build build/vcpkg --config Release

# Install (optional)
sudo cmake --install build/vcpkg
```

### Step 4: Run Tests

```bash
# Run all tests
ctest --test-dir build/vcpkg --verbose

# Run specific test categories
ctest --test-dir build/vcpkg -L "core"
ctest --test-dir build/vcpkg -L "graphics"
```

---

## Build Configurations

### Debug Build

Optimized for development with debugging information:

```bash
cmake --preset debug
cmake --build build/debug

# Features enabled in debug mode:
# - Debug symbols (-g)
# - Assertions enabled
# - Slower optimization (-O0 or -O1)
# - Address sanitizer (optional)
# - Memory leak detection
```

### Release Build

Optimized for production deployment:

```bash
cmake --preset release
cmake --build build/release

# Features in release mode:
# - Full optimization (-O3)
# - No debug symbols (smaller binaries)
# - NDEBUG defined (disables assertions)
# - Link-time optimization (LTO)
# - Strip debug information
```

### Profile Build

Optimized with profiling information:

```bash
cmake --preset profile
cmake --build build/profile

# Features in profile mode:
# - Optimization with debug info (-O2 -g)
# - Profiling symbols
# - Performance counters enabled
```

### Sanitizer Builds

For detecting runtime errors during development:

```bash
# Address Sanitizer (detect memory errors)
cmake -B build/asan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"
cmake --build build/asan

# Thread Sanitizer (detect race conditions)
cmake -B build/tsan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=thread"
cmake --build build/tsan

# Memory Sanitizer (detect uninitialized memory)
cmake -B build/msan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=memory -fno-omit-frame-pointer"
cmake --build build/msan

# Undefined Behavior Sanitizer
cmake -B build/ubsan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=undefined"
cmake --build build/ubsan
```

---

## Cross-Platform Building

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update && sudo apt-get install -y $(cat scripts/ubuntu-deps.txt)

# Configure and build
cmake --preset vcpkg \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc
cmake --build build/vcpkg

# Create package
cpack --config build/vcpkg/CPackConfig.cmake
```

### Windows (Visual Studio)

```powershell
# Using Visual Studio Developer PowerShell
git clone https://github.com/keshav/nnVisualiser.git
cd nnVisualiser

# Set up vcpkg
$env:VCPKG_ROOT = "C:\vcpkg"

# Configure for Visual Studio 2019
cmake --preset vcpkg -G "Visual Studio 16 2019" -A x64

# Build
cmake --build build/vcpkg --config Release --parallel

# Create installer
cpack --config build/vcpkg/CPackConfig.cmake
```

### macOS

```bash
# Install Xcode command line tools
xcode-select --install

# Install dependencies
brew install cmake ninja pkg-config

# Configure and build
export VCPKG_ROOT=$HOME/vcpkg
cmake --preset vcpkg \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"  # Universal binary

cmake --build build/vcpkg

# Create .dmg package
cpack --config build/vcpkg/CPackConfig.cmake
```

### Cross-Compilation

#### Linux → Windows (MinGW)

```bash
# Install MinGW-w64
sudo apt-get install -y mingw-w64 wine

# Set up cross-compilation toolchain
cmake -B build/mingw \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/mingw-w64.cmake \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build/mingw
```

#### Linux → ARM (Raspberry Pi)

```bash
# Install cross-compilation toolchain
sudo apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# Configure for ARM
cmake -B build/arm \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-linux.cmake \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build/arm
```

---

## Advanced Build Options

### CMake Configuration Options

```bash
# Core features
-DBUILD_TESTS=ON/OFF              # Build unit tests (default: ON)
-DBUILD_EXAMPLES=ON/OFF           # Build example programs (default: ON)
-DBUILD_DOCS=ON/OFF               # Build documentation (default: OFF)
-DBUILD_BENCHMARKS=ON/OFF         # Build performance benchmarks (default: OFF)

# Dependencies
-DUSE_VCPKG=ON/OFF               # Use vcpkg package manager (default: ON)
-DUSE_CONAN=ON/OFF               # Use Conan package manager (default: OFF)
-DUSE_SYSTEM_LIBS=ON/OFF         # Use system-installed libraries (default: OFF)

# Graphics and UI
-DENABLE_GRAPHICS=ON/OFF         # Enable graphics/visualization (default: ON)
-DENABLE_SFML=ON/OFF            # Enable SFML graphics backend (default: ON)
-DENABLE_IMGUI=ON/OFF           # Enable ImGui interface (default: ON)

# Performance optimizations
-DENABLE_SIMD=ON/OFF            # Enable SIMD optimizations (default: ON)
-DENABLE_OPENMP=ON/OFF          # Enable OpenMP parallelization (default: ON)
-DENABLE_CUDA=ON/OFF            # Enable CUDA GPU acceleration (default: OFF)
-DENABLE_OPENCL=ON/OFF          # Enable OpenCL GPU acceleration (default: OFF)

# Development options
-DENABLE_CLANG_TIDY=ON/OFF      # Run clang-tidy static analysis (default: OFF)
-DENABLE_CPPCHECK=ON/OFF        # Run cppcheck static analysis (default: OFF)
-DENABLE_COVERAGE=ON/OFF        # Generate code coverage reports (default: OFF)
-DENABLE_PROFILING=ON/OFF       # Enable profiling instrumentation (default: OFF)

# Security features
-DENABLE_HARDENING=ON/OFF       # Enable security hardening flags (default: ON in Release)
-DENABLE_STACK_PROTECTION=ON/OFF # Enable stack protection (default: ON)
```

### Custom Build Example

```bash
# High-performance build with GPU support
cmake -B build/gpu \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_CUDA=ON \
    -DENABLE_SIMD=ON \
    -DENABLE_OPENMP=ON \
    -DCMAKE_CXX_FLAGS="-march=native -mtune=native" \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# Development build with all analysis tools
cmake -B build/dev \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_CLANG_TIDY=ON \
    -DENABLE_CPPCHECK=ON \
    -DENABLE_COVERAGE=ON \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined"

# Minimal build for embedded systems
cmake -B build/minimal \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DENABLE_GRAPHICS=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DCMAKE_CXX_FLAGS="-Os -ffunction-sections -fdata-sections" \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,--gc-sections"
```

### Compiler-Specific Optimizations

#### GCC
```bash
cmake -B build/gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-march=native -mtune=native -flto -fuse-linker-plugin"
```

#### Clang
```bash
cmake -B build/clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-march=native -mtune=native -flto=thin"
```

#### MSVC
```powershell
cmake -B build/msvc `
    -G "Visual Studio 16 2019" `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_CXX_FLAGS="/arch:AVX2 /GL /Gy"
```

---

## Continuous Integration

### GitHub Actions Configuration

The project includes CI/CD pipelines for automated building and testing:

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-linux:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        compiler: [gcc-9, gcc-10, clang-10, clang-11]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y ninja-build libx11-dev libxi-dev libxrandr-dev libxcursor-dev libudev-dev libgl1-mesa-dev

    - name: Set up vcpkg
      uses: lukka/run-vcpkg@v10

    - name: Configure
      run: cmake --preset vcpkg -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}

    - name: Build
      run: cmake --build build/vcpkg --config ${{ matrix.build_type }}

    - name: Test
      run: ctest --test-dir build/vcpkg --config ${{ matrix.build_type }}

  build-windows:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive

    - name: Set up vcpkg
      uses: lukka/run-vcpkg@v10

    - name: Configure
      run: cmake --preset vcpkg -G "Visual Studio 17 2022"

    - name: Build
      run: cmake --build build/vcpkg --config Release

    - name: Test
      run: ctest --test-dir build/vcpkg --config Release

  build-macos:
    runs-on: macos-latest
    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive

    - name: Install dependencies
      run: brew install ninja

    - name: Set up vcpkg
      uses: lukka/run-vcpkg@v10

    - name: Configure
      run: cmake --preset vcpkg

    - name: Build
      run: cmake --build build/vcpkg --config Release

    - name: Test
      run: ctest --test-dir build/vcpkg --config Release
```

### Local CI Testing

You can run CI-like builds locally:

```bash
# Install act (GitHub Actions runner)
curl https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash

# Run GitHub Actions locally
act -j build-linux

# Or use Docker for consistent environments
docker run -v $(pwd):/workspace -w /workspace ubuntu:20.04 bash -c "
  apt-get update && apt-get install -y build-essential cmake ninja-build git
  cmake --preset vcpkg
  cmake --build build/vcpkg
  ctest --test-dir build/vcpkg
"
```

---

## Packaging and Distribution

### Creating Packages

#### DEB Package (Ubuntu/Debian)
```bash
# Install packaging tools
sudo apt-get install -y dpkg-dev devscripts

# Configure with packaging support
cmake --preset release -DCPACK_GENERATOR="DEB"
cmake --build build/release

# Create package
cpack --config build/release/CPackConfig.cmake

# Install package
sudo dpkg -i build/release/neural-network-visualizer_*.deb
```

#### RPM Package (Fedora/CentOS)
```bash
# Install packaging tools
sudo dnf install -y rpm-build rpm-devel

# Configure and build
cmake --preset release -DCPACK_GENERATOR="RPM"
cmake --build build/release

# Create package
cpack --config build/release/CPackConfig.cmake
```

#### Windows Installer
```powershell
# Install NSIS (Nullsoft Scriptable Install System)
# Download from https://nsis.sourceforge.io/

# Configure and build
cmake --preset vcpkg -DCPACK_GENERATOR="NSIS"
cmake --build build/vcpkg --config Release

# Create installer
cpack --config build/vcpkg/CPackConfig.cmake
```

#### macOS Bundle
```bash
# Configure for macOS bundle
cmake --preset vcpkg -DCPACK_GENERATOR="Bundle;DragNDrop"
cmake --build build/vcpkg --config Release

# Create .dmg
cpack --config build/vcpkg/CPackConfig.cmake
```

### AppImage (Linux)

```bash
# Download linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage

# Build and install to temporary directory
cmake --preset release -DCMAKE_INSTALL_PREFIX=/tmp/nnvis-appdir/usr
cmake --build build/release
cmake --install build/release

# Create AppImage
./linuxdeploy-x86_64.AppImage --appdir /tmp/nnvis-appdir --output appimage
```

### Flatpak Package

```yaml
# flatpak/dev.nnvisualizer.NeuralNetworkVisualizer.yml
app-id: dev.nnvisualizer.NeuralNetworkVisualizer
runtime: org.freedesktop.Platform
runtime-version: '22.08'
sdk: org.freedesktop.Sdk
command: neural-network-visualizer

modules:
  - name: neural-network-visualizer
    buildsystem: cmake-ninja
    config-opts:
      - -DCMAKE_BUILD_TYPE=Release
      - -DUSE_SYSTEM_LIBS=ON
    sources:
      - type: git
        url: https://github.com/keshav/nnVisualiser.git
        tag: v1.0.0
```

```bash
# Build Flatpak
flatpak-builder build-dir flatpak/dev.nnvisualizer.NeuralNetworkVisualizer.yml --force-clean
flatpak-builder --repo=repo --force-clean build-dir flatpak/dev.nnvisualizer.NeuralNetworkVisualizer.yml

# Install locally
flatpak --user remote-add --no-gpg-verify local-repo repo
flatpak --user install local-repo dev.nnvisualizer.NeuralNetworkVisualizer
```

---

## Troubleshooting Build Issues

### Common CMake Issues

#### Issue 1: CMake Version Too Old
```bash
# Error: CMake 3.16 or higher is required
# Solution: Update CMake
pip install --upgrade cmake
# Or download from https://cmake.org/download/
```

#### Issue 2: Generator Not Found
```bash
# Error: CMake was unable to find a build program corresponding to "Ninja"
# Solution: Install ninja
sudo apt-get install ninja-build  # Ubuntu/Debian
brew install ninja                # macOS
# Windows: Download from https://github.com/ninja-build/ninja/releases
```

#### Issue 3: vcpkg Toolchain Not Found
```bash
# Error: CMAKE_TOOLCHAIN_FILE not found
# Solution: Set VCPKG_ROOT environment variable
export VCPKG_ROOT=/path/to/vcpkg
# Or specify explicitly
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Dependency Issues

#### Issue 1: SFML Not Found
```bash
# Error: Could not find SFML
# Solution: Install system dependencies or check vcpkg installation
sudo apt-get install libsfml-dev  # System installation
# OR
$VCPKG_ROOT/vcpkg install sfml     # vcpkg installation
```

#### Issue 2: OpenGL Issues
```bash
# Error: OpenGL not found or version too old
# Solution: Install Mesa development packages
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev

# Check OpenGL version
glxinfo | grep "OpenGL version"

# Update graphics drivers if needed
```

#### Issue 3: Package Version Conflicts
```bash
# Error: Version conflict between packages
# Solution: Clean vcpkg cache and reinstall
rm -rf $VCPKG_ROOT/buildtrees
rm -rf $VCPKG_ROOT/packages
$VCPKG_ROOT/vcpkg install --recurse
```

### Compilation Issues

#### Issue 1: C++17 Not Supported
```bash
# Error: C++17 features not available
# Solution: Update compiler or specify version explicitly
sudo apt-get install gcc-9 g++-9  # Install newer GCC
cmake -DCMAKE_CXX_COMPILER=g++-9  # Use specific version
```

#### Issue 2: Missing Headers
```bash
# Error: fatal error: 'some_header.h' file not found
# Solution: Install development packages
sudo apt-get install libx11-dev libxi-dev libxrandr-dev libxcursor-dev

# Or check include paths
cmake -DCMAKE_VERBOSE_MAKEFILE=ON
```

#### Issue 3: Linking Errors
```bash
# Error: undefined reference to 'symbol'
# Solution: Check library linking order and dependencies
cmake -DCMAKE_VERBOSE_MAKEFILE=ON
# Review link.txt files in build directory
```

### Runtime Issues

#### Issue 1: Shared Library Not Found
```bash
# Error: error while loading shared libraries
# Solution: Set LD_LIBRARY_PATH or install to system
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/libs
# Or
sudo ldconfig /path/to/libs
```

#### Issue 2: Graphics Context Issues
```bash
# Error: Failed to create OpenGL context
# Solution: Check graphics drivers and X11 forwarding
# For SSH with X11 forwarding:
ssh -X username@hostname

# Check graphics capabilities:
glxinfo | head -20
```

### Performance Issues

#### Issue 1: Slow Compilation
```bash
# Solution: Use parallel compilation
cmake --build build/vcpkg --parallel $(nproc)

# Enable compiler cache
export CCACHE_DIR=$HOME/.ccache
cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

#### Issue 2: Large Binary Size
```bash
# Solution: Enable optimizations and stripping
cmake -DCMAKE_BUILD_TYPE=Release
cmake -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
strip build/release/src/neural-network-visualizer
```

### Debug Build Issues

For complex build issues, enable verbose output:

```bash
# CMake verbose configuration
cmake --preset vcpkg --debug-output

# Make verbose build
cmake --build build/vcpkg --verbose

# Detailed error information
VERBOSE=1 make -C build/vcpkg

# Debug CMake cache
cmake -LAH build/vcpkg  # List all variables
```

---

## Build Scripts

For convenience, the project includes build scripts:

### Linux/macOS Build Script
```bash
#!/bin/bash
# scripts/build.sh

set -e

BUILD_TYPE=${1:-Release}
USE_VCPKG=${USE_VCPKG:-ON}
NUM_JOBS=${NUM_JOBS:-$(nproc)}

echo "Building Neural Network Visualizer"
echo "Build type: $BUILD_TYPE"
echo "Using vcpkg: $USE_VCPKG"
echo "Parallel jobs: $NUM_JOBS"

# Configure
if [ "$USE_VCPKG" = "ON" ]; then
    cmake --preset vcpkg -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    BUILD_DIR="build/vcpkg"
else
    cmake -B build/system -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUSE_SYSTEM_LIBS=ON
    BUILD_DIR="build/system"
fi

# Build
cmake --build $BUILD_DIR --config $BUILD_TYPE --parallel $NUM_JOBS

# Test
if [ "$BUILD_TYPE" = "Debug" ] || [ "$BUILD_TYPE" = "RelWithDebInfo" ]; then
    echo "Running tests..."
    ctest --test-dir $BUILD_DIR --config $BUILD_TYPE --parallel $NUM_JOBS
fi

echo "Build completed successfully!"
echo "Binary location: $BUILD_DIR/src/neural-network-visualizer"
```

### Usage
```bash
# Default release build with vcpkg
./scripts/build.sh

# Debug build
./scripts/build.sh Debug

# System libraries instead of vcpkg
USE_VCPKG=OFF ./scripts/build.sh

# Specify job count
NUM_JOBS=4 ./scripts/build.sh Release
```

This comprehensive build guide should help you successfully build the Neural Network Visualizer from source on any supported platform, with various configuration options for different use cases.