cmake_minimum_required (VERSION 3.8)

project ("Simulation")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    main.cpp
    Body.cpp
    Octant.cpp
    BHTreeNode.cpp
    Simulation.cpp
)

set(HEADERS
    Body.h
    Octant.h
    BHTreeNode.h
    Simulation.h
)

add_executable(Simulation ${SOURCES} ${HEADERS})

find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Simulation PUBLIC OpenMP::OpenMP_CXX)
endif()