cmake_minimum_required(VERSION 3.10)
project(NBodySimulationCPU LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
set(TEST_SOURCES
    tests/tests.cpp
    src/physics.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests gtest gtest_main nlohmann_json::nlohmann_json)
add_test(NAME RunTests COMMAND tests)

set(SOURCES
    src/main.cpp
    src/physics.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenMP::OpenMP_CXX
    nlohmann_json::nlohmann_json
)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3)
elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Debug>:/W4 /RTC1 /Od>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Release>:/W4 /O2>)
endif()


set_target_properties(NBodySimulationCPU PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)