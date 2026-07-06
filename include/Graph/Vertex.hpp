#pragma once

#include <string>

namespace BooleanEngine
{

using VertexID = int;

/**
 * @brief Represents one vertex in a generic graph.
 *
 * Vertex only owns per-vertex data. It intentionally contains no graph
 * algorithms or adjacency knowledge.
 */
class Vertex
{
public:
    /**
     * @brief Creates vertex 0 with an empty label and unvisited state.
     */
    Vertex() = default;

    /**
     * @brief Creates a vertex with the supplied ID.
     */
    explicit Vertex(VertexID id) noexcept;

    /**
     * @brief Creates a vertex with the supplied ID and label.
     */
    Vertex(VertexID id, std::string label);

    [[nodiscard]] VertexID getId() const noexcept;
    void setId(VertexID id) noexcept;

    [[nodiscard]] const std::string& getLabel() const noexcept;
    void setLabel(const std::string& label);
    void setLabel(std::string&& label) noexcept;

    [[nodiscard]] bool isVisited() const noexcept;
    void setVisited(bool visited) noexcept;
    void markVisited() noexcept;
    void clearVisited() noexcept;

private:
    VertexID id_ = 0;
    std::string label_;
    bool visited_ = false;
};

} // namespace BooleanEngine
