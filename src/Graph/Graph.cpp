#include "Graph/Graph.hpp"

#include <algorithm>
#include <queue>

namespace BooleanEngine
{

VertexID Graph::addVertex()
{
    const VertexID id = nextAvailableVertexId();
    const bool added = addVertex(id);
    (void)added;
    return id;
}

VertexID Graph::addVertex(const std::string& label)
{
    const VertexID id = nextAvailableVertexId();
    const bool added = addVertex(id, label);
    (void)added;
    return id;
}

bool Graph::addVertex(VertexID id, const std::string& label)
{
    if (containsVertex(id))
    {
        return false;
    }

    vertices_.emplace(id, Vertex(id, label));
    adjacencyList_.emplace(id, std::vector<VertexID>{});
    updateNextVertexId(id);

    return true;
}

bool Graph::addVertex(const Vertex& vertex)
{
    return addVertex(vertex.getId(), vertex.getLabel());
}

bool Graph::removeVertex(VertexID id)
{
    if (!containsVertex(id))
    {
        return false;
    }

    const auto incidentNeighbors = adjacencyList_.at(id);

    for (VertexID neighbor : incidentNeighbors)
    {
        auto& neighborList = adjacencyList_.at(neighbor);
        neighborList.erase(std::remove(neighborList.begin(), neighborList.end(), id),
                           neighborList.end());
    }

    edges_.erase(std::remove_if(edges_.begin(),
                                edges_.end(),
                                [id](const Edge& edge) {
                                    return edge.getSource() == id || edge.getDestination() == id;
                                }),
                 edges_.end());

    adjacencyList_.erase(id);
    vertices_.erase(id);

    return true;
}

bool Graph::containsVertex(VertexID id) const
{
    return vertices_.find(id) != vertices_.end();
}

const Vertex* Graph::getVertex(VertexID id) const
{
    const auto it = vertices_.find(id);
    if (it == vertices_.end())
    {
        return nullptr;
    }

    return &it->second;
}

std::size_t Graph::vertexCount() const noexcept
{
    return vertices_.size();
}

void Graph::clear()
{
    vertices_.clear();
    adjacencyList_.clear();
    edges_.clear();
    nextVertexId_ = 0;
}

bool Graph::addEdge(VertexID source, VertexID destination, double weight)
{
    if (source == destination || !containsVertex(source) || !containsVertex(destination) ||
        containsEdge(source, destination))
    {
        return false;
    }

    adjacencyList_.at(source).push_back(destination);
    adjacencyList_.at(destination).push_back(source);
    edges_.emplace_back(source, destination, weight);

    return true;
}

bool Graph::removeEdge(VertexID source, VertexID destination)
{
    if (!containsEdge(source, destination))
    {
        return false;
    }

    auto& sourceNeighbors = adjacencyList_.at(source);
    sourceNeighbors.erase(std::remove(sourceNeighbors.begin(), sourceNeighbors.end(), destination),
                          sourceNeighbors.end());

    auto& destinationNeighbors = adjacencyList_.at(destination);
    destinationNeighbors.erase(
        std::remove(destinationNeighbors.begin(), destinationNeighbors.end(), source),
        destinationNeighbors.end());

    edges_.erase(std::remove_if(edges_.begin(),
                                edges_.end(),
                                [source, destination](const Edge& edge) {
                                    return (edge.getSource() == source &&
                                            edge.getDestination() == destination) ||
                                           (edge.getSource() == destination &&
                                            edge.getDestination() == source);
                                }),
                 edges_.end());

    return true;
}

bool Graph::containsEdge(VertexID source, VertexID destination) const
{
    const auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end())
    {
        return false;
    }

    const auto destinationIt = adjacencyList_.find(destination);
    if (destinationIt == adjacencyList_.end())
    {
        return false;
    }

    const auto& sourceNeighbors = sourceIt->second;
    return std::find(sourceNeighbors.begin(), sourceNeighbors.end(), destination) !=
           sourceNeighbors.end();
}

std::size_t Graph::edgeCount() const noexcept
{
    return edges_.size();
}

std::vector<VertexID> Graph::neighbors(VertexID id) const
{
    const auto it = adjacencyList_.find(id);
    if (it == adjacencyList_.end())
    {
        return {};
    }

    return it->second;
}

std::size_t Graph::degree(VertexID id) const
{
    const auto it = adjacencyList_.find(id);
    if (it == adjacencyList_.end())
    {
        return 0;
    }

    return it->second.size();
}

bool Graph::isEmpty() const noexcept
{
    return vertices_.empty();
}

std::vector<VertexID> Graph::breadthFirstSearch(VertexID start) const
{
    if (!containsVertex(start))
    {
        return {};
    }

    std::vector<VertexID> order;
    std::queue<VertexID> queue;
    std::unordered_map<VertexID, bool> visited;

    visited[start] = true;
    queue.push(start);

    while (!queue.empty())
    {
        const VertexID current = queue.front();
        queue.pop();
        order.push_back(current);

        const auto adjacencyIt = adjacencyList_.find(current);
        if (adjacencyIt == adjacencyList_.end())
        {
            continue;
        }

        for (VertexID neighbor : adjacencyIt->second)
        {
            if (!visited[neighbor])
            {
                visited[neighbor] = true;
                queue.push(neighbor);
            }
        }
    }

    return order;
}

std::vector<VertexID> Graph::depthFirstSearch(VertexID start) const
{
    if (!containsVertex(start))
    {
        return {};
    }

    std::vector<VertexID> order;
    std::unordered_map<VertexID, bool> visited;

    depthFirstSearchFrom(start, visited, order);

    return order;
}

std::vector<std::vector<VertexID>> Graph::connectedComponents() const
{
    std::vector<std::vector<VertexID>> components;
    std::unordered_map<VertexID, bool> visited;

    for (const auto& entry : vertices_)
    {
        const VertexID vertexId = entry.first;

        if (visited[vertexId])
        {
            continue;
        }

        std::vector<VertexID> component;
        std::queue<VertexID> queue;

        visited[vertexId] = true;
        queue.push(vertexId);

        while (!queue.empty())
        {
            const VertexID current = queue.front();
            queue.pop();
            component.push_back(current);

            const auto adjacencyIt = adjacencyList_.find(current);
            if (adjacencyIt == adjacencyList_.end())
            {
                continue;
            }

            for (VertexID neighbor : adjacencyIt->second)
            {
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    queue.push(neighbor);
                }
            }
        }

        components.push_back(component);
    }

    return components;
}

void Graph::depthFirstSearchFrom(VertexID current,
                                 std::unordered_map<VertexID, bool>& visited,
                                 std::vector<VertexID>& order) const
{
    visited[current] = true;
    order.push_back(current);

    const auto adjacencyIt = adjacencyList_.find(current);
    if (adjacencyIt == adjacencyList_.end())
    {
        return;
    }

    for (VertexID neighbor : adjacencyIt->second)
    {
        if (!visited[neighbor])
        {
            depthFirstSearchFrom(neighbor, visited, order);
        }
    }
}

VertexID Graph::nextAvailableVertexId() const
{
    VertexID candidate = nextVertexId_;

    while (vertices_.find(candidate) != vertices_.end())
    {
        ++candidate;
    }

    return candidate;
}

void Graph::updateNextVertexId(VertexID id) noexcept
{
    if (id >= nextVertexId_)
    {
        nextVertexId_ = id + 1;
    }
}

} // namespace BooleanEngine
