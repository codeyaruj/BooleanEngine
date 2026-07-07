#pragma once

#include "KarnaughMap/KarnaughMap.hpp"

#include <cstdint>

namespace BooleanEngine
{

/**
 * @brief Constructs standard 1-, 2-, 3-, and 4-variable Karnaugh Maps.
 */
class KarnaughMapGenerator
{
public:
    /**
     * @brief Generates a Karnaugh Map with Gray-code row and column ordering.
     *
     * @throws KarnaughMapException when variableCount is outside [1, 4].
     */
    [[nodiscard]] static KarnaughMap generate(uint32_t variableCount);
};

} // namespace BooleanEngine
