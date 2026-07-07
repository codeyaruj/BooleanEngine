#include "GrayCode/GrayCode.hpp"

#include <limits>
#include <stdexcept>

namespace GrayCode
{

unsigned int binaryToGray(unsigned int binary)
{
    return binary ^ (binary >> 1);
}

unsigned int grayToBinary(unsigned int gray)
{
    unsigned int binary = gray;

    while (gray >>= 1)
    {
        binary ^= gray;
    }

    return binary;
}

std::vector<unsigned int> generateSequence(unsigned int bits)
{
    if (bits >= std::numeric_limits<unsigned int>::digits)
    {
        throw std::out_of_range("Gray code bit width is too large");
    }

    std::vector<unsigned int> sequence;

    unsigned int total = 1u << bits;

    sequence.reserve(total);

    for (unsigned int i = 0; i < total; ++i)
    {
        sequence.push_back(binaryToGray(i));
    }

    return sequence;
}

bool isAdjacent(unsigned int a, unsigned int b)
{
    unsigned int diff = a ^ b;

    if (diff == 0)
        return false;

    return (diff & (diff - 1)) == 0;
}

}
