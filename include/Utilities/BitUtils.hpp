#pragma once

#include <cstdint>

namespace BooleanEngine
{
namespace BitUtils
{

// Returns the number of set bits (Hamming Weight)
int popcount(uint32_t value);

// Returns true if the bit at 'position' is 1
bool getBit(uint32_t value, int position);

// Sets a bit
uint32_t setBit(uint32_t value, int position);

// Clears a bit
uint32_t clearBit(uint32_t value, int position);

// Toggles a bit
uint32_t toggleBit(uint32_t value, int position);

// Returns Hamming Distance between two integers
int hammingDistance(uint32_t a, uint32_t b);

}
}