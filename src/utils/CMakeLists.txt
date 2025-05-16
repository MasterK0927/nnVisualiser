set(UTILS_SOURCES
    Logger.cpp
    ConfigManager.cpp
    DataLoader.cpp
    Common.cpp
)

set(UTILS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/utils/Logger.hpp
    ${CMAKE_SOURCE_DIR}/include/utils/ConfigManager.hpp
    ${CMAKE_SOURCE_DIR}/include/utils/DataLoader.hpp
    ${CMAKE_SOURCE_DIR}/include/utils/Common.hpp
)

add_library(nnv_utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})

target_include_directories(nnv_utils
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(nnv_utils
    PUBLIC
        nlohmann_json::nlohmann_json
)

# Add spdlog for logging if available
find_package(spdlog QUIET)
if(spdlog_FOUND)
    target_link_libraries(nnv_utils PUBLIC spdlog::spdlog)
    target_compile_definitions(nnv_utils PUBLIC HAS_SPDLOG)
endif()

# Add fmt for formatting if available
find_package(fmt QUIET)
if(fmt_FOUND)
    target_link_libraries(nnv_utils PUBLIC fmt::fmt)
    target_compile_definitions(nnv_utils PUBLIC HAS_FMT)
endif()

set_target_properties(nnv_utils PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
