set(GRAPHICS_SOURCES
    ColorScheme.cpp
    AnimationSystem.cpp
    RenderConfig.cpp
)

set(GRAPHICS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/graphics/ColorScheme.hpp
    ${CMAKE_SOURCE_DIR}/include/graphics/AnimationSystem.hpp
    ${CMAKE_SOURCE_DIR}/include/graphics/RenderConfig.hpp
)

add_library(nnv_graphics STATIC ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS})

target_include_directories(nnv_graphics
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(nnv_graphics
    PUBLIC
        nnv_core
        nnv_utils
)

set_target_properties(nnv_graphics PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
