#ifndef COMBINATIONS_H
#define COMBINATIONS_H


#include <unordered_set>
#include <vector>

struct Combination
{
public:
    Combination(std::unordered_set<int> values);
    Combination(const Combination&) = default;
    Combination(Combination&&) = default;
    Combination& operator=(const Combination&) = default;
    Combination& operator=(Combination&&) = default;

    bool contains(int id) const;

    bool operator==(const Combination& combination) const;
    bool operator!=(const Combination& combination) const;

private:
    std::unordered_set<int> _ids;
};


struct Combinations
{
public:
    Combinations(std::size_t size);

    bool contains(const Combination& combination) const;

private:
    std::vector<Combination> _combinations;
};


#endif // COMBINATIONS_H
