# Testing configuration
find_package(GTest QUIET)

if(GTest_FOUND)
    enable_testing()
    
    # Test sources
    set(TEST_SOURCES
        test_main.cpp
        core/test_neuron.cpp
        core/test_layer.cpp
        core/test_activation_functions.cpp
        utils/test_config_manager.cpp
        utils/test_logger.cpp
    )
    
    # Create test executable
    add_executable(nnv_tests ${TEST_SOURCES})
    
    target_link_libraries(nnv_tests
        PRIVATE
            nnv_core
            nnv_utils
            GTest::gtest
            GTest::gtest_main
    )
    
    target_include_directories(nnv_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
    )
    
    # Register tests with CTest
    include(GoogleTest)
    gtest_discover_tests(nnv_tests)
    
    # Individual test executables
    add_executable(core_tests
        test_main.cpp
        core/test_neuron.cpp
        core/test_layer.cpp
        core/test_activation_functions.cpp
    )
    
    target_link_libraries(core_tests
        PRIVATE
            nnv_core
            nnv_utils
            GTest::gtest
            GTest::gtest_main
    )
    
    target_include_directories(core_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
    )
    
    add_executable(utils_tests
        test_main.cpp
        utils/test_config_manager.cpp
        utils/test_logger.cpp
    )
    
    target_link_libraries(utils_tests
        PRIVATE
            nnv_utils
            GTest::gtest
            GTest::gtest_main
    )
    
    target_include_directories(utils_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
    )
    
else()
    message(WARNING "Google Test not found. Tests will not be built.")
endif()
