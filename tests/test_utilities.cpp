#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Utilities/BitUtils.hpp"
#include "Utilities/BinaryUtils.hpp"
#include "Utilities/MathUtils.hpp"

using namespace BooleanEngine;

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
    // -----------------------------
    // popcount()
    // -----------------------------
    assert(BitUtils::popcount(0) == 0);
    assert(BitUtils::popcount(15) == 4);
    assert(BitUtils::popcount(255) == 8);

    // -----------------------------
    // getBit()
    // -----------------------------
    assert(BitUtils::getBit(5, 0) == true);   // 101
    assert(BitUtils::getBit(5, 1) == false);
    assert(BitUtils::getBit(5, 2) == true);
    assertThrows([] { (void)BitUtils::getBit(5, -1); });
    assertThrows([] { (void)BitUtils::getBit(5, 32); });

    // -----------------------------
    // setBit()
    // -----------------------------
    assert(BitUtils::setBit(4, 0) == 5);

    // -----------------------------
    // clearBit()
    // -----------------------------
    assert(BitUtils::clearBit(5, 0) == 4);

    // -----------------------------
    // toggleBit()
    // -----------------------------
    assert(BitUtils::toggleBit(5, 0) == 4);
    assert(BitUtils::toggleBit(4, 0) == 5);

    // -----------------------------
    // Hamming Distance
    // -----------------------------
    assert(BitUtils::hammingDistance(5, 7) == 1);   //101 vs111
    assert(BitUtils::hammingDistance(5, 2) == 3);   //101 vs010

    // -----------------------------
    // Binary Conversion
    // -----------------------------
    assert(BinaryUtils::toBinary(5, 4) == "0101");
    assert(BinaryUtils::toBinary(15, 4) == "1111");
    assert(BinaryUtils::toBinary(0, 4) == "0000");

    assert(BinaryUtils::fromBinary("1010") == 10);
    assert(BinaryUtils::fromBinary("1111") == 15);
    assert(BinaryUtils::fromBinary("0000") == 0);
    assertThrows([] { (void)BinaryUtils::toBinary(1, -1); });
    assertThrows([] { (void)BinaryUtils::fromBinary("1021"); });

    // -----------------------------
    // Binary Validation
    // -----------------------------
    assert(BinaryUtils::isBinaryString("101010"));
    assert(BinaryUtils::isBinaryString("0000"));
    assert(!BinaryUtils::isBinaryString("1021"));
    assert(!BinaryUtils::isBinaryString("ABCD"));

    // -----------------------------
    // Math Utilities
    // -----------------------------
    assert(MathUtils::isPowerOfTwo(1));
    assert(MathUtils::isPowerOfTwo(2));
    assert(MathUtils::isPowerOfTwo(8));
    assert(MathUtils::isPowerOfTwo(1024));

    assert(!MathUtils::isPowerOfTwo(0));
    assert(!MathUtils::isPowerOfTwo(3));
    assert(!MathUtils::isPowerOfTwo(10));

    assert(MathUtils::powerOfTwo(0) == 1);
    assert(MathUtils::powerOfTwo(3) == 8);
    assert(MathUtils::powerOfTwo(5) == 32);
    assertThrows([] { (void)MathUtils::powerOfTwo(-1); });

    std::cout << "=====================================\n";
    std::cout << "All Utilities Tests Passed Successfully!\n";
    std::cout << "=====================================\n";

    return 0;
}
