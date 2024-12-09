cmake_minimum_required (VERSION 3.8)

project ("Simulation")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


include_directories(${PROJECT_SOURCE_DIR}/src)
set(TEST_SOURCES
    src/Body.cpp
    src/Octant.cpp
    src/BHTreeNode.cpp
    src/Simulation.cpp
    tests/BodyTest.cpp 
    tests/OctantTest.cpp 
    tests/BHTreeNodeTest.cpp
    tests/SimulationTest.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests gtest gtest_main)
add_test(NAME RunTests COMMAND tests)

set(SOURCES
    src/main.cpp
    src/Body.cpp
    src/Octant.cpp
    src/BHTreeNode.cpp
    src/Simulation.cpp
)

set(HEADERS
    src/Body.h
    src/Octant.h
    src/BHTreeNode.h
    src/Simulation.h
)

add_executable(Simulation ${SOURCES} ${HEADERS})

find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Simulation PUBLIC OpenMP::OpenMP_CXX)
endif()