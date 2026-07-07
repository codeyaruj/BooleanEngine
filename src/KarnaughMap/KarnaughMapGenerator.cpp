#include "KarnaughMap/KarnaughMapGenerator.hpp"

#include "Core/Constants.hpp"
#include "Core/Exceptions.hpp"
#include "GrayCode/GrayCode.hpp"
#include "Hypercube/HypercubeGenerator.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace BooleanEngine
{
namespace
{

void addUniqueNeighbor(std::vector<int>& neighbors, int neighbor)
{
    if (std::find(neighbors.begin(), neighbors.end(), neighbor) == neighbors.end())
    {
        neighbors.push_back(neighbor);
    }
}

void validateVariableCount(uint32_t variableCount)
{
    if (variableCount < static_cast<uint32_t>(MIN_SUPPORTED_VARIABLES) ||
        variableCount > static_cast<uint32_t>(MAX_SUPPORTED_VARIABLES))
    {
        throw KarnaughMapException("KarnaughMap supports only 2 to 4 variables");
    }
}

std::size_t cellCountFor(uint32_t variableCount)
{
    return static_cast<std::size_t>(1u << variableCount);
}

void validateMintermList(uint32_t variableCount,
                         const std::vector<uint32_t>& values,
                         const std::string& label)
{
    const std::size_t cellCount = cellCountFor(variableCount);
    std::vector<bool> seen(cellCount, false);

    for (uint32_t value : values)
    {
        if (value >= cellCount)
        {
            throw KarnaughMapException(label + " contains out-of-range minterm");
        }

        if (seen[value])
        {
            throw KarnaughMapException(label + " contains duplicate minterm");
        }

        seen[value] = true;
    }
}

void validateNoOverlap(const std::vector<uint32_t>& lhs,
                       const std::vector<uint32_t>& rhs,
                       const std::string& message)
{
    for (uint32_t value : lhs)
    {
        if (std::find(rhs.begin(), rhs.end(), value) != rhs.end())
        {
            throw KarnaughMapException(message);
        }
    }
}

std::vector<bool> variableAssignmentFor(uint32_t minterm, uint32_t variableCount)
{
    std::vector<bool> assignment;
    assignment.reserve(variableCount);

    for (uint32_t bit = 0; bit < variableCount; ++bit)
    {
        const uint32_t mask = 1u << (variableCount - bit - 1u);
        assignment.push_back((minterm & mask) != 0);
    }

    return assignment;
}

KarnaughMap buildMap(uint32_t variableCount,
                     const std::vector<bool>& values,
                     const std::vector<uint32_t>& dontCares)
{
    validateVariableCount(variableCount);

    const std::size_t cellCount = cellCountFor(variableCount);
    if (values.size() != cellCount)
    {
        throw KarnaughMapException("Truth table size does not match variable count");
    }

    validateMintermList(variableCount, dontCares, "Don't-care list");

    const uint32_t rowBits = variableCount / 2;
    const uint32_t columnBits = variableCount - rowBits;
    const std::size_t rows = static_cast<std::size_t>(1u << rowBits);
    const std::size_t columns = static_cast<std::size_t>(1u << columnBits);

    const std::vector<unsigned int> rowOrder = GrayCode::generateSequence(rowBits);
    const std::vector<unsigned int> columnOrder = GrayCode::generateSequence(columnBits);

    std::vector<BooleanCell> cells;
    cells.reserve(rows * columns);

    for (std::size_t row = 0; row < rows; ++row)
    {
        for (std::size_t column = 0; column < columns; ++column)
        {
            const uint32_t minterm =
                (static_cast<uint32_t>(rowOrder[row]) << columnBits) |
                static_cast<uint32_t>(columnOrder[column]);
            const bool dontCare =
                std::find(dontCares.begin(), dontCares.end(), minterm) != dontCares.end();

            BooleanCell cell;
            cell.id = static_cast<int>(cells.size());
            cell.binaryValue = minterm;
            cell.grayCode = GrayCode::binaryToGray(minterm);
            cell.row = static_cast<int>(row);
            cell.column = static_cast<int>(column);
            cell.value = values[minterm] && !dontCare;
            cell.dontCare = dontCare;
            cell.variableAssignment = variableAssignmentFor(minterm, variableCount);

            cells.push_back(cell);
        }
    }

    for (std::size_t row = 0; row < rows; ++row)
    {
        for (std::size_t column = 0; column < columns; ++column)
        {
            BooleanCell& cell = cells[row * columns + column];

            const std::size_t up = (row + rows - 1) % rows;
            const std::size_t down = (row + 1) % rows;
            const std::size_t left = (column + columns - 1) % columns;
            const std::size_t right = (column + 1) % columns;

            const int upIndex = static_cast<int>(up * columns + column);
            const int downIndex = static_cast<int>(down * columns + column);
            const int leftIndex = static_cast<int>(row * columns + left);
            const int rightIndex = static_cast<int>(row * columns + right);

            if (upIndex != cell.id)
            {
                addUniqueNeighbor(cell.neighbors, upIndex);
            }

            if (downIndex != cell.id)
            {
                addUniqueNeighbor(cell.neighbors, downIndex);
            }

            if (leftIndex != cell.id)
            {
                addUniqueNeighbor(cell.neighbors, leftIndex);
            }

            if (rightIndex != cell.id)
            {
                addUniqueNeighbor(cell.neighbors, rightIndex);
            }
        }
    }

    Hypercube hypercube = HypercubeGenerator::generate(variableCount);

    return KarnaughMap(variableCount, rows, columns, std::move(cells), std::move(hypercube));
}

} // namespace

KarnaughMap KarnaughMapGenerator::generate(uint32_t variableCount)
{
    validateVariableCount(variableCount);
    return buildMap(variableCount, std::vector<bool>(cellCountFor(variableCount), false), {});
}

KarnaughMap KarnaughMapGenerator::fromMinterms(uint32_t variableCount,
                                               const std::vector<uint32_t>& minterms,
                                               const std::vector<uint32_t>& dontCares)
{
    validateVariableCount(variableCount);
    validateMintermList(variableCount, minterms, "Minterm list");
    validateMintermList(variableCount, dontCares, "Don't-care list");
    validateNoOverlap(minterms, dontCares, "Minterms and don't-cares must not overlap");

    std::vector<bool> values(cellCountFor(variableCount), false);
    for (uint32_t minterm : minterms)
    {
        values[minterm] = true;
    }

    return buildMap(variableCount, values, dontCares);
}

KarnaughMap KarnaughMapGenerator::fromMaxterms(uint32_t variableCount,
                                               const std::vector<uint32_t>& maxterms,
                                               const std::vector<uint32_t>& dontCares)
{
    validateVariableCount(variableCount);
    validateMintermList(variableCount, maxterms, "Maxterm list");
    validateMintermList(variableCount, dontCares, "Don't-care list");
    validateNoOverlap(maxterms, dontCares, "Maxterms and don't-cares must not overlap");

    std::vector<bool> values(cellCountFor(variableCount), true);
    for (uint32_t maxterm : maxterms)
    {
        values[maxterm] = false;
    }

    return buildMap(variableCount, values, dontCares);
}

KarnaughMap KarnaughMapGenerator::fromTruthTable(uint32_t variableCount,
                                                 const std::vector<bool>& truthTable,
                                                 const std::vector<uint32_t>& dontCares)
{
    validateVariableCount(variableCount);
    validateMintermList(variableCount, dontCares, "Don't-care list");
    return buildMap(variableCount, truthTable, dontCares);
}

} // namespace BooleanEngine
