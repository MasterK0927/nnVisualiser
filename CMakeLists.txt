cmake_minimum_required(VERSION 3.16)

project(NeuralNetworkVisualizer 
    VERSION 1.0.0
    DESCRIPTION "Advanced Neural Network Visualization Application"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Enable SIMD optimizations if available
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
if(COMPILER_SUPPORTS_AVX2)
    add_compile_options(-mavx2)
    add_compile_definitions(USE_AVX2)
endif()

# Package management setup
option(USE_VCPKG "Use vcpkg for dependency management" ON)
option(USE_CONAN "Use Conan for dependency management" OFF)

if(USE_VCPKG)
    find_package(PkgConfig REQUIRED)
endif()

# Find required packages
find_package(sfml REQUIRED COMPONENTS system window graphics)
find_package(nlohmann_json REQUIRED)

# Optional packages
find_package(OpenCV QUIET)
find_package(yaml-cpp QUIET)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(external)
add_subdirectory(examples)

# Testing
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    endif()
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS NeuralNetworkVisualizer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# CPack configuration
include(CPack)
set(CPACK_PACKAGE_NAME "NeuralNetworkVisualizer")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Neural Network Visualizer Team")
