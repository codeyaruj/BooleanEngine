#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Core/Exceptions.hpp"
#include "KarnaughMap/KarnaughMap.hpp"
#include "KarnaughMap/KarnaughMapGenerator.hpp"
#include "Utilities/BitUtils.hpp"

using namespace BooleanEngine;

namespace
{

std::vector<int> sorted(std::vector<int> values)
{
    std::sort(values.begin(), values.end());
    return values;
}

std::vector<uint32_t> sorted(std::vector<uint32_t> values)
{
    std::sort(values.begin(), values.end());
    return values;
}

bool containsGroup(const std::vector<std::vector<uint32_t>>& groups, std::vector<uint32_t> expected)
{
    expected = sorted(std::move(expected));

    return std::any_of(groups.begin(), groups.end(), [&expected](std::vector<uint32_t> group) {
        return sorted(std::move(group)) == expected;
    });
}

template <typename Callable>
void assertThrowsKMap(Callable callable)
{
    bool threw = false;

    try
    {
        callable();
    }
    catch (const KarnaughMapException&)
    {
        threw = true;
    }

    assert(threw);
}

void assertGeneralShape(const KarnaughMap& map,
                        uint32_t dimension,
                        std::size_t rows,
                        std::size_t columns)
{
    assert(map.dimension() == dimension);
    assert(map.rows() == rows);
    assert(map.columns() == columns);
    assert(map.cellCount() == rows * columns);
    assert(map.hypercube().dimension() == dimension);
    assert(map.hypercube().vertexCount() == map.cellCount());
    assert(map.hypercube().graph().connectedComponents().size() == 1);
}

void assertLayout(const KarnaughMap& map, const std::vector<std::vector<uint32_t>>& layout)
{
    for (std::size_t row = 0; row < layout.size(); ++row)
    {
        for (std::size_t column = 0; column < layout[row].size(); ++column)
        {
            const BooleanCell* cell = map.getCell(row, column);
            const uint32_t minterm = layout[row][column];

            assert(cell != nullptr);
            assert(cell->row == static_cast<int>(row));
            assert(cell->column == static_cast<int>(column));
            assert(cell->binaryValue == minterm);
            assert(cell->variableAssignment.size() == map.dimension());
            assert(map.containsMinterm(minterm));
            assert(map.getCellFromMinterm(minterm) == cell);
            assert(map.getCell(static_cast<std::size_t>(cell->id)) == cell);
        }
    }
}

void assertMintermNeighborsMatchHypercube(const KarnaughMap& map)
{
    for (uint32_t minterm = 0; minterm < map.cellCount(); ++minterm)
    {
        const auto kmapNeighbors = sorted(map.neighbors(minterm));

        std::vector<int> hypercubeNeighbors;
        for (VertexID neighbor : map.hypercube().neighbors(static_cast<VertexID>(minterm)))
        {
            hypercubeNeighbors.push_back(neighbor);
        }

        assert(kmapNeighbors == sorted(hypercubeNeighbors));

        for (int neighbor : kmapNeighbors)
        {
            assert(BitUtils::hammingDistance(minterm, static_cast<uint32_t>(neighbor)) == 1);
        }
    }
}

void testInvalidInputs()
{
    assertThrowsKMap([] { (void)KarnaughMapGenerator::generate(0); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::generate(1); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::generate(5); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::fromMinterms(4, {0, 0}); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::fromMinterms(4, {16}); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::fromMinterms(4, {1}, {1}); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::fromMaxterms(4, {1}, {1}); });
    assertThrowsKMap([] { (void)KarnaughMapGenerator::fromTruthTable(3, {true, false}); });
}

void testTwoVariableMap()
{
    const KarnaughMap map = KarnaughMapGenerator::generate(2);

    assertGeneralShape(map, 2, 2, 2);
    assertLayout(map, {{0, 1}, {2, 3}});

    assert(map.getCellFromMinterm(0)->row == 0);
    assert(map.getCellFromMinterm(0)->column == 0);
    assert(map.getCellFromMinterm(3)->row == 1);
    assert(map.getCellFromMinterm(3)->column == 1);

    assert(sorted(map.neighbors(0)) == std::vector<int>({1, 2}));
    assert(sorted(map.neighbors(3)) == std::vector<int>({1, 2}));

    for (uint32_t minterm = 0; minterm < map.cellCount(); ++minterm)
    {
        assert(!map.getCellFromMinterm(minterm)->value);
        assert(!map.getCellFromMinterm(minterm)->dontCare);
        assert(map.neighbors(minterm).size() == 2);
        assert(map.getCellFromMinterm(minterm)->neighbors.size() == 2);
    }
}

void testThreeVariableMap()
{
    const KarnaughMap map = KarnaughMapGenerator::generate(3);

    assertGeneralShape(map, 3, 2, 4);
    assertLayout(map, {{0, 1, 3, 2}, {4, 5, 7, 6}});

    assert(map.getCellFromMinterm(2)->row == 0);
    assert(map.getCellFromMinterm(2)->column == 3);
    assert(map.getCellFromMinterm(7)->row == 1);
    assert(map.getCellFromMinterm(7)->column == 2);

    assert(sorted(map.neighbors(0)) == std::vector<int>({1, 2, 4}));
    assert(sorted(map.neighbors(2)) == std::vector<int>({0, 3, 6}));
    assert(sorted(map.neighbors(7)) == std::vector<int>({3, 5, 6}));

    for (uint32_t minterm = 0; minterm < map.cellCount(); ++minterm)
    {
        assert(map.neighbors(minterm).size() == 3);
        assert(map.getCellFromMinterm(minterm)->neighbors.size() == 3);
    }
}

void testFourVariableMap()
{
    const KarnaughMap map = KarnaughMapGenerator::generate(4);

    assertGeneralShape(map, 4, 4, 4);
    assertLayout(map, {{0, 1, 3, 2}, {4, 5, 7, 6}, {12, 13, 15, 14}, {8, 9, 11, 10}});

    assert(map.getCellFromMinterm(10)->row == 3);
    assert(map.getCellFromMinterm(10)->column == 3);
    assert(map.getCellFromMinterm(15)->row == 2);
    assert(map.getCellFromMinterm(15)->column == 2);

    assert(sorted(map.neighbors(0)) == std::vector<int>({1, 2, 4, 8}));
    assert(sorted(map.neighbors(10)) == std::vector<int>({2, 8, 11, 14}));
    assert(sorted(map.neighbors(15)) == std::vector<int>({7, 11, 13, 14}));

    for (uint32_t minterm = 0; minterm < map.cellCount(); ++minterm)
    {
        assert(map.neighbors(minterm).size() == 4);
        assert(map.getCellFromMinterm(minterm)->neighbors.size() == 4);
    }
}

void testConstructionHelpers()
{
    const KarnaughMap mintermMap = KarnaughMapGenerator::fromMinterms(4, {0, 1, 2, 3}, {8, 9});

    assert(mintermMap.getCellFromMinterm(0)->value);
    assert(mintermMap.getCellFromMinterm(3)->value);
    assert(!mintermMap.getCellFromMinterm(8)->value);
    assert(mintermMap.getCellFromMinterm(8)->dontCare);
    assert(mintermMap.getCellFromMinterm(10)->variableAssignment ==
           std::vector<bool>({true, false, true, false}));

    const KarnaughMap maxtermMap = KarnaughMapGenerator::fromMaxterms(2, {0});
    assert(!maxtermMap.getCellFromMinterm(0)->value);
    assert(maxtermMap.getCellFromMinterm(1)->value);
    assert(maxtermMap.simplifySOP() == "A + B");

    const KarnaughMap truthMap =
        KarnaughMapGenerator::fromTruthTable(2, {false, true, true, false});
    assert(truthMap.simplifySOP() == "A'B + AB'");
}

void testGroupingAndSimplification()
{
    const KarnaughMap fullMap = KarnaughMapGenerator::fromMinterms(2, {0, 1, 2, 3});
    assert(fullMap.primeImplicants() == std::vector<std::vector<uint32_t>>({{0, 1, 2, 3}}));
    assert(fullMap.essentialPrimeImplicants() ==
           std::vector<std::vector<uint32_t>>({{0, 1, 2, 3}}));
    assert(fullMap.simplifySOP() == "1");

    const KarnaughMap singletonMap = KarnaughMapGenerator::fromMinterms(2, {0});
    assert(singletonMap.simplifySOP() == "A'B'");

    const KarnaughMap pairMap = KarnaughMapGenerator::fromMinterms(2, {0, 1});
    assert(containsGroup(pairMap.groups(), {0, 1}));
    assert(pairMap.simplifySOP() == "A'");

    const KarnaughMap cornerWrapMap = KarnaughMapGenerator::fromMinterms(4, {0, 2, 8, 10});
    assert(containsGroup(cornerWrapMap.groups(), {0, 2, 8, 10}));
    assert(cornerWrapMap.simplifySOP() == "B'D'");

    const KarnaughMap dontCareMap = KarnaughMapGenerator::fromMinterms(4, {1, 3}, {0, 2});
    assert(containsGroup(dontCareMap.primeImplicants(), {0, 1, 2, 3}));
    assert(dontCareMap.simplifySOP() == "A'B'");
}

void testRepeatedBoundaryAndRendering()
{
    const KarnaughMap map = KarnaughMapGenerator::fromMinterms(4, {0, 10}, {15});

    const BooleanCell* first = map.getCell(0, 0);
    const BooleanCell* firstAgain = map.getCellFromMinterm(0);
    const BooleanCell* corner = map.getCell(3, 3);

    assert(first != nullptr);
    assert(first == firstAgain);
    assert(corner != nullptr);
    assert(corner->binaryValue == 10);
    assert(map.getCell(4, 0) == nullptr);
    assert(map.getCell(0, 4) == nullptr);
    assert(map.getCell(16) == nullptr);
    assert(map.getCellFromMinterm(16) == nullptr);
    assert(map.neighbors(16).empty());

    assert(sorted(first->neighbors) == std::vector<int>({1, 3, 4, 12}));
    assert(sorted(corner->neighbors) == std::vector<int>({3, 11, 12, 14}));

    const std::string rendered = map.renderAscii();
    assert(rendered.find("KMap(4 variables)") != std::string::npos);
    assert(rendered.find('1') != std::string::npos);
    assert(rendered.find('X') != std::string::npos);
}

} // namespace

int main()
{
    testInvalidInputs();
    testTwoVariableMap();
    testThreeVariableMap();
    testFourVariableMap();
    testConstructionHelpers();
    testGroupingAndSimplification();
    testRepeatedBoundaryAndRendering();

    assertMintermNeighborsMatchHypercube(KarnaughMapGenerator::generate(2));
    assertMintermNeighborsMatchHypercube(KarnaughMapGenerator::generate(3));
    assertMintermNeighborsMatchHypercube(KarnaughMapGenerator::generate(4));

    std::cout << "All KarnaughMap tests passed!\n";

    return 0;
}
