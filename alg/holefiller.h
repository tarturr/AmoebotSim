#ifndef AMOEBOTSIM_ALG_HOLEFILLER_H_
#define AMOEBOTSIM_ALG_HOLEFILLER_H_

#include "core/amoebotparticle.h"
#include "core/amoebotsystem.h"


class HoleFillerParticle : public AmoebotParticle
{
public:
    enum class State
    {
        Stabilized,
        Drop,
        Rescue
    };

    enum Direction
    {
        Right,
        TopRight,
        TopLeft,
        Left,
        BottomLeft,
        BottomRight,
        None
    };

    HoleFillerParticle(const Node head, const int globalTailDir, const int orientation, AmoebotSystem& system);
    using DownNeighbors = std::pair<HoleFillerParticle*, HoleFillerParticle*>;

    void activate() override;
    HoleFillerParticle& nbrAtLabel(int label) const;
    QString inspectionText() const override;
    int headMarkColor() const override;
    int tailMarkColor() const override;

protected:
    int globalDirToLabel(int dir) const;
    HoleFillerParticle& nbrAtGlobalDir(int dir) const;
    bool hasNbrAtGlobalDir(int dir) const;
    void expandToGlobalDir(int dir);
    void pushToGlobalDir(int dir);
    void pullFromGlobalDir(int dir);

    void onStabilized();
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
    HoleFillerSystem(unsigned int particles = 50);

private:
    Node chooseRandomNode(const std::vector<Node>& nodes) const;
};


#endif // AMOEBOTSIM_ALG_HOLEFILLER_H_
