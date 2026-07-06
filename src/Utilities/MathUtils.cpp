#include "Utilities/MathUtils.hpp"

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
    return 1 << exponent;
}

}
}