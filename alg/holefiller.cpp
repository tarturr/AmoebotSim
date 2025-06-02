#include "alg/holefiller.h"

#include <vector>
#include <unordered_set>
#include <algorithm>


constexpr std::initializer_list<HoleFillerParticle::Direction> HoleFillerParticle::_ORDERED_DIRS;

using Direction = HoleFillerParticle::Direction;

HoleFillerParticle::HoleFillerParticle(const Node head, const int globalTailDir, const int orientation, AmoebotSystem& system)
    : AmoebotParticle(head, globalTailDir, orientation, system), _state(State::Stabilized)
{
}

void HoleFillerParticle::activate()
{
    switch (_state)
    {
    case State::Stabilized:
        onStabilized();
        break;

    case State::Drop:
        onDrop();
        break;

    case State::Rescue:
        onRescue();
        break;
    }
}

// Invariants:
//     - Down neighbors both present or absent.
//     - If absent, has at least a neighbor, left or right.
void HoleFillerParticle::onStabilized()
{
    // Find the first rescue-mode expanded particle and push it, if any.
    for (const Direction dir : HoleFillerParticle::_ORDERED_DIRS)
    {
        if (!hasNbrAtGlobalDir(dir)) continue;

        if (hasNbrAtGlobalDir(dir))
        {
            auto nbr{ nbrAtGlobalDir(dir) };

            if (nbr._state == State::Rescue && nbr.isExpanded())
            {
                pushToGlobalDir(dir);
                _state = State::Rescue;
                return;
            }
        }
    }

    bool hasBtmLeft{ hasNbrAtGlobalDir(Direction::BottomLeft) };
    bool hasBtmRight{ hasNbrAtGlobalDir(Direction::BottomRight) };

    if (hasBtmLeft || hasBtmRight)
    {
        Direction expandedDir{ findExpandedNbr({ Direction::BottomLeft, Direction::BottomRight }) };

        if (expandedDir == Direction::None)
        {
            if (hasBtmLeft ^ hasBtmRight)
            {
                expandToGlobalDir(hasBtmRight ? Direction::BottomLeft : Direction::BottomRight);
                _state = State::Drop;
            }
        }
        else
        {
            auto nbr{ nbrAtGlobalDir(expandedDir) };
            int oldHeadDir{ (nbr.globalTailDir + 3) % 6 };
            nbr.contractHead();

            if (hasNbrAtGlobalDir(expandedDir))
            {
                nbr.expandToGlobalDir(oldHeadDir);
                pushToGlobalDir(expandedDir);
                _state = State::Drop;
            }
            else
            {
                nbr.expandToGlobalDir(oldHeadDir);
            }
        }

        return;
    }
}

// Invariants:
//     - Has either one neighbor down left, or down right.
void HoleFillerParticle::onDrop()
{
    if (isContracted())
    {
        _state = State::Stabilized;
        return;
    }

    for (const int label : tailLabels())
    {
        if (hasNbrAtLabel(label) && !isHeadLabel(label))
        {
            return;
        }
    }

    contractTail();
    _state = State::Stabilized;
}

// Invariants:
//     - Has only one neighbor or neighbors which
void HoleFillerParticle::onRescue()
{
    if (isExpanded())
    {
        for (const int label : tailLabels())
        {
            int dir{ labelToGlobalDir(label) };

            switch (dir)
            {
            case Direction::BottomRight:
            case Direction::Right:
            case Direction::TopRight:
                if (hasNbrAtGlobalDir(dir))
                {
                    auto nbr{ nbrAtGlobalDir(dir) };

                    if (nbr.isExpanded() && nbr._state == State::Rescue)
                    {
                        // "Rotation": from pointing x with tail at y, to pointing y with tail at x.
                        int oldDir{ globalTailDir };
                        contractTail();
                        expandToGlobalDir(oldDir);

                        // Pull the preceding rescue particle.
                        pullFromGlobalDir((oldDir + 3) % 6);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for (const Direction dir : HoleFillerParticle::_ORDERED_DIRS)
        {
            if (hasNbrAtGlobalDir(dir))
            {
                auto nbr{ nbrAtGlobalDir(dir) };

                // If any rescue-mode expanded neighbor is NOT pointing at me,
                // which means it's travelling back because a more important particle
                // asks to follow it.
                if (nbr.isExpanded() && nbr._state == State::Rescue && !pointsAtMe(nbr, globalDirToLabel(dir)))
                {
                    pushToGlobalDir(dir);
                    return;
                }
            }
        }

        _state = State::Stabilized;
    }
}


Direction HoleFillerParticle::findExpandedNbr(std::initializer_list<Direction> dirs) const
{
    Q_ASSERT(dirs.size() != 0);

    for (const Direction dir : dirs)
    {
        if (hasNbrAtGlobalDir(dir))
        {
            if (nbrAtGlobalDir(dir).isExpanded())
            {
                return dir;
            }
        }
    }

    return Direction::None;
}

HoleFillerParticle& HoleFillerParticle::nbrAtLabel(int label) const
{
    return AmoebotParticle::nbrAtLabel<HoleFillerParticle>(label);
}

QString HoleFillerParticle::inspectionText() const
{
    QString text;
    text += "Global Info:\n";
    text += "  head: (" + QString::number(head.x) + ", "
            + QString::number(head.y) + ")\n";
    text += "  orientation: " + QString::number(orientation) + "\n";
    text += "  globalTailDir: " + QString::number(globalTailDir) + "\n";

    for (int i = 0; i < 6; ++i)
    {
        text += "  neighborAt" + QString::number(i) + ": " + QString(hasNbrAtGlobalDir(i) ? "true" : "false") + "\n";
    }

    return text;
}

int HoleFillerParticle::headMarkColor() const
{
    switch (_state)
    {
    case State::Stabilized: return 0x00FF00; // Green
    case State::Drop:       return 0xFF0000; // Red
    case State::Rescue:     return 0xFF7700; // Orange
    }
}

int HoleFillerParticle::tailMarkColor() const
{
    return headMarkColor();
}

int HoleFillerParticle::globalDirToLabel(int dir) const
{
    return dirToHeadLabel(globalToLocalDir(dir));
}

HoleFillerParticle& HoleFillerParticle::nbrAtGlobalDir(int dir) const
{
    return nbrAtLabel(globalDirToLabel(dir));
}

bool HoleFillerParticle::hasNbrAtGlobalDir(int dir) const
{
    return hasNbrAtLabel(globalDirToLabel(dir));
}

void HoleFillerParticle::expandToGlobalDir(int dir)
{
    expand(globalDirToLabel(dir));
}

void HoleFillerParticle::pushToGlobalDir(int dir)
{
    push(globalDirToLabel(dir));
}

void HoleFillerParticle::pullFromGlobalDir(int dir)
{
    pull(globalDirToLabel(dir));
}


Node HoleFillerSystem::chooseRandomNode(const std::vector<Node>& nodes) const
{
    // Loops until a node placed next to an already-placed random node from the graph is particle-free.
    while (true)
    {
        // Select a random "root" node from the already placed nodes.
        const Node& root{ nodes[randInt(0, nodes.size())] };
        std::unordered_set<int> occupied;
        occupied.reserve(6);

        // Select a random node among the root neighbors.
        int dir{ randDir() };
        Node random{ root.nodeInDir(dir) };

        // While the node in the provided random direction already exists AND the number of already occupied
        // nodes is less than 6 (otherwise, this would mean the node cannot provide any space next to it).
        while (std::find(nodes.begin(), nodes.end(), random) != nodes.end()
               && occupied.size() < 6)
        {
            occupied.insert(dir);
            dir = randDir();
            random = root.nodeInDir(dir);
        }

        if (occupied.size() != 6)
        {
            return random;
        }
    }
}

HoleFillerSystem::HoleFillerSystem(unsigned int particles)
{
    std::vector<Node> nodes;

    // Adding the central node.
    nodes.emplace_back(0, 0);

    // Filling the nodes with randomly placed particles such that the formed graph is always connected.
    for (unsigned int i = 0; i < particles - 1; ++i)
    {
        Node random{ chooseRandomNode(nodes) };
        insert(new HoleFillerParticle(random, -1, 0, *this));
        nodes.push_back(std::move(random));
    }
}
