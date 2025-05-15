set(UI_SOURCES
    NetworkPanel.cpp
    UIPanel.cpp
)

set(UI_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ui/NetworkPanel.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/UIPanel.hpp
)

add_library(nnv_ui STATIC ${UI_SOURCES} ${UI_HEADERS})

target_include_directories(nnv_ui
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(nnv_ui
    PUBLIC
        nnv_core
        nnv_graphics
        nnv_utils
)

set_target_properties(nnv_ui PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
