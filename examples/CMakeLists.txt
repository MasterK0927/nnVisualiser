# Examples for Neural Network Visualizer

# XOR Demo
add_executable(xor_demo simple_xor_demo.cpp)

target_link_libraries(xor_demo
    PRIVATE
        nnv_core
        nnv_utils
        nlohmann_json::nlohmann_json
)

target_include_directories(xor_demo
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(xor_demo PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "xor_demo"
)

# Data Loading Demo (when implemented)
# add_executable(data_demo data_loading_demo.cpp)
# target_link_libraries(data_demo PRIVATE nnv_core nnv_utils)

# Visualization Demo (when implemented)
# add_executable(viz_demo visualization_demo.cpp)
# target_link_libraries(viz_demo PRIVATE nnv_core nnv_graphics nnv_ui nnv_utils)

# Install examples
install(TARGETS xor_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)

# Install example configurations
install(DIRECTORY configs/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/nnv/examples/configs
    FILES_MATCHING PATTERN "*.json"
)
