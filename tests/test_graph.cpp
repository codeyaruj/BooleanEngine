#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "Graph/Edge.hpp"
#include "Graph/Graph.hpp"
#include "Graph/Vertex.hpp"

using namespace BooleanEngine;

namespace
{

std::vector<VertexID> sorted(std::vector<VertexID> values)
{
    std::sort(values.begin(), values.end());
    return values;
}

std::vector<std::vector<VertexID>> normalizedComponents(
    std::vector<std::vector<VertexID>> components)
{
    for (auto& component : components)
    {
        std::sort(component.begin(), component.end());
    }

    std::sort(components.begin(), components.end());
    return components;
}

void testVertex()
{
    Vertex vertex(7, "alpha");

    assert(vertex.getId() == 7);
    assert(vertex.getLabel() == "alpha");
    assert(!vertex.isVisited());

    vertex.setId(8);
    vertex.setLabel("beta");
    vertex.markVisited();

    assert(vertex.getId() == 8);
    assert(vertex.getLabel() == "beta");
    assert(vertex.isVisited());

    vertex.clearVisited();
    assert(!vertex.isVisited());

    vertex.setVisited(true);
    assert(vertex.isVisited());
}

void testEdge()
{
    Edge defaultEdge;
    assert(defaultEdge.getSource() == 0);
    assert(defaultEdge.getDestination() == 0);
    assert(defaultEdge.getWeight() == 1.0);

    Edge edge(1, 2, 3.5);
    assert(edge.getSource() == 1);
    assert(edge.getDestination() == 2);
    assert(edge.getWeight() == 3.5);
}

void testEmptyGraph()
{
    Graph graph;

    assert(graph.isEmpty());
    assert(graph.vertexCount() == 0);
    assert(graph.edgeCount() == 0);
    assert(!graph.containsVertex(1));
    assert(!graph.containsEdge(1, 2));
    assert(graph.neighbors(1).empty());
    assert(graph.degree(1) == 0);
    assert(graph.breadthFirstSearch(1).empty());
    assert(graph.depthFirstSearch(1).empty());
    assert(graph.connectedComponents().empty());
}

void testAddingVertices()
{
    Graph graph;

    assert(graph.addVertex(1, "one"));
    assert(graph.addVertex(Vertex(2, "two")));

    const VertexID generatedId = graph.addVertex();
    const VertexID generatedLabelId = graph.addVertex("generated");

    assert(generatedId == 3);
    assert(generatedLabelId == 4);
    assert(graph.vertexCount() == 4);
    assert(graph.containsVertex(1));
    assert(graph.containsVertex(2));
    assert(graph.containsVertex(generatedId));
    assert(graph.containsVertex(generatedLabelId));
    assert(!graph.isEmpty());

    const Vertex* vertex = graph.getVertex(1);
    assert(vertex != nullptr);
    assert(vertex->getId() == 1);
    assert(vertex->getLabel() == "one");
    assert(graph.getVertex(99) == nullptr);
}

void testDuplicateVertices()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(!graph.addVertex(1));
    assert(!graph.addVertex(Vertex(1, "duplicate")));
    assert(graph.vertexCount() == 1);
}

void testRemovingVertices()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(graph.addVertex(2));
    assert(graph.addVertex(3));
    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(2, 3));

    assert(graph.removeVertex(2));
    assert(!graph.containsVertex(2));
    assert(graph.vertexCount() == 2);
    assert(graph.edgeCount() == 0);
    assert(!graph.containsEdge(1, 2));
    assert(!graph.containsEdge(2, 3));
    assert(graph.degree(1) == 0);
    assert(graph.degree(3) == 0);

    assert(!graph.removeVertex(42));
}

void testAddingEdges()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(graph.addVertex(2));
    assert(graph.addVertex(3));

    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(2, 3, 2.5));
    assert(graph.edgeCount() == 2);
    assert(graph.containsEdge(1, 2));
    assert(graph.containsEdge(2, 1));
    assert(graph.containsEdge(2, 3));

    assert(!graph.addEdge(1, 2));
    assert(!graph.addEdge(1, 1));
    assert(!graph.addEdge(1, 99));
    assert(!graph.addEdge(99, 1));
    assert(graph.edgeCount() == 2);
}

void testRemovingEdges()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(graph.addVertex(2));
    assert(graph.addVertex(3));
    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(2, 3));

    assert(graph.removeEdge(2, 1));
    assert(!graph.containsEdge(1, 2));
    assert(graph.containsEdge(2, 3));
    assert(graph.edgeCount() == 1);
    assert(graph.degree(1) == 0);
    assert(graph.degree(2) == 1);

    assert(!graph.removeEdge(1, 2));
    assert(!graph.removeEdge(1, 99));
}

void testNeighborsAndDegree()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(graph.addVertex(2));
    assert(graph.addVertex(3));
    assert(graph.addVertex(4));
    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(1, 3));
    assert(graph.addEdge(1, 4));

    assert(graph.degree(1) == 3);
    assert(graph.degree(2) == 1);
    assert(sorted(graph.neighbors(1)) == std::vector<VertexID>({2, 3, 4}));
    assert(graph.neighbors(99).empty());
    assert(graph.degree(99) == 0);
}

void testBreadthFirstSearch()
{
    Graph graph;

    for (VertexID id = 1; id <= 5; ++id)
    {
        assert(graph.addVertex(id));
    }

    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(1, 3));
    assert(graph.addEdge(2, 4));
    assert(graph.addEdge(3, 5));

    const auto order = graph.breadthFirstSearch(1);
    assert(order == std::vector<VertexID>({1, 2, 3, 4, 5}));
    assert(graph.breadthFirstSearch(99).empty());
}

void testDepthFirstSearch()
{
    Graph graph;

    for (VertexID id = 1; id <= 5; ++id)
    {
        assert(graph.addVertex(id));
    }

    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(1, 3));
    assert(graph.addEdge(2, 4));
    assert(graph.addEdge(3, 5));

    const auto order = graph.depthFirstSearch(1);
    assert(order == std::vector<VertexID>({1, 2, 4, 3, 5}));
    assert(graph.depthFirstSearch(99).empty());
}

void testConnectedComponents()
{
    Graph graph;

    for (VertexID id = 1; id <= 6; ++id)
    {
        assert(graph.addVertex(id));
    }

    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(2, 3));
    assert(graph.addEdge(4, 5));

    auto components = normalizedComponents(graph.connectedComponents());
    const std::vector<std::vector<VertexID>> expected{{1, 2, 3}, {4, 5}, {6}};

    assert(components == expected);
}

void testSingleVertexGraph()
{
    Graph graph;

    assert(graph.addVertex(10));
    assert(graph.vertexCount() == 1);
    assert(graph.edgeCount() == 0);
    assert(graph.degree(10) == 0);
    assert(graph.breadthFirstSearch(10) == std::vector<VertexID>({10}));
    assert(graph.depthFirstSearch(10) == std::vector<VertexID>({10}));
    assert(normalizedComponents(graph.connectedComponents()) ==
           std::vector<std::vector<VertexID>>({{10}}));
}

void testDisconnectedGraph()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(graph.addVertex(2));
    assert(graph.addVertex(3));

    assert(graph.breadthFirstSearch(1) == std::vector<VertexID>({1}));
    assert(graph.depthFirstSearch(2) == std::vector<VertexID>({2}));
    assert(normalizedComponents(graph.connectedComponents()) ==
           std::vector<std::vector<VertexID>>({{1}, {2}, {3}}));
}

void testFullyConnectedGraph()
{
    Graph graph;

    for (VertexID id = 1; id <= 4; ++id)
    {
        assert(graph.addVertex(id));
    }

    assert(graph.addEdge(1, 2));
    assert(graph.addEdge(1, 3));
    assert(graph.addEdge(1, 4));
    assert(graph.addEdge(2, 3));
    assert(graph.addEdge(2, 4));
    assert(graph.addEdge(3, 4));

    assert(graph.edgeCount() == 6);

    for (VertexID id = 1; id <= 4; ++id)
    {
        assert(graph.degree(id) == 3);
    }

    assert(normalizedComponents(graph.connectedComponents()) ==
           std::vector<std::vector<VertexID>>({{1, 2, 3, 4}}));
}

void testClear()
{
    Graph graph;

    assert(graph.addVertex(1));
    assert(graph.addVertex(2));
    assert(graph.addEdge(1, 2));
    graph.clear();

    assert(graph.isEmpty());
    assert(graph.vertexCount() == 0);
    assert(graph.edgeCount() == 0);
    assert(graph.neighbors(1).empty());

    const VertexID generatedId = graph.addVertex();
    assert(generatedId == 0);
}

} // namespace

int main()
{
    testVertex();
    testEdge();
    testEmptyGraph();
    testAddingVertices();
    testDuplicateVertices();
    testRemovingVertices();
    testAddingEdges();
    testRemovingEdges();
    testNeighborsAndDegree();
    testBreadthFirstSearch();
    testDepthFirstSearch();
    testConnectedComponents();
    testSingleVertexGraph();
    testDisconnectedGraph();
    testFullyConnectedGraph();
    testClear();

    std::cout << "All Graph tests passed!\n";

    return 0;
}
