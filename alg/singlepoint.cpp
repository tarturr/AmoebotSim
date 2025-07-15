#include "alg/singlepoint.h"

#include "combinations.h"


SingleParticle::SingleParticle(const Node& head,
                               const int globalTailDir,
                               const int orientation,
                               AmoebotSystem& system)
    : GlobalParticle(head, globalTailDir, orientation, system),
    _state(State::Active),
    _pointAt(-1) {}

void SingleParticle::activate()
{
    if (_state == State::Active)
    {
        if (getActiveNbrs().empty())
        {
            _state = State::Leader;
        }
        else
        {
            int newOrientation{ erode() };

            // If the particle CAN erode.
            if (newOrientation != -1)
            {
                _pointAt = newOrientation;
                _state = State::Idle;
            }
        }
    }
}

QString SingleParticle::inspectionText() const
{
    QString text;
    text += "Global Info:\n";
    text += "  head: (" + QString::number(head.x) + ", "
            + QString::number(head.y) + ")\n";
    text += "  orientation: " + QString::number(orientation) + "\n";
    text += "  globalTailDir: " + QString::number(globalTailDir) + "\n";

    text += "Local Info:\n";

    switch(_state)
    {
    case State::Active:   text += "Init\n";
    case State::Idle:   text += "Idle\n";
    case State::Leader: text += "Leader\n";
    }

    return text;
}

int SingleParticle::headMarkDir() const
{
    return _pointAt;
}

int SingleParticle::headMarkColor() const
{
    switch (_state)
    {
    case State::Active: return 0x1010FF; // Dark Blue
    case State::Idle:   return 0x7777FF; // Light Blue
    case State::Leader: return 0xFF0000; // Green
    }
}

int SingleParticle::tailMarkColor() const
{
    return headMarkColor();
}

SingleParticle& SingleParticle::nbrAtGlobalDir(int dir, bool head) const
{
    return GlobalParticle::nbrAtGlobalDir<SingleParticle>(dir, head);
}

std::unordered_set<int> SingleParticle::getActiveNbrs() const
{
    std::unordered_set<int> activeNbrs;

    for (int i = 0; i < 6; ++i)
    {
        if (hasNbrAtGlobalDir(i) && nbrAtGlobalDir(i)._state == State::Active)
        {
            activeNbrs.insert(i);
        }
    }

    return activeNbrs;
}

int SingleParticle::erode() const
{
    std::unordered_set<int> activeNbrs = getActiveNbrs();
    std::size_t max{ activeNbrs.size() };

    int first{ *activeNbrs.begin() };

    if (max == 6)
    {
        return -1;
    }
    else if (max == 1)
    {
        return first;
    }

    Combinations combinations{ max };
    return combinations.contains(activeNbrs) ? first : -1;

    /* Rotations rotations = getRotations(max);

    if (std::find(rotations.begin(), rotations.end(), activeNbrs) != rotations.end())
    {
        return activeNbrs[0];
    }

    return -1; */
}


SingleParticleSystem::SingleParticleSystem(const int numParticles)
{
    int x, y;
    for (int i = 1; i <= numParticles; ++i) {
        int layer = 1;
        int position = i - 1;
        while (position - (6 * layer) >= 0) {
            position -= 6 * layer;
            ++layer;
        }

        switch(position / layer) {
        case 0: {
            x = layer;
            y = (position % layer) - layer;
            if (position % layer == 0) {x -= 1; y += 1;}  // Corner case.
            break;
        }
        case 1: {
            x = layer - (position % layer);
            y = position % layer;
            break;
        }
        case 2: {
            x = -1 * (position % layer);
            y = layer;
            break;
        }
        case 3: {
            x = -1 * layer;
            y = layer - (position % layer);
            break;
        }
        case 4: {
            x = (position % layer) - layer;
            y = -1 * (position % layer);
            break;
        }
        case 5: {
            x = (position % layer);
            y = -1 * layer;
            break;
        }
        }

        insert(new SingleParticle(Node(x, y), -1, 0, *this));
    }
}
