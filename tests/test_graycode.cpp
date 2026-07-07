#include <cassert>
#include <iostream>
#include <stdexcept>

#include "GrayCode/GrayCode.hpp"

namespace
{

template <typename Callable>
void assertThrows(Callable callable)
{
    bool threw = false;

    try
    {
        callable();
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    assert(threw);
}

} // namespace

int main()
{
    // Binary -> Gray
    assert(GrayCode::binaryToGray(0) == 0);
    assert(GrayCode::binaryToGray(1) == 1);
    assert(GrayCode::binaryToGray(2) == 3);
    assert(GrayCode::binaryToGray(3) == 2);
    assert(GrayCode::binaryToGray(11) == 14);

    // Gray -> Binary
    assert(GrayCode::grayToBinary(0) == 0);
    assert(GrayCode::grayToBinary(1) == 1);
    assert(GrayCode::grayToBinary(3) == 2);
    assert(GrayCode::grayToBinary(2) == 3);
    assert(GrayCode::grayToBinary(14) == 11);

    // Sequence
    auto seq = GrayCode::generateSequence(2);

    assert(seq.size() == 4);

    assert(seq[0] == 0);
    assert(seq[1] == 1);
    assert(seq[2] == 3);
    assert(seq[3] == 2);
    assertThrows([] { (void)GrayCode::generateSequence(32); });

    // Adjacency
    assert(GrayCode::isAdjacent(0, 1));
    assert(GrayCode::isAdjacent(1, 3));
    assert(GrayCode::isAdjacent(3, 2));
    assert(!GrayCode::isAdjacent(0, 3));

    std::cout << "All Gray Code tests passed!\n";

    return 0;
}
