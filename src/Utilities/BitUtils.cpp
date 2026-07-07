#include "Utilities/BitUtils.hpp"

#include <limits>
#include <stdexcept>

namespace BooleanEngine
{
namespace BitUtils
{

namespace
{

void validateBitPosition(int position)
{
    if (position < 0 || position >= static_cast<int>(std::numeric_limits<uint32_t>::digits))
    {
        throw std::out_of_range("Bit position must be between 0 and 31");
    }
}

} // namespace

int popcount(uint32_t value)
{
    return __builtin_popcount(value);
}

bool getBit(uint32_t value, int position)
{
    validateBitPosition(position);
    return (value >> position) & 1;
}

uint32_t setBit(uint32_t value, int position)
{
    validateBitPosition(position);
    return value | (1u << position);
}

uint32_t clearBit(uint32_t value, int position)
{
    validateBitPosition(position);
    return value & ~(1u << position);
}

uint32_t toggleBit(uint32_t value, int position)
{
    validateBitPosition(position);
    return value ^ (1u << position);
}

int hammingDistance(uint32_t a, uint32_t b)
{
    return popcount(a ^ b);
}

}
}
