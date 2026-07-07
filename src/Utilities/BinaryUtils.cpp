#include "Utilities/BinaryUtils.hpp"

#include <limits>
#include <stdexcept>

namespace BooleanEngine
{
namespace BinaryUtils
{

std::string toBinary(uint32_t value, int bits)
{
    if (bits < 0 || bits > static_cast<int>(std::numeric_limits<uint32_t>::digits))
    {
        throw std::out_of_range("Binary width must be between 0 and 32 bits");
    }

    std::string result;
    result.reserve(static_cast<std::size_t>(bits));

    for (int i = bits - 1; i >= 0; --i)
    {
        result += ((value >> i) & 1) ? '1' : '0';
    }

    return result;
}

uint32_t fromBinary(const std::string& binary)
{
    if (binary.size() > std::numeric_limits<uint32_t>::digits)
    {
        throw std::out_of_range("Binary string is too wide for uint32_t");
    }

    uint32_t value = 0;

    for (char c : binary)
    {
        if (c != '0' && c != '1')
        {
            throw std::invalid_argument("Binary string contains a non-binary character");
        }

        value <<= 1;

        if (c == '1')
        {
            value |= 1;
        }
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
