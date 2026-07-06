#include "Graph/Edge.hpp"

namespace BooleanEngine
{

Edge::Edge(VertexID source, VertexID destination, double weight) noexcept
    : source_(source),
      destination_(destination),
      weight_(weight)
{
}

VertexID Edge::getSource() const noexcept
{
    return source_;
}

VertexID Edge::getDestination() const noexcept
{
    return destination_;
}

double Edge::getWeight() const noexcept
{
    return weight_;
}

} // namespace BooleanEngine
