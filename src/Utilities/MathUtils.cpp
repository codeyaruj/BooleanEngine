#include "Utilities/MathUtils.hpp"

#include <limits>
#include <stdexcept>

namespace BooleanEngine
{
namespace MathUtils
{

bool isPowerOfTwo(int number)
{
    if (number <= 0)
        return false;

    return (number & (number - 1)) == 0;
}

int powerOfTwo(int exponent)
{
    if (exponent < 0 || exponent >= static_cast<int>(std::numeric_limits<int>::digits))
    {
        throw std::out_of_range("Exponent is outside the safe range for int powers of two");
    }

    return 1 << exponent;
}

}
}
