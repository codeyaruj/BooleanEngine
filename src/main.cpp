#include <iostream>
#include <bitset>
#include <vector>

#include "GrayCode/GrayCode.hpp"

int main()
{
    constexpr unsigned int bits = 4;

    unsigned int binary = 0b1011;
    unsigned int gray = GrayCode::binaryToGray(binary);
    unsigned int recovered = GrayCode::grayToBinary(gray);

    std::cout << "Binary          : "
              << std::bitset<bits>(binary) << '\n';

    std::cout << "Gray            : "
              << std::bitset<bits>(gray) << '\n';

    std::cout << "Recovered Binary: "
              << std::bitset<bits>(recovered) << "\n\n";

    std::cout << bits << "-bit Gray Sequence\n";

    std::vector<unsigned int> sequence = GrayCode::generateSequence(bits);

    for (unsigned int value : sequence)
    {
        std::cout << std::bitset<bits>(value) << '\n';
    }

    std::cout << "\nAdjacency Tests\n";

    unsigned int a = 0b0011;
    unsigned int b = 0b0010;
    unsigned int c = 0b0110;

    std::cout << std::bitset<bits>(a)
              << " and "
              << std::bitset<bits>(b)
              << " : "
              << (GrayCode::isAdjacent(a, b) ? "Adjacent" : "Not Adjacent")
              << '\n';

    std::cout << std::bitset<bits>(a)
              << " and "
              << std::bitset<bits>(c)
              << " : "
              << (GrayCode::isAdjacent(a, c) ? "Adjacent" : "Not Adjacent")
              << '\n';

    return 0;
}