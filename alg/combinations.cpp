#include "combinations.h"

#include <algorithm>

Combination::Combination(std::unordered_set<int> values)
    : _ids(std::move(values)) {}

bool Combination::contains(int id) const
{
    return std::find(_ids.begin(), _ids.end(), id) != _ids.end();
}

bool Combination::operator==(const Combination& combination) const
{
    if (_ids.size() != combination._ids.size())
    {
        return false;
    }

    auto otherContains = [&combination](int id) -> bool
    {
        return combination.contains(id);
    };

    return std::find_if_not(_ids.begin(), _ids.end(), otherContains) == _ids.end();
}

bool Combination::operator!=(const Combination& combination) const
{
    return !(*this == combination);
}


Combinations::Combinations(std::size_t size)
    : _combinations()
{
    for (int i = 0; i < 6; ++i)
    {
        std::unordered_set<int> combination;

        for (std::size_t j = 0; j < size; ++j)
        {
            combination.insert((i + j) % 6);
        }

        _combinations.emplace_back(std::move(combination));
    }
}

bool Combinations::contains(const Combination& other) const
{
    return std::find(_combinations.begin(), _combinations.end(), other) != _combinations.end();
}
