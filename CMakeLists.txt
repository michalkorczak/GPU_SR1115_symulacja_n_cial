cmake_minimum_required(VERSION 3.18)
project(NBodyProblem LANGUAGES CXX CUDA)

set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(NBodyProblem src/main.cu src/initialization.cu)

set_target_properties(NBodyProblem PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_CUDA_COMPILER nvcc)
set(CMAKE_CXX_COMPILER g++)