# Dear ImGui library
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_sfml.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui
    PUBLIC
        imgui
        imgui/backends
)

target_link_libraries(imgui
    PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC OpenGL::GL)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(imgui PRIVATE opengl32)
elseif(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(imgui PRIVATE ${OPENGL_LIBRARY})
else()
    target_link_libraries(imgui PRIVATE GL)
endif()

set_target_properties(imgui PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
