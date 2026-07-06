#pragma once

#include "Graph/Graph.hpp"
#include "Graph/Vertex.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Generic n-dimensional Boolean hypercube.
 *
 * The hypercube owns an undirected graph whose vertices are binary states and
 * whose edges connect states that differ in exactly one bit. This class is
 * intentionally independent of Karnaugh maps, Gray-code layouts, and variable
 * count restrictions.
 */
class Hypercube
{
public:
    Hypercube() = default;
    explicit Hypercube(uint32_t dimension) noexcept;
    Hypercube(uint32_t dimension, Graph graph);

    /**
     * @brief Returns the number of dimensions, also known as variables.
     */
    [[nodiscard]] uint32_t dimension() const noexcept;

    /**
     * @brief Returns the number of vertices in the owned graph.
     */
    [[nodiscard]] std::size_t vertexCount() const noexcept;

    /**
     * @brief Returns the number of undirected edges in the owned graph.
     */
    [[nodiscard]] std::size_t edgeCount() const noexcept;

    /**
     * @brief Provides read-only access to the underlying graph.
     */
    [[nodiscard]] const Graph& graph() const noexcept;

    /**
     * @brief Checks whether the binary state exists as a vertex.
     */
    [[nodiscard]] bool containsVertex(VertexID id) const;

    /**
     * @brief Returns a read-only vertex pointer, or nullptr when missing.
     */
    [[nodiscard]] const Vertex* getVertex(VertexID id) const;

    /**
     * @brief Returns adjacent binary states for the supplied vertex.
     */
    [[nodiscard]] std::vector<VertexID> neighbors(VertexID id) const;

private:
    Graph graph_;
    uint32_t dimension_ = 0;
};

} // namespace BooleanEngine
