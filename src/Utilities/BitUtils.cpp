#include "Utilities/BitUtils.hpp"

namespace BooleanEngine
{
namespace BitUtils
{

int popcount(uint32_t value)
{
    return __builtin_popcount(value);
}

bool getBit(uint32_t value, int position)
{
    return (value >> position) & 1;
}

uint32_t setBit(uint32_t value, int position)
{
    return value | (1u << position);
}

uint32_t clearBit(uint32_t value, int position)
{
    return value & ~(1u << position);
}

uint32_t toggleBit(uint32_t value, int position)
{
    return value ^ (1u << position);
}

int hammingDistance(uint32_t a, uint32_t b)
{
    return popcount(a ^ b);
}

}
}