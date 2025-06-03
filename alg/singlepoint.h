#ifndef SINGLEPOINT_H
#define SINGLEPOINT_H

#include "core/globalparticle.h"
#include "core/amoebotsystem.h"


class SingleParticle : public GlobalParticle
{
public:
    using Direction = GlobalParticle::Direction;

    enum class State
    {
        Active,
        Idle,
        Leader
    };

    SingleParticle(const Node& head, const int globalTailDir, const int orientation, AmoebotSystem& system);

    void activate() override;
    QString inspectionText() const override;

    int headMarkDir() const override;
    int headMarkColor() const override;
    int tailMarkColor() const override;

    SingleParticle& nbrAtGlobalDir(int dir, bool head = true) const;
protected:
    int erode() const;
    std::vector<int> getActiveNbrs() const;

    State _state;
    int _pointAt;
private:
    friend class SingleParticleSystem;
};


class SingleParticleSystem : public AmoebotSystem
{
public:
    SingleParticleSystem(const int numParticles = 250);
};


#endif // SINGLEPOINT_H
