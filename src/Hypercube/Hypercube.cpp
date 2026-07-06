#include "Hypercube/Hypercube.hpp"

#include <utility>

namespace BooleanEngine
{

Hypercube::Hypercube(uint32_t dimension) noexcept
    : dimension_(dimension)
{
}

Hypercube::Hypercube(uint32_t dimension, Graph graph)
    : graph_(std::move(graph)),
      dimension_(dimension)
{
}

uint32_t Hypercube::dimension() const noexcept
{
    return dimension_;
}

std::size_t Hypercube::vertexCount() const noexcept
{
    return graph_.vertexCount();
}

std::size_t Hypercube::edgeCount() const noexcept
{
    return graph_.edgeCount();
}

const Graph& Hypercube::graph() const noexcept
{
    return graph_;
}

bool Hypercube::containsVertex(VertexID id) const
{
    return graph_.containsVertex(id);
}

const Vertex* Hypercube::getVertex(VertexID id) const
{
    return graph_.getVertex(id);
}

std::vector<VertexID> Hypercube::neighbors(VertexID id) const
{
    return graph_.neighbors(id);
}

} // namespace BooleanEngine
