#pragma once

#include "KarnaughMap/KarnaughMap.hpp"

#include <cstdint>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Constructs standard 2-, 3-, and 4-variable Karnaugh Maps.
 */
class KarnaughMapGenerator
{
public:
    /**
     * @brief Generates a Karnaugh Map with Gray-code row and column ordering.
     *
     * @throws KarnaughMapException when variableCount is outside [2, 4].
     */
    [[nodiscard]] static KarnaughMap generate(uint32_t variableCount);

    /**
     * @brief Generates a K-map from minterms and optional don't-care minterms.
     */
    [[nodiscard]] static KarnaughMap fromMinterms(
        uint32_t variableCount,
        const std::vector<uint32_t>& minterms,
        const std::vector<uint32_t>& dontCares = {});

    /**
     * @brief Generates a K-map from maxterms and optional don't-care minterms.
     */
    [[nodiscard]] static KarnaughMap fromMaxterms(
        uint32_t variableCount,
        const std::vector<uint32_t>& maxterms,
        const std::vector<uint32_t>& dontCares = {});

    /**
     * @brief Generates a K-map from a truth table and optional don't-care minterms.
     */
    [[nodiscard]] static KarnaughMap fromTruthTable(
        uint32_t variableCount,
        const std::vector<bool>& truthTable,
        const std::vector<uint32_t>& dontCares = {});
};

} // namespace BooleanEngine
