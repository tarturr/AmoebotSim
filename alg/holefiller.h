#ifndef AMOEBOTSIM_ALG_HOLEFILLER_H_
#define AMOEBOTSIM_ALG_HOLEFILLER_H_

#include "core/globalparticle.h"
#include "core/amoebotsystem.h"


class HoleFillerParticle : public GlobalParticle
{
public:
    using Direction = GlobalParticle::Direction;

    enum class State
    {
        Stabilized,
        Drop,
        Rescue
    };

    HoleFillerParticle(const Node head, const int globalTailDir, const int orientation, AmoebotSystem& system);
    using DownNeighbors = std::pair<HoleFillerParticle*, HoleFillerParticle*>;

    void activate() override;
    HoleFillerParticle& nbrAtLabel(int label) const;
    QString inspectionText() const override;
    int headMarkColor() const override;
    int tailMarkColor() const override;

protected:
    HoleFillerParticle& nbrAtGlobalDir(int dir, bool head = true) const;
    void onDrop();
    void onRescue();
    Direction extendTo();
    Direction findExpandedNbr(std::initializer_list<Direction> dirs) const;

    State _state;

    static constexpr std::initializer_list<Direction> _ORDERED_DIRS
    {
        Direction::BottomRight,
        Direction::Right,
        Direction::TopRight,
        Direction::BottomLeft,
        Direction::Left,
        Direction::TopLeft
    };

private:
    friend class HoleFillerSystem;
};


class HoleFillerSystem : public AmoebotSystem
{
public:
    HoleFillerSystem(const unsigned int particles = 50, const double holeProb = 0.2);

private:
    Node chooseRandomNode(const std::vector<Node>& nodes) const;
};


#endif // AMOEBOTSIM_ALG_HOLEFILLER_H_
