#ifndef HOLEDLEADERELECTION_H
#define HOLEDLEADERELECTION_H


#include "core/globalparticle.h"

#include <unordered_set>

class HLEParticle : public GlobalParticle
{
public:
    using Direction = GlobalParticle::Direction;

    HLEParticle(const Node head, const int orientation, AmoebotSystem& system);

    enum class State
    {
        Active,
        Idle,
        Leader
    };

    void activate() override;
    QString inspectionText() const override;

    int headMarkColor() const override;
    int tailMarkColor() const override;

    void pullFromGlobalDir(int dir);
    void pushToGlobalDir(int dir);
    void expandToGlobalDir(int dir);

private:
    HLEParticle& nbrAtGlobalDir(int dir, bool head = true) const;

    void onFilling();
    void onErosion();
    bool erode() const;
    std::unordered_set<int> getActiveNbrs() const;

    State _state;
};


class HLESystem : public AmoebotSystem
{
public:
    HLESystem(const unsigned int numParticles = 200, const double holeProb = 0.2);
};


#endif // HOLEDLEADERELECTION_H
