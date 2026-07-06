#pragma once

#include "Graph/Edge.hpp"
#include "Graph/Vertex.hpp"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace BooleanEngine
{

/**
 * @brief Generic undirected graph backed by adjacency lists.
 *
 * This module is deliberately domain-neutral. It does not depend on Boolean
 * functions, Gray codes, Karnaugh maps, hypercubes, or variable counts.
 */
class Graph
{
public:
    Graph() = default;

    /**
     * @brief Adds a vertex with the next available generated ID.
     * @return The generated vertex ID.
     */
    VertexID addVertex();

    /**
     * @brief Adds a labeled vertex with the next available generated ID.
     * @return The generated vertex ID.
     */
    VertexID addVertex(const std::string& label);

    /**
     * @brief Adds a vertex with a caller-supplied unique ID.
     * @return true when inserted, false when the ID already exists.
     */
    bool addVertex(VertexID id, const std::string& label = "");

    /**
     * @brief Adds a copy of an existing vertex by ID and label.
     * @return true when inserted, false when the ID already exists.
     */
    bool addVertex(const Vertex& vertex);

    /**
     * @brief Removes a vertex and all incident edges.
     * @return true when a vertex was removed.
     */
    bool removeVertex(VertexID id);

    /**
     * @brief Checks whether a vertex ID exists.
     */
    [[nodiscard]] bool containsVertex(VertexID id) const;

    /**
     * @brief Returns a read-only vertex pointer, or nullptr when missing.
     */
    [[nodiscard]] const Vertex* getVertex(VertexID id) const;

    /**
     * @brief Returns the number of vertices.
     */
    [[nodiscard]] std::size_t vertexCount() const noexcept;

    /**
     * @brief Removes all vertices and edges.
     */
    void clear();

    /**
     * @brief Adds an undirected edge between two existing, distinct vertices.
     * @return true when inserted, false for missing vertices, loops, or duplicates.
     */
    bool addEdge(VertexID source, VertexID destination, double weight = 1.0);

    /**
     * @brief Removes an undirected edge.
     * @return true when an edge was removed.
     */
    bool removeEdge(VertexID source, VertexID destination);

    /**
     * @brief Checks whether an undirected edge exists.
     */
    [[nodiscard]] bool containsEdge(VertexID source, VertexID destination) const;

    /**
     * @brief Returns the number of stored undirected edges.
     */
    [[nodiscard]] std::size_t edgeCount() const noexcept;

    /**
     * @brief Returns a copy of the adjacent vertex IDs for a vertex.
     */
    [[nodiscard]] std::vector<VertexID> neighbors(VertexID id) const;

    /**
     * @brief Returns the number of adjacent vertices.
     */
    [[nodiscard]] std::size_t degree(VertexID id) const;

    /**
     * @brief Checks whether the graph has no vertices.
     */
    [[nodiscard]] bool isEmpty() const noexcept;

    /**
     * @brief Performs breadth-first traversal from a starting vertex.
     */
    [[nodiscard]] std::vector<VertexID> breadthFirstSearch(VertexID start) const;

    /**
     * @brief Performs depth-first traversal from a starting vertex.
     */
    [[nodiscard]] std::vector<VertexID> depthFirstSearch(VertexID start) const;

    /**
     * @brief Finds all connected components in the graph.
     */
    [[nodiscard]] std::vector<std::vector<VertexID>> connectedComponents() const;

private:
    void depthFirstSearchFrom(VertexID current,
                              std::unordered_map<VertexID, bool>& visited,
                              std::vector<VertexID>& order) const;

    VertexID nextAvailableVertexId() const;
    void updateNextVertexId(VertexID id) noexcept;

    std::unordered_map<VertexID, Vertex> vertices_;
    std::unordered_map<VertexID, std::vector<VertexID>> adjacencyList_;
    std::vector<Edge> edges_;
    VertexID nextVertexId_ = 0;
};

} // namespace BooleanEngine
