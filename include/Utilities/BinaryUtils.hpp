#pragma once

#include <string>
#include <cstdint>

namespace BooleanEngine
{
namespace BinaryUtils
{

std::string toBinary(uint32_t value, int bits);

uint32_t fromBinary(const std::string& binary);

bool isBinaryString(const std::string& binary);

}
}