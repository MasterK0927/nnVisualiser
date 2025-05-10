set(UI_SOURCES
    UIManager.cpp
    InputHandler.cpp
    ConfigPanel.cpp
    NetworkEditor.cpp
    TrainingPanel.cpp
    VisualizationControls.cpp
    MenuBar.cpp
    StatusBar.cpp
    DockingSystem.cpp
)

set(UI_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ui/UIManager.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/InputHandler.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/ConfigPanel.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/NetworkEditor.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/TrainingPanel.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/VisualizationControls.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/MenuBar.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/StatusBar.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/DockingSystem.hpp
    ${CMAKE_SOURCE_DIR}/include/ui/UIConfig.hpp
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
        imgui
        nnv_core
        nnv_graphics
        nnv_utils
        sfml-graphics
        sfml-window
        sfml-system
)

set_target_properties(nnv_ui PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
