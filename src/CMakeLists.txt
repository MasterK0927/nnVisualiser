# Core library
add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(ui)
add_subdirectory(utils)

# Main executable
set(MAIN_SOURCES
    main.cpp
)

add_executable(NeuralNetworkVisualizer ${MAIN_SOURCES})

target_link_libraries(NeuralNetworkVisualizer
    PRIVATE
        nnv_core
        nnv_graphics
        nnv_ui
        nnv_utils
        sfml-system
        sfml-window
        sfml-graphics
        nlohmann_json::nlohmann_json
)

if(yaml-cpp_FOUND)
    target_link_libraries(NeuralNetworkVisualizer PRIVATE yaml-cpp)
    target_compile_definitions(NeuralNetworkVisualizer PRIVATE HAS_YAML_CPP)
endif()

if(OpenCV_FOUND)
    target_link_libraries(NeuralNetworkVisualizer PRIVATE ${OpenCV_LIBS})
    target_compile_definitions(NeuralNetworkVisualizer PRIVATE HAS_OPENCV)
endif()

# Set target properties
set_target_properties(NeuralNetworkVisualizer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "nnvisualizer"
)

# Include directories
target_include_directories(NeuralNetworkVisualizer
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
)
