#pragma once

#include "Hypercube/Hypercube.hpp"

#include <cstdint>

namespace BooleanEngine
{

/**
 * @brief Constructs generic Boolean hypercubes.
 */
class HypercubeGenerator
{
public:
    /**
     * @brief Generates an n-dimensional Boolean hypercube.
     *
     * The generated graph contains 2^dimension vertices. Vertex labels are the
     * fixed-width binary representation of their integer IDs.
     */
    [[nodiscard]] static Hypercube generate(uint32_t dimension);
};

} // namespace BooleanEngine
