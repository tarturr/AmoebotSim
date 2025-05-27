#ifndef SINGLEPOINT_H
#define SINGLEPOINT_H

#include "core/amoebotparticle.h"
#include "core/amoebotsystem.h"

#include <vector>
#include <unordered_map>


class SingleParticle : public AmoebotParticle
{
public:
    enum class State
    {
        Init,
        Idle,
        Leader,
        LeaderConnected,
        Connecting,
        Connected
    };

    SingleParticle(const Node& head, const int globalTailDir, const int orientation, AmoebotSystem& system);

    void activate() override;
    QString inspectionText() const override;

    int headMarkDir() const override;
    int headMarkColor() const override;
    int tailMarkColor() const override;

    SingleParticle& nbrAtLabel(int label) const;
protected:
    using Neighbors = std::unordered_map<int, SingleParticle*>;

    bool canIdle() const;
    std::vector<std::vector<int>> uselessShapes(int n) const;
    void connect(int label);
    Neighbors neighbors() const;

    State _state;
    int _connectedNbr;
private:
    friend class SingleParticleSystem;
};


class SingleParticleSystem : public AmoebotSystem
{
public:
    SingleParticleSystem(const int numParticles = 100, const double holeProb = 0.2);
};


#endif // SINGLEPOINT_H
