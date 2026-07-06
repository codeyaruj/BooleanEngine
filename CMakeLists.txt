cmake_minimum_required(VERSION 3.16)

project(BooleanEngine VERSION 1.0.0 LANGUAGES CXX)

# --------------------------------------------------
# C++ Standard
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# Compiler Warnings
# --------------------------------------------------
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --------------------------------------------------
# Include Directories
# --------------------------------------------------
include_directories(include)

# --------------------------------------------------
# Main Executable
# --------------------------------------------------
add_executable(BooleanEngine

    src/main.cpp

    # Utilities
    src/Utilities/BinaryUtils.cpp
    src/Utilities/BitUtils.cpp
    src/Utilities/MathUtils.cpp

    # Gray Code
    src/GrayCode/GrayCode.cpp

    # Graph
    src/Graph/Vertex.cpp
    src/Graph/Edge.cpp
    src/Graph/Graph.cpp

    # Hypercube
    src/Hypercube/Hypercube.cpp
    src/Hypercube/HypercubeGenerator.cpp

    # Parser
    # src/Parser/Parser.cpp

    # Grouping
    # src/Grouping/Grouping.cpp

    # Prime Implicants
    # src/PrimeImplicants/PrimeImplicants.cpp

    # Simplifier
    # src/Simplifier/Simplifier.cpp

    # Expression
    # src/Expression/Expression.cpp

    # Exporter
    # src/Exporter/Exporter.cpp
)

# --------------------------------------------------
# Testing
# --------------------------------------------------
enable_testing()

add_executable(test_utilities

    tests/test_utilities.cpp

    src/Utilities/BinaryUtils.cpp
    src/Utilities/BitUtils.cpp
    src/Utilities/MathUtils.cpp
)

target_include_directories(test_utilities PRIVATE include)

add_test(
    NAME UtilitiesTest
    COMMAND test_utilities
)

add_executable(test_gray

    tests/test_graycode.cpp

    src/GrayCode/GrayCode.cpp
)

target_include_directories(test_gray PRIVATE include)

add_test(
    NAME GrayCodeTest
    COMMAND test_gray
)

add_executable(test_graph

    tests/test_graph.cpp

    src/Graph/Vertex.cpp
    src/Graph/Edge.cpp
    src/Graph/Graph.cpp
)

target_include_directories(test_graph PRIVATE include)

add_test(
    NAME GraphTest
    COMMAND test_graph
)

add_executable(test_hypercube

    tests/test_hypercube.cpp

    src/Hypercube/Hypercube.cpp
    src/Hypercube/HypercubeGenerator.cpp

    src/Graph/Vertex.cpp
    src/Graph/Edge.cpp
    src/Graph/Graph.cpp

    src/Utilities/BinaryUtils.cpp
    src/Utilities/BitUtils.cpp
)

target_include_directories(test_hypercube PRIVATE include)

add_test(
    NAME HypercubeTest
    COMMAND test_hypercube
)
