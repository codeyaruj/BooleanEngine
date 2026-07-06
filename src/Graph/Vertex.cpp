#include "Graph/Vertex.hpp"

#include <utility>

namespace BooleanEngine
{

Vertex::Vertex(VertexID id) noexcept
    : id_(id)
{
}

Vertex::Vertex(VertexID id, std::string label)
    : id_(id),
      label_(std::move(label))
{
}

VertexID Vertex::getId() const noexcept
{
    return id_;
}

void Vertex::setId(VertexID id) noexcept
{
    id_ = id;
}

const std::string& Vertex::getLabel() const noexcept
{
    return label_;
}

void Vertex::setLabel(const std::string& label)
{
    label_ = label;
}

void Vertex::setLabel(std::string&& label) noexcept
{
    label_ = std::move(label);
}

bool Vertex::isVisited() const noexcept
{
    return visited_;
}

void Vertex::setVisited(bool visited) noexcept
{
    visited_ = visited;
}

void Vertex::markVisited() noexcept
{
    visited_ = true;
}

void Vertex::clearVisited() noexcept
{
    visited_ = false;
}

} // namespace BooleanEngine
