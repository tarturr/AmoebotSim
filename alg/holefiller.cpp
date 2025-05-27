#include "alg/holefiller.h"

#include <vector>
#include <unordered_set>
#include <algorithm>


HoleFillerParticle::HoleFillerParticle(const Node head, const int globalTailDir, const int orientation, AmoebotSystem& system)
    : AmoebotParticle(head, globalTailDir, orientation, system) {}

void HoleFillerParticle::activate()
{
    bool sbdyBtmRight{ canExpand(1) };
    bool sbdyBtmLeft{ canExpand(2) };

    // true either if somebody is at the bottom left and nobody is
    // at the bottom right, either the opposite (^ is XOR).
    if (sbdyBtmLeft ^ sbdyBtmRight)
    {
        // The particle expands in the empty direction.
        expand(sbdyBtmRight + 1);
    }
    else if (sbdyBtmLeft && sbdyBtmRight)
    {
        // If the particle can push the bottom left particle, it means this particle is
        // extended.
        bool btmLeftExtended{ canPush(2) };
        bool btmRightExtended{ canPush(1) };

        if (btmLeftExtended)
        {
            push(2);
        }
        else if (btmRightExtended)
        {
            push(1);
        }
    }

    bool sbdyTopRight{ canExpand(5) };
    bool sbdyTopLeft{ canExpand(4) };

    // true if nobody is above the particle.
    if (!sbdyTopLeft && !sbdyTopRight)
    {
        contractTail();
    }
}

HoleFillerParticle& HoleFillerParticle::nbrAtLabel(int label) const
{
    return AmoebotParticle::nbrAtLabel<HoleFillerParticle>(label);
}

HoleFillerParticle::DownNeighbors HoleFillerParticle::getDownNeighbors() const
{
    return std::make_pair(&nbrAtLabel(1), &nbrAtLabel(2));
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
        insert(new HoleFillerParticle(random, -1, -1, *this));
        nodes.push_back(std::move(random));
    }
}












