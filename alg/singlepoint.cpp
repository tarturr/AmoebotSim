#include "alg/singlepoint.h"


SingleParticle::SingleParticle(const Node& head,
                               const int globalTailDir,
                               const int orientation,
                               AmoebotSystem& system)
    : AmoebotParticle(head, globalTailDir, orientation, system),
    _state(State::Init),
    _connectedNbr(-1) {}

void SingleParticle::activate()
{
    switch (_state)
    {
    case State::Init:
    {
        SingleParticle::Neighbors nbrs{ neighbors() };
        auto containsInit = [](const std::pair<int, SingleParticle*> elem) -> bool
        {
            return elem.second->_state == State::Init;
        };

        if (std::find_if(nbrs.begin(), nbrs.end(), containsInit) == nbrs.end())
        {
            _state = State::Leader;
        }
        else if (canIdle())
        {
            _state = State::Idle;
        }

        break;
    }

    case State::Leader:
        for (const auto& neighbor : neighbors())
        {
            neighbor.second->connect(neighbor.first);
            neighbor.second->_state = State::Connecting;
        }

        _state = State::LeaderConnected;
        break;

    case State::Connecting:
        for (const auto& neighbor : neighbors())
        {
            State pState{ neighbor.second->_state };

            if (pState != State::LeaderConnected && pState != State::Connected)
            {
                neighbor.second->connect(neighbor.first);
            }
        }

        _state = State::Connected;

    default:
        break;
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
    text += "  neighborsCount: " + QString::number(neighbors().size()) + "\n\n";

    text += "Local Info:\n";
    text += [this](){
        switch(_state) {
        case State::Init:            return "init\n";
        case State::Idle:            return "idle\n";
        case State::Leader:          return "leader\n";
        case State::LeaderConnected: return "leaderconnected\n";
        case State::Connecting:      return "connecting\n";
        case State::Connected:       return "connected\n";
        }
        return "no state\n";
    }();

    return text;
}

int SingleParticle::headMarkDir() const
{
    return _connectedNbr;
}

int SingleParticle::headMarkColor() const
{
    switch (_state)
    {
    case State::Idle:            return 0x777777; // Gray
    case State::Connecting:      return 0xFF8010; // Orange
    case State::Connected:       return 0x33FFFF; // Cyan
    case State::Init:            return 0x1010FF; // Blue
    case State::Leader:          return 0x10FF10; // Green
    case State::LeaderConnected: return 0xFFFF10; // Pink
    }
}

int SingleParticle::tailMarkColor() const
{
    return headMarkColor();
}

SingleParticle& SingleParticle::nbrAtLabel(int label) const
{
    return AmoebotParticle::nbrAtLabel<SingleParticle>(label);
}

bool SingleParticle::canIdle() const
{
    SingleParticle::Neighbors nbrs{ neighbors() };
    std::size_t nbrCount{ nbrs.size() };

    auto shapes{ uselessShapes(nbrCount) };

    for (const auto& shape : shapes)
    {
        std::size_t count = 0;

        for (const int label : shape)
        {
            auto nbr{ nbrs.find(label) };

            if (nbr != nbrs.end() && nbr->second->_state == State::Init)
            {
                ++count;
            }
        }

        if (count == nbrCount)
            return true;
    }

    return false;
}

std::vector<std::vector<int>> SingleParticle::uselessShapes(int n) const
{
    std::vector<std::vector<int>> shapes;
    std::vector<int> values;

    for (int i = 0; i < n; ++i)
    {
        values.push_back(i);
    }

    for (int i = 0; i < 6; ++i)
    {
        for (int& val : values)
        {
            val = (val + 1) % 6;
        }

        shapes.push_back(values);
    }

    return shapes;
}

void SingleParticle::connect(int label)
{
    _connectedNbr = (label + 3) % 6;
}


SingleParticleSystem::SingleParticleSystem(const int numParticles, const double holeProb)
{
    Q_ASSERT(numParticles > 0);
    Q_ASSERT(0 <= holeProb && holeProb <= 1);

    // Insert the seed at (0,0).
    insert(new SingleParticle(Node(0, 0), -1, randDir(), *this));
    std::set<Node> occupied;
    occupied.insert(Node(0, 0));

    std::set<Node> candidates;
    for (int i = 0; i < 6; ++i) {
        candidates.insert(Node(0, 0).nodeInDir(i));
    }

    // Add inactive particles.
    int numNonStaticParticles = 0;
    while (numNonStaticParticles < numParticles && !candidates.empty()) {
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
            insert(new SingleParticle(randomCandidate, -1, randDir(), *this));
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

SingleParticle::Neighbors SingleParticle::neighbors() const
{
    SingleParticle::Neighbors neighbors;

    for (int i = 0; i < 6; ++i)
    {
        if (hasNbrAtLabel(i))
        {
            neighbors.insert({ i, &nbrAtLabel(i) });
        }
    }

    return neighbors;
}













