#ifndef AMOEBOTSIM_ALG_HOLEFILLER_H_
#define AMOEBOTSIM_ALG_HOLEFILLER_H_

#include "core/amoebotparticle.h"
#include "core/amoebotsystem.h"


class HoleFillerParticle : public AmoebotParticle
{
public:
    HoleFillerParticle(const Node head, const int globalTailDir, const int orientation, AmoebotSystem& system);
    using DownNeighbors = std::pair<HoleFillerParticle*, HoleFillerParticle*>;

    void activate() override;
    HoleFillerParticle& nbrAtLabel(int label) const;

private:
    DownNeighbors getDownNeighbors() const;
    friend class HoleFillerSystem;
};


class HoleFillerSystem : public AmoebotSystem
{
public:
    HoleFillerSystem(unsigned int particles = 50);

private:
    Node chooseRandomNode(const std::vector<Node>& nodes) const;
};


#endif // AMOEBOTSIM_ALG_HOLEFILLER_H_
