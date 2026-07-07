#include "KarnaughMap/KarnaughMap.hpp"

#include "Core/Exceptions.hpp"

#include <limits>
#include <utility>

namespace BooleanEngine
{
namespace
{

std::size_t invalidIndex() noexcept
{
    return std::numeric_limits<std::size_t>::max();
}

} // namespace

KarnaughMap::KarnaughMap(uint32_t variableCount,
                         std::size_t rows,
                         std::size_t columns,
                         std::vector<BooleanCell> cells,
                         Hypercube hypercube)
    : variableCount_(variableCount),
      rows_(rows),
      columns_(columns),
      cells_(std::move(cells)),
      hypercube_(std::move(hypercube))
{
    validate();
    buildLookupTables();
}

uint32_t KarnaughMap::dimension() const noexcept
{
    return variableCount_;
}

std::size_t KarnaughMap::rows() const noexcept
{
    return rows_;
}

std::size_t KarnaughMap::columns() const noexcept
{
    return columns_;
}

std::size_t KarnaughMap::cellCount() const noexcept
{
    return cells_.size();
}

const BooleanCell* KarnaughMap::getCell(std::size_t index) const
{
    if (index >= cells_.size())
    {
        return nullptr;
    }

    return &cells_[index];
}

const BooleanCell* KarnaughMap::getCell(std::size_t row, std::size_t column) const
{
    if (row >= rows_ || column >= columns_ || coordinateToCellIndex_.empty())
    {
        return nullptr;
    }

    const std::size_t index = coordinateToCellIndex_[coordinateIndex(row, column)];
    if (index == invalidIndex())
    {
        return nullptr;
    }

    return &cells_[index];
}

const BooleanCell* KarnaughMap::getCellFromMinterm(uint32_t minterm) const
{
    if (minterm >= mintermToCellIndex_.size())
    {
        return nullptr;
    }

    const std::size_t index = mintermToCellIndex_[minterm];
    if (index == invalidIndex())
    {
        return nullptr;
    }

    return &cells_[index];
}

bool KarnaughMap::containsMinterm(uint32_t minterm) const
{
    return getCellFromMinterm(minterm) != nullptr;
}

std::vector<int> KarnaughMap::neighbors(uint32_t minterm) const
{
    const BooleanCell* cell = getCellFromMinterm(minterm);
    if (cell == nullptr)
    {
        return {};
    }

    std::vector<int> neighborMinterms;
    neighborMinterms.reserve(cell->neighbors.size());

    for (int neighborCellIndex : cell->neighbors)
    {
        const BooleanCell* neighborCell = getCell(static_cast<std::size_t>(neighborCellIndex));
        if (neighborCell != nullptr)
        {
            neighborMinterms.push_back(static_cast<int>(neighborCell->binaryValue));
        }
    }

    return neighborMinterms;
}

const Hypercube& KarnaughMap::hypercube() const noexcept
{
    return hypercube_;
}

std::size_t KarnaughMap::coordinateIndex(std::size_t row, std::size_t column) const
{
    return row * columns_ + column;
}

void KarnaughMap::buildLookupTables()
{
    mintermToCellIndex_.assign(cells_.size(), invalidIndex());
    coordinateToCellIndex_.assign(cells_.size(), invalidIndex());

    for (std::size_t index = 0; index < cells_.size(); ++index)
    {
        const BooleanCell& cell = cells_[index];
        mintermToCellIndex_[cell.binaryValue] = index;
        coordinateToCellIndex_[coordinateIndex(static_cast<std::size_t>(cell.row),
                                               static_cast<std::size_t>(cell.column))] = index;
    }
}

void KarnaughMap::validate() const
{
    if (variableCount_ < 1 || variableCount_ > 4)
    {
        throw KarnaughMapException("KarnaughMap supports only 1 to 4 variables");
    }

    if (rows_ == 0 || columns_ == 0 || rows_ * columns_ != cells_.size())
    {
        throw KarnaughMapException("Invalid KarnaughMap dimensions");
    }

    if (hypercube_.dimension() != variableCount_ || hypercube_.vertexCount() != cells_.size())
    {
        throw KarnaughMapException("KarnaughMap hypercube does not match cell layout");
    }

    for (const BooleanCell& cell : cells_)
    {
        if (cell.row < 0 || cell.column < 0 ||
            static_cast<std::size_t>(cell.row) >= rows_ ||
            static_cast<std::size_t>(cell.column) >= columns_ ||
            cell.binaryValue >= cells_.size())
        {
            throw KarnaughMapException("Invalid KarnaughMap cell");
        }
    }
}

} // namespace BooleanEngine
