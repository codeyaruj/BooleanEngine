#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace BooleanEngine
{

/*---------------------------------------------------------
 * BooleanCell
 *
 * Represents one cell of a Karnaugh Map.
 * The KMap is only a visualization layer.
 *--------------------------------------------------------*/
struct BooleanCell
{
    int id = 0;

    uint32_t binaryValue = 0;

    uint32_t grayCode = 0;

    int row = 0;
    int column = 0;

    bool value = false;

    bool dontCare = false;

    std::vector<int> neighbors;
};


/*---------------------------------------------------------
 * HypercubeNode
 *
 * Represents one vertex of the Boolean Hypercube.
 *--------------------------------------------------------*/
struct HypercubeNode
{
    int vertexID = 0;

    uint32_t binaryLabel = 0;

    std::vector<int> adjacentVertices;
};


/*---------------------------------------------------------
 * Group
 *
 * Represents a valid Boolean subcube.
 *--------------------------------------------------------*/
struct Group
{
    std::vector<int> cells;

    int dimension = 0;

    std::vector<int> variablesRemoved;

    std::string expression;
};


/*---------------------------------------------------------
 * Prime Implicant
 *--------------------------------------------------------*/
struct PrimeImplicant
{
    Group group;

    bool essential = false;
};


/*---------------------------------------------------------
 * Boolean Expression
 *--------------------------------------------------------*/
struct BooleanExpression
{
    std::string sopExpression;

    std::string posExpression;
};


/*---------------------------------------------------------
 * Coverage Matrix
 *
 * Maps Prime Implicants to covered minterms.
 *--------------------------------------------------------*/
struct CoverageMatrix
{
    std::vector<std::vector<bool>> matrix;

    std::vector<int> minterms;

    std::vector<PrimeImplicant> primeImplicants;
};

struct BooleanFunction
{
    int variableCount = 0;

    std::vector<int> minterms;

    std::vector<int> dontCares;
};

} // namespace BooleanEngine

