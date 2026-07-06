#ifndef GRAY_CODE_H
#define GRAY_CODE_H

#include <vector>

namespace GrayCode
{
    /**
     * @brief Converts a binary number to its Gray code representation.
     *
     * Formula:
     * Gray = Binary ^ (Binary >> 1)
     *
     * @param binary Binary number.
     * @return Equivalent Gray code.
     */
    unsigned int binaryToGray(unsigned int binary);

    /**
     * @brief Converts a Gray code number back to binary.
     *
     * Uses cumulative XOR to reconstruct the original binary value.
     *
     * @param gray Gray code number.
     * @return Equivalent binary number.
     */
    unsigned int grayToBinary(unsigned int gray);

    /**
     * @brief Generates the Reflected Binary Gray Code sequence.
     *
     * Example (bits = 2):
     * 00, 01, 11, 10
     *
     * @param bits Number of bits.
     * @return Vector containing the Gray code sequence.
     */
    std::vector<unsigned int> generateSequence(unsigned int bits);

    /**
     * @brief Checks whether two codes differ by exactly one bit.
     *
     * This property is fundamental to Gray codes and Karnaugh Maps.
     *
     * @param a First code.
     * @param b Second code.
     * @return true if they differ by exactly one bit.
     */
    bool isAdjacent(unsigned int a, unsigned int b);
}

#endif // GRAY_CODE_H