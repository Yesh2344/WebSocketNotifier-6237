cmake_minimum_required(VERSION 3.14)
project(WebSocketNotifier LANGUAGES CXX)

# ----------------------------------------------------------------------
# Global settings
# ----------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ----------------------------------------------------------------------
# Find packages
# ----------------------------------------------------------------------
find_package(Boost REQUIRED COMPONENTS system thread regex)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# GoogleTest is optional – only needed for tests
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
endif()

# ----------------------------------------------------------------------
# Sources
# ----------------------------------------------------------------------
set(SRC_FILES
    src/main.cpp
    src/Server.cpp
    src/ClientManager.cpp
    src/Config.cpp
    src/Logger.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::system Boost::thread Boost::regex
    PRIVATE spdlog::spdlog
    PRIVATE nlohmann_json::nlohmann_json
)

# ----------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------
if(BUILD_TESTS)
    file(GLOB TEST_SOURCES test/*.cpp)
    add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES} src/Server.cpp src/ClientManager.cpp src/Config.cpp src/Logger.cpp)
    target_include_directories(${PROJECT_NAME}_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME}_tests
        PRIVATE Boost::system Boost::thread Boost::regex
        PRIVATE spdlog::spdlog
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE GTest::gtest GTest::gtest_main
    )
    add_test(NAME WebSocketNotifierTests COMMAND ${PROJECT_NAME}_tests)
endif()