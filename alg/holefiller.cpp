#include "alg/holefiller.h"


constexpr std::initializer_list<HoleFillerParticle::Direction> HoleFillerParticle::_ORDERED_DIRS;

using Direction = HoleFillerParticle::Direction;

HoleFillerParticle::HoleFillerParticle(const Node head, const int globalTailDir, const int orientation, AmoebotSystem& system)
    : GlobalParticle(head, globalTailDir, orientation, system), _state(State::Stabilized)
{
}

void HoleFillerParticle::activate()
{
    switch (_state)
    {
    case State::Stabilized:
        // onStabilized();
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
// void HoleFillerParticle::onStabilized()
// {
//     // Find the first rescue-mode expanded particle and push it, if any.
//     for (const Direction dir : HoleFillerParticle::_ORDERED_DIRS)
//     {
//         if (hasNbrAtGlobalDir(dir))
//         {
//             HoleFillerParticle& nbr{ nbrAtGlobalDir(dir) };

//             if (nbr._state == State::Rescue && nbr.isExpanded())
//             {
//                 pushToGlobalDir(dir);
//                 _state = State::Rescue;
//                 return;
//             }
//         }
//     }

//     bool hasBtmLeft{ hasNbrAtGlobalDir(Direction::BottomLeft) };
//     bool hasBtmRight{ hasNbrAtGlobalDir(Direction::BottomRight) };

//     if (hasBtmLeft || hasBtmRight)
//     {
//         Direction expandedDir{ findExpandedNbr({ Direction::BottomLeft, Direction::BottomRight }) };

//         if (expandedDir == Direction::None)
//         {
//             if (hasBtmLeft ^ hasBtmRight)
//             {
//                 expandToGlobalDir(hasBtmRight ? Direction::BottomLeft : Direction::BottomRight);
//                 _state = State::Drop;
//             }
//         }
//         else
//         {
//             HoleFillerParticle& nbr{ nbrAtGlobalDir(expandedDir) };
//             int oldHeadDir{ (nbr.globalTailDir + 3) % 6 };
//             nbr.contractHead();

//             if (hasNbrAtGlobalDir(expandedDir))
//             {
//                 nbr.expandToGlobalDir(oldHeadDir);
//                 pushToGlobalDir(expandedDir);
//                 _state = State::Drop;
//             }
//             else
//             {
//                 nbr.expandToGlobalDir(oldHeadDir);
//             }
//         }

//         return;
//     }
// }

// Invariants:
//     - Has either one neighbor down left, or down right.
void HoleFillerParticle::onDrop()
{
    if (isContracted())
    {
        _state = State::Stabilized;
        return;
    }

    if (globalTailDir == Direction::TopLeft)
    {
        std::initializer_list<Direction> dirs
        {
            Direction::TopRight,
            Direction::TopLeft,
            Direction::Left
        };

        for (const Direction dir : dirs)
        {
            if (hasNbrAtGlobalDir(dir) && nbrAtGlobalDir(dir).isContracted())
            {

            }
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
                if (hasNbrAtGlobalDir(dir, false))
                {
                    HoleFillerParticle& nbr{ nbrAtGlobalDir(dir) };

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
                HoleFillerParticle& nbr{ nbrAtGlobalDir(dir) };

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

HoleFillerParticle& HoleFillerParticle::nbrAtGlobalDir(int dir, bool head) const
{
    return GlobalParticle::nbrAtGlobalDir<HoleFillerParticle>(globalDirToLabel(dir, head));
}


HoleFillerSystem::HoleFillerSystem(const unsigned int particles, const double holeProb) {
    Q_ASSERT(particles > 0);
    Q_ASSERT(0 <= holeProb && holeProb <= 1);

    // Insert the seed at (0,0).
    insert(new HoleFillerParticle(Node(0, 0), -1, randDir(), *this));
    std::set<Node> occupied;
    occupied.insert(Node(0, 0));

    std::set<Node> candidates;
    for (int i = 0; i < 6; ++i) {
        candidates.insert(Node(0, 0).nodeInDir(i));
    }

    // Add inactive particles.
    unsigned int numNonStaticParticles = 0;
    while (numNonStaticParticles < particles && !candidates.empty()) {
        // Pick random candidate.
        int randIndex = randInt(0, candidates.size());
        Node randomCandidate;
        for (auto it = candidates.begin(); it != candidates.end(); ++it) {
            if (randIndex == 0) {
                randomCandidate = *it;
                candidates.erase(it);
                break;
            } else {
                randIndex--;
            }
        }

        occupied.insert(randomCandidate);

        // Add this candidate as a particle if not a hole.
        if (randBool(1.0 - holeProb)) {
            insert(new HoleFillerParticle(randomCandidate, -1, randDir(), *this));
            ++numNonStaticParticles;

            // Add new candidates.
            for (int i = 0; i < 6; ++i) {
                auto neighbor = randomCandidate.nodeInDir(i);
                if (occupied.find(neighbor) == occupied.end()) {
                    candidates.insert(neighbor);
                }
            }
        }
    }
}
