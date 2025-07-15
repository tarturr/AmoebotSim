#include "holedleaderelection.h"

#include "combinations.h"


HLEParticle::HLEParticle(const Node head, const int orientation, AmoebotSystem& system)
    : GlobalParticle(head, -1, orientation, system), _state(State::Active) {}

HLEParticle& HLEParticle::nbrAtGlobalDir(int dir, bool head) const
{
    return GlobalParticle::nbrAtGlobalDir<HLEParticle>(dir, head);
}

void HLEParticle::pullFromGlobalDir(int dir)
{
    GlobalParticle::pullFromGlobalDir(dir);
}

void HLEParticle::pushToGlobalDir(int dir)
{
    GlobalParticle::pushToGlobalDir(dir);
    _state = State::Active;
}

void HLEParticle::expandToGlobalDir(int dir)
{
    GlobalParticle::expandToGlobalDir(dir);
    _state = State::Active;
}

void HLEParticle::activate()
{
    if (isContracted())
    {
        for (int i = 0; i < 6; ++i)
        {
            if (hasNbrAtGlobalDir(i) && nbrAtGlobalDir(i).isExpanded())
            {
                _state = State::Active;
                onFilling();
                return;
            }
        }
    }

    onFilling();

    if (isContracted())
    {
        onErosion();
    }
}

void HLEParticle::onFilling()
{
    bool exp = isExpanded();

    // Si la particule est contractée.
    if (!exp)
    {
        // S'il y a un voisin en bas à droite et pas en bas à gauche.
        if (!hasNbrAtGlobalDir(Direction::BottomLeft) && hasNbrAtGlobalDir(Direction::BottomRight))
        {
            expandToGlobalDir(Direction::BottomLeft);
            exp = true;
        }
        // S'il y a un voisin en bas à gauche et pas en bas à droite.
        else if(!hasNbrAtGlobalDir(Direction::BottomRight) && hasNbrAtGlobalDir(Direction::BottomLeft))
        {
            expandToGlobalDir(Direction::BottomRight);
            exp = true;
        }
    }

    // Si la particule est étendue.
    if (exp)
    {
        bool canContract = true;

        switch (globalTailDir)
        {
        // Tête étendue en bas à droite.
        case Direction::TopLeft:
            for (int i = 1; i <= 3; i++)
            {
                if (hasNbrAtGlobalDir(i, false))
                {
                    HLEParticle& other = nbrAtGlobalDir(i, false);

                    // Si le voisin courant peut être tiré.
                    if (other.isContracted())
                    {
                        pullFromGlobalDir(i);
                        exp = false;
                        break;
                    }
                    // Si la particule voisine présente est celle de gauche.
                    else if (i == Direction::Left)
                    {
                        // Si je suis étendu et que personne n'est en bas à gauche.
                        if (!hasNbrAtGlobalDir(Direction::BottomLeft, false) && canContract)
                        {
                            contractTail();
                            expandToGlobalDir(Direction::Left);
                            canContract = false;
                        }

                        break;
                    }

                    canContract = false;
                }
            }

            break;

        // Tête étendue à droite.
        case Direction::Left:
            for (int i = 1; i <= 3; i++)
            {
                if (hasNbrAtGlobalDir(i, false))
                {
                    HLEParticle& other = nbrAtGlobalDir(i, false);

                    // Si le voisin courant peut être tiré.
                    if (other.isContracted())
                    {
                        pullFromGlobalDir(i);
                        exp = false;
                        break;
                    }
                    // Si la particule voisine présente est celle de gauche.
                    else if (i == Direction::Left)
                    {
                        // Si la voisine est étendue à l'opposé de ma tête.
                        if (other.globalTailDir == Direction::Right && canContract)
                        {
                            contractTail();
                            expandToGlobalDir(Direction::Left);
                        }
                    }

                    canContract = false;
                }
            }

            if (exp
                && canContract
                && hasNbrAtGlobalDir(Direction::BottomLeft, false)
                && !hasNbrAtGlobalDir(Direction::BottomRight, false))
            {
                contractTail();
                expandToGlobalDir(Direction::BottomLeft);
                canContract = false;
            }

            break;

        // Tête étendue en bas à gauche.
        case Direction::TopRight:
            for (int i = 2; i >= 0; i--)
            {
                if (hasNbrAtGlobalDir(i, false))
                {
                    HLEParticle& other = nbrAtGlobalDir(i, false);

                    // Si le voisin courant peut être tiré.
                    if (other.isContracted())
                    {
                        pullFromGlobalDir(i);
                        exp = false;
                        break;
                    }
                    // Si la particule voisine présente est celle de droite.
                    else if (i == Direction::Right)
                    {
                        // Si je suis étendu et que personne n'est en bas à droite.
                        if(!hasNbrAtGlobalDir(Direction::BottomRight, false) && canContract)
                        {
                            contractTail();
                            expandToGlobalDir(Direction::Right);
                            canContract = false;
                        }

                        break;
                    }

                    canContract = false;
                }
            }

            break;

        // Tête étendue à gauche.
        case Direction::Right:
            for (int i = 2; i >= 0; i--)
            {
                if (hasNbrAtGlobalDir(i, false))
                {
                    HLEParticle& other = nbrAtGlobalDir(i, false);

                    // Si le voisin courant peut être tiré.
                    if (other.isContracted())
                    {
                        pullFromGlobalDir(i);
                        exp = false;
                        break;
                    }
                    // Si la particule voisine présente est celle de droite.
                    else if (i == Direction::Right)
                    {
                        // Si la voisine est étendue à l'opposé de ma tête.
                        if (other.globalTailDir == Direction::Left && canContract)
                        {
                            contractTail();
                            expandToGlobalDir(Direction::Right);
                        }
                    }
                    canContract = false;
                }
            }

            if (exp
                && canContract
                && hasNbrAtGlobalDir(Direction::BottomRight, false)
                && !hasNbrAtGlobalDir(Direction::BottomLeft, false))
            {
                contractTail();
                expandToGlobalDir(Direction::BottomRight);
                canContract = false;
            }

            break;

        default:
            break;
        }

        if (exp && canContract){
            contractTail();
        }
    }
}

void HLEParticle::onErosion()
{
    if (_state == State::Active || _state == State::Idle)
    {
        if (getActiveNbrs().empty())
        {
            _state = State::Leader;
        }
        else if (erode())
        {
            _state = State::Idle;
        }
        else
        {
            _state = State::Active;
        }
    }
}

bool HLEParticle::erode() const
{
    std::unordered_set<int> nbrs = getActiveNbrs();
    std::size_t amount{ nbrs.size() };

    if (amount == 6)
    {
        return false;
    }
    else if (amount == 1)
    {
        return true;
    }

    Combinations combinations{ amount };
    return combinations.contains(nbrs);
}

std::unordered_set<int> HLEParticle::getActiveNbrs() const
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

QString HLEParticle::inspectionText() const
{
    QString text;
    text += "Global Info:\n";
    text += "  head: (" + QString::number(head.x) + ", "
            + QString::number(head.y) + ")\n";
    text += "  orientation: " + QString::number(orientation) + "\n";
    text += "  globalTailDir: " + QString::number(globalTailDir) + "\n";

    text += "Local Info:\n";
    text += "Active Neighbors:";

    for (int dir : getActiveNbrs())
    {
        text += " " + QString::number(dir);
    }

    return text;
}

int HLEParticle::headMarkColor() const
{
    switch (_state)
    {
    case State::Active:
        return 0x0000FF;

    case State::Idle:
        return 0x7777FF;

    case State::Leader:
        return 0x00FF00;
    }
}

int HLEParticle::tailMarkColor() const
{
    return headMarkColor();
}



HLESystem::HLESystem(const unsigned int numParticles, const double holeProb)
{
    Q_ASSERT(numParticles > 0);
    Q_ASSERT(0 <= holeProb && holeProb <= 1);

    // Insert the seed at (0,0).
    insert(new HLEParticle(Node(0, 0), randDir(), *this));
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
            insert(new HLEParticle(randomCandidate, randDir(), *this));
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
