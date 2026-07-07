#pragma once

#include "Core/Types.hpp"
#include "Hypercube/Hypercube.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Represents a generated Karnaugh Map for one to four variables.
 *
 * The map stores cells in row-major display order while also providing O(1)
 * lookup by minterm and row/column coordinate. This class only represents the
 * map layout and adjacency; it does not perform grouping or simplification.
 */
class KarnaughMap
{
public:
    KarnaughMap() = default;
    KarnaughMap(uint32_t variableCount,
                std::size_t rows,
                std::size_t columns,
                std::vector<BooleanCell> cells,
                Hypercube hypercube);

    /**
     * @brief Returns the number of Boolean variables represented by the map.
     */
    [[nodiscard]] uint32_t dimension() const noexcept;

    /**
     * @brief Returns the number of display rows.
     */
    [[nodiscard]] std::size_t rows() const noexcept;

    /**
     * @brief Returns the number of display columns.
     */
    [[nodiscard]] std::size_t columns() const noexcept;

    /**
     * @brief Returns the number of cells in the map.
     */
    [[nodiscard]] std::size_t cellCount() const noexcept;

    /**
     * @brief Returns a cell by row-major cell index, or nullptr when missing.
     */
    [[nodiscard]] const BooleanCell* getCell(std::size_t index) const;

    /**
     * @brief Returns a cell by display coordinate, or nullptr when missing.
     */
    [[nodiscard]] const BooleanCell* getCell(std::size_t row, std::size_t column) const;

    /**
     * @brief Returns the cell that represents a minterm, or nullptr when missing.
     */
    [[nodiscard]] const BooleanCell* getCellFromMinterm(uint32_t minterm) const;

    /**
     * @brief Checks whether a minterm is represented in the map.
     */
    [[nodiscard]] bool containsMinterm(uint32_t minterm) const;

    /**
     * @brief Returns neighboring minterms for a minterm, including wrap-around.
     */
    [[nodiscard]] std::vector<int> neighbors(uint32_t minterm) const;

    /**
     * @brief Provides read-only access to the underlying hypercube.
     */
    [[nodiscard]] const Hypercube& hypercube() const noexcept;

private:
    [[nodiscard]] std::size_t coordinateIndex(std::size_t row, std::size_t column) const;
    void buildLookupTables();
    void validate() const;

    uint32_t variableCount_ = 0;
    std::size_t rows_ = 0;
    std::size_t columns_ = 0;
    std::vector<BooleanCell> cells_;
    std::vector<std::size_t> mintermToCellIndex_;
    std::vector<std::size_t> coordinateToCellIndex_;
    Hypercube hypercube_;
};

} // namespace BooleanEngine
