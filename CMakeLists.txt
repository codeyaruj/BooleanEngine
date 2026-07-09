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

    # Karnaugh Map
    src/KarnaughMap/KarnaughMap.cpp
    src/KarnaughMap/KarnaughMapGenerator.cpp

    # Parser
    src/Parser/Parser.cpp
    src/Parser/Tokenizer.cpp
    src/Parser/ParserUtilities.cpp
    src/Parser/BooleanExpressionParser.cpp
    src/Parser/MintermParser.cpp
    src/Parser/TruthTableParser.cpp

    # Grouping
    src/Grouping/GroupDetector.cpp
    src/Grouping/GroupExpander.cpp

    # Prime Implicants
    src/PrimeImplicants/CoverageMatrix.cpp
    src/PrimeImplicants/PrimeImplicantFinder.cpp

    # Simplifier
    src/Simplifier/Simplifier.cpp
    src/Simplifier/Petrick.cpp

    # Expression
    src/Expression/AST.cpp
    src/Expression/Expression.cpp
    src/Expression/ExpressionGenerator.cpp

    # Evaluation
    src/Evaluation/BooleanEvaluator.cpp

    # Exporter
    src/Exporter/JsonExporter.cpp
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

add_executable(test_kmap

    tests/test_kmap.cpp

    src/KarnaughMap/KarnaughMap.cpp
    src/KarnaughMap/KarnaughMapGenerator.cpp

    src/Hypercube/Hypercube.cpp
    src/Hypercube/HypercubeGenerator.cpp

    src/Graph/Vertex.cpp
    src/Graph/Edge.cpp
    src/Graph/Graph.cpp

    src/Utilities/BinaryUtils.cpp
    src/Utilities/BitUtils.cpp

    src/GrayCode/GrayCode.cpp
)

target_include_directories(test_kmap PRIVATE include)

add_test(
    NAME KarnaughMapTest
    COMMAND test_kmap
)

add_executable(test_parser

    tests/test_parser.cpp

    src/Parser/Tokenizer.cpp
    src/Parser/ParserUtilities.cpp
)

target_include_directories(test_parser PRIVATE include)

add_test(
    NAME ParserInfrastructureTest
    COMMAND test_parser
)

add_executable(test_boolean_expression_parser

    tests/test_boolean_expression_parser.cpp

    src/Parser/BooleanExpressionParser.cpp
    src/Parser/Tokenizer.cpp
    src/Parser/ParserUtilities.cpp

    src/Expression/AST.cpp
    src/Expression/Expression.cpp
)

target_include_directories(test_boolean_expression_parser PRIVATE include)

add_test(
    NAME BooleanExpressionParserTest
    COMMAND test_boolean_expression_parser
)

add_executable(test_minterm_parser

    tests/test_minterm_parser.cpp

    src/Parser/MintermParser.cpp
    src/Parser/Tokenizer.cpp
    src/Parser/ParserUtilities.cpp
)

target_include_directories(test_minterm_parser PRIVATE include)

add_test(
    NAME MintermParserTest
    COMMAND test_minterm_parser
)

add_executable(test_truth_table_parser

    tests/test_truth_table_parser.cpp

    src/Parser/TruthTableParser.cpp
    src/Parser/ParserUtilities.cpp
)

target_include_directories(test_truth_table_parser PRIVATE include)

add_test(
    NAME TruthTableParserTest
    COMMAND test_truth_table_parser
)

add_executable(test_boolean_evaluator

    tests/test_boolean_evaluator.cpp

    src/Evaluation/BooleanEvaluator.cpp

    src/Parser/BooleanExpressionParser.cpp
    src/Parser/Tokenizer.cpp
    src/Parser/ParserUtilities.cpp

    src/Expression/AST.cpp
    src/Expression/Expression.cpp
)

target_include_directories(test_boolean_evaluator PRIVATE include)

add_test(
    NAME BooleanEvaluatorTest
    COMMAND test_boolean_evaluator
)
