#include "KarnaughMap/KarnaughMap.hpp"

#include "Core/Exceptions.hpp"
#include "GrayCode/GrayCode.hpp"
#include "Utilities/BinaryUtils.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace BooleanEngine
{
namespace
{

std::size_t invalidIndex() noexcept
{
    return std::numeric_limits<std::size_t>::max();
}

bool containsAny(const std::vector<uint32_t>& haystack, const std::vector<uint32_t>& needles)
{
    return std::any_of(needles.begin(), needles.end(), [&haystack](uint32_t needle) {
        return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
    });
}

bool isSubsetOf(const std::vector<uint32_t>& subset, const std::vector<uint32_t>& superset)
{
    return std::all_of(subset.begin(), subset.end(), [&superset](uint32_t value) {
        return std::find(superset.begin(), superset.end(), value) != superset.end();
    });
}

std::vector<uint32_t> sortedUnique(std::vector<uint32_t> values)
{
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    return values;
}

std::size_t freeBitCountForGroupSize(std::size_t groupSize)
{
    std::size_t freeBits = 0;

    while (groupSize > 1)
    {
        groupSize >>= 1;
        ++freeBits;
    }

    return freeBits;
}

} // namespace

KarnaughMap::KarnaughMap(uint32_t variableCount,
                         std::size_t rows,
                         std::size_t columns,
                         std::vector<BooleanCell> cells,
                         Hypercube hypercube)
    : variableCount_(variableCount),
      rows_(rows),
      columns_(columns),
      cells_(std::move(cells)),
      hypercube_(std::move(hypercube))
{
    validate();
    buildLookupTables();
}

uint32_t KarnaughMap::dimension() const noexcept
{
    return variableCount_;
}

std::size_t KarnaughMap::rows() const noexcept
{
    return rows_;
}

std::size_t KarnaughMap::columns() const noexcept
{
    return columns_;
}

std::size_t KarnaughMap::cellCount() const noexcept
{
    return cells_.size();
}

const BooleanCell* KarnaughMap::getCell(std::size_t index) const
{
    if (index >= cells_.size())
    {
        return nullptr;
    }

    return &cells_[index];
}

const BooleanCell* KarnaughMap::getCell(std::size_t row, std::size_t column) const
{
    if (row >= rows_ || column >= columns_ || coordinateToCellIndex_.empty())
    {
        return nullptr;
    }

    const std::size_t index = coordinateToCellIndex_[coordinateIndex(row, column)];
    if (index == invalidIndex())
    {
        return nullptr;
    }

    return &cells_[index];
}

const BooleanCell* KarnaughMap::getCellFromMinterm(uint32_t minterm) const
{
    if (minterm >= mintermToCellIndex_.size())
    {
        return nullptr;
    }

    const std::size_t index = mintermToCellIndex_[minterm];
    if (index == invalidIndex())
    {
        return nullptr;
    }

    return &cells_[index];
}

bool KarnaughMap::containsMinterm(uint32_t minterm) const
{
    return getCellFromMinterm(minterm) != nullptr;
}

std::vector<int> KarnaughMap::neighbors(uint32_t minterm) const
{
    const BooleanCell* cell = getCellFromMinterm(minterm);
    if (cell == nullptr)
    {
        return {};
    }

    std::vector<int> neighborMinterms;
    neighborMinterms.reserve(cell->neighbors.size());

    for (int neighborCellIndex : cell->neighbors)
    {
        const BooleanCell* neighborCell = getCell(static_cast<std::size_t>(neighborCellIndex));
        if (neighborCell != nullptr)
        {
            neighborMinterms.push_back(static_cast<int>(neighborCell->binaryValue));
        }
    }

    return neighborMinterms;
}

std::vector<std::vector<uint32_t>> KarnaughMap::groups() const
{
    const std::vector<uint32_t> ones = oneMinterms();
    const std::vector<uint32_t> active = activeMinterms();

    if (ones.empty())
    {
        return {};
    }

    std::vector<std::vector<uint32_t>> groups;
    std::set<std::vector<uint32_t>> seen;
    const uint32_t variableCombinations = 1u << variableCount_;

    for (uint32_t freeMask = 0; freeMask < variableCombinations; ++freeMask)
    {
        const uint32_t fixedMask = (variableCombinations - 1u) ^ freeMask;

        for (uint32_t pattern = 0; pattern < variableCombinations; ++pattern)
        {
            std::vector<uint32_t> group;

            for (uint32_t minterm = 0; minterm < variableCombinations; ++minterm)
            {
                if ((minterm & fixedMask) == (pattern & fixedMask))
                {
                    group.push_back(minterm);
                }
            }

            group = sortedUnique(std::move(group));

            if (!group.empty() && isSubsetOf(group, active) && containsAny(group, ones) &&
                seen.insert(group).second)
            {
                groups.push_back(group);
            }
        }
    }

    std::sort(groups.begin(), groups.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.size() != rhs.size())
        {
            return lhs.size() > rhs.size();
        }

        return lhs < rhs;
    });

    return groups;
}

std::vector<std::vector<uint32_t>> KarnaughMap::primeImplicants() const
{
    const auto allGroups = groups();
    std::vector<std::vector<uint32_t>> primes;

    for (const auto& candidate : allGroups)
    {
        const bool coveredByLargerGroup =
            std::any_of(allGroups.begin(), allGroups.end(), [&candidate](const auto& other) {
                return other.size() > candidate.size() && isSubsetOf(candidate, other);
            });

        if (!coveredByLargerGroup)
        {
            primes.push_back(candidate);
        }
    }

    return primes;
}

std::vector<std::vector<uint32_t>> KarnaughMap::essentialPrimeImplicants() const
{
    const auto primes = primeImplicants();
    const auto ones = oneMinterms();
    std::vector<std::vector<uint32_t>> essentials;

    for (const auto& prime : primes)
    {
        bool essential = false;

        for (uint32_t minterm : ones)
        {
            if (std::find(prime.begin(), prime.end(), minterm) == prime.end())
            {
                continue;
            }

            const std::size_t coveringPrimeCount =
                static_cast<std::size_t>(std::count_if(primes.begin(),
                                                       primes.end(),
                                                       [minterm](const auto& otherPrime) {
                                                           return std::find(otherPrime.begin(),
                                                                            otherPrime.end(),
                                                                            minterm) !=
                                                                  otherPrime.end();
                                                       }));

            if (coveringPrimeCount == 1)
            {
                essential = true;
                break;
            }
        }

        if (essential)
        {
            essentials.push_back(prime);
        }
    }

    return essentials;
}

std::string KarnaughMap::simplifySOP() const
{
    const auto ones = oneMinterms();
    if (ones.empty())
    {
        return "0";
    }

    if (ones.size() == cells_.size())
    {
        return "1";
    }

    const auto primes = primeImplicants();
    if (primes.empty())
    {
        return "0";
    }

    std::vector<std::vector<uint32_t>> chosen = essentialPrimeImplicants();
    std::vector<uint32_t> covered;

    for (const auto& group : chosen)
    {
        covered.insert(covered.end(), group.begin(), group.end());
    }

    covered = sortedUnique(std::move(covered));

    std::vector<uint32_t> remainingOnes;
    for (uint32_t minterm : ones)
    {
        if (std::find(covered.begin(), covered.end(), minterm) == covered.end())
        {
            remainingOnes.push_back(minterm);
        }
    }

    std::vector<std::vector<uint32_t>> candidatePrimes;
    for (const auto& prime : primes)
    {
        if (std::find(chosen.begin(), chosen.end(), prime) == chosen.end() &&
            containsAny(prime, remainingOnes))
        {
            candidatePrimes.push_back(prime);
        }
    }

    std::vector<std::vector<uint32_t>> bestExtra;
    std::size_t bestLiteralCount = std::numeric_limits<std::size_t>::max();
    const std::size_t subsetCount = static_cast<std::size_t>(1) << candidatePrimes.size();

    for (std::size_t mask = 0; mask < subsetCount; ++mask)
    {
        std::vector<uint32_t> subsetCoverage = covered;
        std::vector<std::vector<uint32_t>> subsetGroups;
        std::size_t literalCount = 0;

        for (std::size_t index = 0; index < candidatePrimes.size(); ++index)
        {
            if ((mask & (static_cast<std::size_t>(1u) << index)) == 0)
            {
                continue;
            }

            subsetGroups.push_back(candidatePrimes[index]);
            subsetCoverage.insert(subsetCoverage.end(),
                                  candidatePrimes[index].begin(),
                                  candidatePrimes[index].end());
            literalCount += static_cast<std::size_t>(variableCount_) -
                            freeBitCountForGroupSize(candidatePrimes[index].size());
        }

        subsetCoverage = sortedUnique(std::move(subsetCoverage));

        if (!isSubsetOf(ones, subsetCoverage))
        {
            continue;
        }

        if (bestExtra.empty() || subsetGroups.size() < bestExtra.size() ||
            (subsetGroups.size() == bestExtra.size() && literalCount < bestLiteralCount))
        {
            bestExtra = subsetGroups;
            bestLiteralCount = literalCount;
        }
    }

    chosen.insert(chosen.end(), bestExtra.begin(), bestExtra.end());

    if (chosen.empty())
    {
        return "0";
    }

    std::vector<std::string> terms;
    terms.reserve(chosen.size());

    for (const auto& group : chosen)
    {
        terms.push_back(groupToTerm(group));
    }

    std::sort(terms.begin(), terms.end());
    terms.erase(std::unique(terms.begin(), terms.end()), terms.end());

    std::ostringstream expression;
    for (std::size_t index = 0; index < terms.size(); ++index)
    {
        if (index > 0)
        {
            expression << " + ";
        }

        expression << terms[index];
    }

    return expression.str();
}

std::string KarnaughMap::renderAscii() const
{
    const uint32_t rowBits = variableCount_ / 2;
    const uint32_t columnBits = variableCount_ - rowBits;
    const auto rowOrder = GrayCode::generateSequence(rowBits);
    const auto columnOrder = GrayCode::generateSequence(columnBits);

    std::ostringstream output;
    output << "KMap(" << variableCount_ << " variables)\n";
    output << "      ";

    for (unsigned int columnLabel : columnOrder)
    {
        output << std::setw(3) << BinaryUtils::toBinary(columnLabel, static_cast<int>(columnBits));
    }

    output << '\n';

    for (std::size_t row = 0; row < rows_; ++row)
    {
        output << std::setw(5)
               << BinaryUtils::toBinary(rowOrder[row], static_cast<int>(rowBits));

        for (std::size_t column = 0; column < columns_; ++column)
        {
            const BooleanCell* cell = getCell(row, column);
            char marker = '0';

            if (cell != nullptr && cell->dontCare)
            {
                marker = 'X';
            }
            else if (cell != nullptr && cell->value)
            {
                marker = '1';
            }

            output << std::setw(3) << marker;
        }

        output << '\n';
    }

    return output.str();
}

const Hypercube& KarnaughMap::hypercube() const noexcept
{
    return hypercube_;
}

std::size_t KarnaughMap::coordinateIndex(std::size_t row, std::size_t column) const
{
    return row * columns_ + column;
}

std::vector<uint32_t> KarnaughMap::activeMinterms() const
{
    std::vector<uint32_t> minterms;

    for (const BooleanCell& cell : cells_)
    {
        if (cell.value || cell.dontCare)
        {
            minterms.push_back(cell.binaryValue);
        }
    }

    return sortedUnique(std::move(minterms));
}

std::vector<uint32_t> KarnaughMap::oneMinterms() const
{
    std::vector<uint32_t> minterms;

    for (const BooleanCell& cell : cells_)
    {
        if (cell.value)
        {
            minterms.push_back(cell.binaryValue);
        }
    }

    return sortedUnique(std::move(minterms));
}

std::string KarnaughMap::groupToTerm(const std::vector<uint32_t>& group) const
{
    if (group.size() == cells_.size())
    {
        return "1";
    }

    static constexpr char variables[] = {'A', 'B', 'C', 'D'};
    std::ostringstream term;

    for (uint32_t bit = 0; bit < variableCount_; ++bit)
    {
        const uint32_t mask = 1u << (variableCount_ - bit - 1u);
        const bool firstValue = (group.front() & mask) != 0;
        const bool constant = std::all_of(group.begin(), group.end(), [mask, firstValue](uint32_t minterm) {
            return ((minterm & mask) != 0) == firstValue;
        });

        if (constant)
        {
            term << variables[bit];
            if (!firstValue)
            {
                term << '\'';
            }
        }
    }

    const std::string result = term.str();
    return result.empty() ? "1" : result;
}

void KarnaughMap::buildLookupTables()
{
    mintermToCellIndex_.assign(cells_.size(), invalidIndex());
    coordinateToCellIndex_.assign(cells_.size(), invalidIndex());

    for (std::size_t index = 0; index < cells_.size(); ++index)
    {
        const BooleanCell& cell = cells_[index];
        mintermToCellIndex_[cell.binaryValue] = index;
        coordinateToCellIndex_[coordinateIndex(static_cast<std::size_t>(cell.row),
                                               static_cast<std::size_t>(cell.column))] = index;
    }
}

void KarnaughMap::validate() const
{
    if (variableCount_ < 2 || variableCount_ > 4)
    {
        throw KarnaughMapException("KarnaughMap supports only 2 to 4 variables");
    }

    if (rows_ == 0 || columns_ == 0 || rows_ * columns_ != cells_.size())
    {
        throw KarnaughMapException("Invalid KarnaughMap dimensions");
    }

    if (hypercube_.dimension() != variableCount_ || hypercube_.vertexCount() != cells_.size())
    {
        throw KarnaughMapException("KarnaughMap hypercube does not match cell layout");
    }

    for (const BooleanCell& cell : cells_)
    {
        if (cell.row < 0 || cell.column < 0 ||
            static_cast<std::size_t>(cell.row) >= rows_ ||
            static_cast<std::size_t>(cell.column) >= columns_ ||
            cell.binaryValue >= cells_.size())
        {
            throw KarnaughMapException("Invalid KarnaughMap cell");
        }
    }
}

} // namespace BooleanEngine
