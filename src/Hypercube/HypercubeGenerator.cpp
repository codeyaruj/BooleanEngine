#include "Hypercube/HypercubeGenerator.hpp"

#include "Utilities/BinaryUtils.hpp"
#include "Utilities/BitUtils.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace BooleanEngine
{

Hypercube HypercubeGenerator::generate(uint32_t dimension)
{
    if (dimension >= static_cast<uint32_t>(std::numeric_limits<VertexID>::digits))
    {
        throw std::invalid_argument("Hypercube dimension is too large for VertexID storage");
    }

    Graph graph;
    const uint32_t vertexCount = 1u << dimension;

    for (uint32_t value = 0; value < vertexCount; ++value)
    {
        graph.addVertex(static_cast<VertexID>(value),
                        BinaryUtils::toBinary(value, static_cast<int>(dimension)));
    }

    for (uint32_t value = 0; value < vertexCount; ++value)
    {
        for (uint32_t bit = 0; bit < dimension; ++bit)
        {
            const uint32_t neighbor = value ^ (1u << bit);

            if (neighbor > value && BitUtils::hammingDistance(value, neighbor) == 1)
            {
                graph.addEdge(static_cast<VertexID>(value), static_cast<VertexID>(neighbor));
            }
        }
    }

    return Hypercube(dimension, std::move(graph));
}

} // namespace BooleanEngine
