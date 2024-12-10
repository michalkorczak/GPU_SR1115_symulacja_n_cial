cmake_minimum_required(VERSION 3.18)
project(NBodyProblem LANGUAGES CXX CUDA)

set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(NBodyProblem src/main.cu src/initialization.cu src/file_operations.cu src/BarnesHut.cu)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(NBodyProblem PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(NBodyProblem PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


set(CMAKE_CUDA_COMPILER nvcc)
set(CMAKE_CXX_COMPILER g++)