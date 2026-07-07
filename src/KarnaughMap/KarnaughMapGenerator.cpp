#include "KarnaughMap/KarnaughMapGenerator.hpp"

#include "Core/Exceptions.hpp"
#include "GrayCode/GrayCode.hpp"
#include "Hypercube/HypercubeGenerator.hpp"

#include <algorithm>
#include <cstddef>
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

} // namespace

KarnaughMap KarnaughMapGenerator::generate(uint32_t variableCount)
{
    if (variableCount < 1 || variableCount > 4)
    {
        throw KarnaughMapException("KarnaughMap supports only 1 to 4 variables");
    }

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

            BooleanCell cell;
            cell.id = static_cast<int>(cells.size());
            cell.binaryValue = minterm;
            cell.grayCode = GrayCode::binaryToGray(minterm);
            cell.row = static_cast<int>(row);
            cell.column = static_cast<int>(column);

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

} // namespace BooleanEngine
