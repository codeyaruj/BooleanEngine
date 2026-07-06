#include "Utilities/BinaryUtils.hpp"

#include <bitset>
#include <stdexcept>

namespace BooleanEngine
{
namespace BinaryUtils
{

std::string toBinary(uint32_t value, int bits)
{
    std::string result;

    for (int i = bits - 1; i >= 0; --i)
    {
        result += ((value >> i) & 1) ? '1' : '0';
    }

    return result;
}

uint32_t fromBinary(const std::string& binary)
{
    uint32_t value = 0;

    for (char c : binary)
    {
        value <<= 1;

        if (c == '1')
            value |= 1;
    }

    return value;
}

bool isBinaryString(const std::string& binary)
{
    for (char c : binary)
    {
        if (c != '0' && c != '1')
            return false;
    }

    return true;
}

}
}