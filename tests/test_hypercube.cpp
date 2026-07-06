#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "Hypercube/Hypercube.hpp"
#include "Hypercube/HypercubeGenerator.hpp"
#include "Utilities/BitUtils.hpp"

using namespace BooleanEngine;

namespace
{

std::vector<VertexID> sorted(std::vector<VertexID> values)
{
    std::sort(values.begin(), values.end());
    return values;
}

std::vector<VertexID> expectedVertices(uint32_t dimension)
{
    const VertexID count = static_cast<VertexID>(1u << dimension);
    std::vector<VertexID> vertices;
    vertices.reserve(static_cast<std::size_t>(count));

    for (VertexID id = 0; id < count; ++id)
    {
        vertices.push_back(id);
    }

    return vertices;
}

std::size_t expectedEdgeCount(uint32_t dimension)
{
    if (dimension == 0)
    {
        return 0;
    }

    return static_cast<std::size_t>(dimension) *
           static_cast<std::size_t>(1u << (dimension - 1));
}

void assertTraversalReachesEveryVertex(const Hypercube& cube)
{
    const auto expected = expectedVertices(cube.dimension());

    assert(sorted(cube.graph().breadthFirstSearch(0)) == expected);
    assert(sorted(cube.graph().depthFirstSearch(0)) == expected);
}

void assertHypercubeShape(uint32_t dimension)
{
    const Hypercube cube = HypercubeGenerator::generate(dimension);
    const std::size_t vertexCount = static_cast<std::size_t>(1u << dimension);

    assert(cube.dimension() == dimension);
    assert(cube.vertexCount() == vertexCount);
    assert(cube.edgeCount() == expectedEdgeCount(dimension));

    for (VertexID id = 0; id < static_cast<VertexID>(vertexCount); ++id)
    {
        assert(cube.containsVertex(id));
        assert(cube.getVertex(id) != nullptr);
        assert(cube.neighbors(id).size() == dimension);

        for (VertexID neighbor : cube.neighbors(id))
        {
            assert(cube.containsVertex(neighbor));
            assert(cube.graph().containsEdge(id, neighbor));
            assert(BitUtils::hammingDistance(static_cast<uint32_t>(id),
                                             static_cast<uint32_t>(neighbor)) == 1);
        }
    }

    assert(!cube.containsVertex(static_cast<VertexID>(vertexCount)));
    assert(cube.getVertex(static_cast<VertexID>(vertexCount)) == nullptr);
    assert(cube.graph().connectedComponents().size() == 1);
    assertTraversalReachesEveryVertex(cube);
}

void testDimensionZero()
{
    const Hypercube cube = HypercubeGenerator::generate(0);

    assert(cube.dimension() == 0);
    assert(cube.vertexCount() == 1);
    assert(cube.edgeCount() == 0);
    assert(cube.containsVertex(0));
    assert(!cube.containsVertex(1));
    assert(cube.neighbors(0).empty());
    assert(cube.graph().breadthFirstSearch(0) == std::vector<VertexID>({0}));
    assert(cube.graph().depthFirstSearch(0) == std::vector<VertexID>({0}));

    const Vertex* vertex = cube.getVertex(0);
    assert(vertex != nullptr);
    assert(vertex->getId() == 0);
    assert(vertex->getLabel().empty());
}

void testDimensionOne()
{
    const Hypercube cube = HypercubeGenerator::generate(1);

    assert(cube.vertexCount() == 2);
    assert(cube.edgeCount() == 1);
    assert(cube.graph().containsEdge(0, 1));
    assert(cube.neighbors(0) == std::vector<VertexID>({1}));
    assert(cube.neighbors(1) == std::vector<VertexID>({0}));

    const Vertex* zero = cube.getVertex(0);
    const Vertex* one = cube.getVertex(1);

    assert(zero != nullptr);
    assert(one != nullptr);
    assert(zero->getLabel() == "0");
    assert(one->getLabel() == "1");
}

void testDimensionTwo()
{
    const Hypercube cube = HypercubeGenerator::generate(2);

    assert(cube.vertexCount() == 4);
    assert(cube.edgeCount() == 4);

    for (VertexID id = 0; id < 4; ++id)
    {
        assert(cube.neighbors(id).size() == 2);
    }

    assert(cube.graph().containsEdge(0, 1));
    assert(cube.graph().containsEdge(0, 2));
    assert(!cube.graph().containsEdge(0, 3));
}

void testDimensionThree()
{
    const Hypercube cube = HypercubeGenerator::generate(3);

    assert(cube.vertexCount() == 8);
    assert(cube.edgeCount() == 12);

    for (VertexID id = 0; id < 8; ++id)
    {
        assert(cube.neighbors(id).size() == 3);
    }

    assert(cube.graph().containsEdge(0, 1));
    assert(cube.graph().containsEdge(0, 2));
    assert(cube.graph().containsEdge(0, 4));
    assert(!cube.graph().containsEdge(0, 3));

    const Vertex* vertex = cube.getVertex(5);
    assert(vertex != nullptr);
    assert(vertex->getLabel() == "101");
}

void testDimensionFour()
{
    const Hypercube cube = HypercubeGenerator::generate(4);

    assert(cube.vertexCount() == 16);
    assert(cube.edgeCount() == 32);

    for (VertexID id = 0; id < 16; ++id)
    {
        assert(cube.neighbors(id).size() == 4);
    }

    assertTraversalReachesEveryVertex(cube);
}

void testGenericLargerDimension()
{
    const Hypercube cube = HypercubeGenerator::generate(5);

    assert(cube.dimension() == 5);
    assert(cube.vertexCount() == 32);
    assert(cube.edgeCount() == 80);

    for (VertexID id = 0; id < 32; ++id)
    {
        assert(cube.neighbors(id).size() == 5);
    }

    assertTraversalReachesEveryVertex(cube);
}

void testDirectHypercubeConstructor()
{
    const Hypercube cube(7);

    assert(cube.dimension() == 7);
    assert(cube.vertexCount() == 0);
    assert(cube.edgeCount() == 0);
    assert(!cube.containsVertex(0));
    assert(cube.getVertex(0) == nullptr);
    assert(cube.neighbors(0).empty());
}

} // namespace

int main()
{
    testDimensionZero();
    testDimensionOne();
    testDimensionTwo();
    testDimensionThree();
    testDimensionFour();
    testGenericLargerDimension();
    testDirectHypercubeConstructor();

    assertHypercubeShape(1);
    assertHypercubeShape(2);
    assertHypercubeShape(3);
    assertHypercubeShape(4);

    std::cout << "All Hypercube tests passed!\n";

    return 0;
}
