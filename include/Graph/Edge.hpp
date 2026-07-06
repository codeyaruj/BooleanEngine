#pragma once

#include "Graph/Vertex.hpp"

namespace BooleanEngine
{

/**
 * @brief Represents one weighted connection between two vertices.
 *
 * The graph owns the interpretation of direction. The Graph class stores these
 * edges as undirected connections.
 */
class Edge
{
public:
    /**
     * @brief Creates an edge from vertex 0 to vertex 0 with weight 1.
     */
    Edge() = default;

    /**
     * @brief Creates an edge from source to destination.
     */
    Edge(VertexID source, VertexID destination, double weight = 1.0) noexcept;

    [[nodiscard]] VertexID getSource() const noexcept;
    [[nodiscard]] VertexID getDestination() const noexcept;
    [[nodiscard]] double getWeight() const noexcept;

private:
    VertexID source_ = 0;
    VertexID destination_ = 0;
    double weight_ = 1.0;
};

} // namespace BooleanEngine
