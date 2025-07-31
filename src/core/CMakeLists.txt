set(CORE_SOURCES
    Application.cpp
    NeuralNetwork.cpp
    Layer.cpp
    Neuron.cpp
    ActivationFunctions.cpp
    LossFunctions.cpp
    WeightInitializers.cpp
)

set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/core/Application.hpp
    ${CMAKE_SOURCE_DIR}/include/core/NeuralNetwork.hpp
    ${CMAKE_SOURCE_DIR}/include/core/Layer.hpp
    ${CMAKE_SOURCE_DIR}/include/core/Neuron.hpp
    ${CMAKE_SOURCE_DIR}/include/core/ActivationFunctions.hpp
    ${CMAKE_SOURCE_DIR}/include/core/LossFunctions.hpp
    ${CMAKE_SOURCE_DIR}/include/core/WeightInitializers.hpp
    ${CMAKE_SOURCE_DIR}/include/core/Types.hpp
)

add_library(nnv_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(nnv_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(nnv_core
    PUBLIC
        nlohmann_json::nlohmann_json
        sfml-graphics
        sfml-window
        sfml-system
    PRIVATE
        nnv_utils
)

# Enable Eigen3 if available
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    target_link_libraries(nnv_core PUBLIC Eigen3::Eigen)
    target_compile_definitions(nnv_core PUBLIC HAS_EIGEN3)
endif()

set_target_properties(nnv_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
