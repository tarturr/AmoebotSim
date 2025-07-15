#ifndef AMOEBOTSIM_CORE_GLOBALPARTICLE_H_
#define AMOEBOTSIM_CORE_GLOBALPARTICLE_H_

#include "core/amoebotparticle.h"


class GlobalParticle : public AmoebotParticle
{
public:
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

    using AmoebotParticle::AmoebotParticle;

    int globalDirToLabel(int dir, bool head = true) const;
    template<class Particle>
    Particle& nbrAtGlobalDir(int dir, bool head = true) const;
    bool hasNbrAtGlobalDir(int dir, bool head = true) const;
    void expandToGlobalDir(int dir);
    void pushToGlobalDir(int dir);
    void pullFromGlobalDir(int dir);
};

template<class Particle>
Particle& GlobalParticle::nbrAtGlobalDir(int dir, bool head) const
{
    return AmoebotParticle::nbrAtLabel<Particle>(globalDirToLabel(dir, head));
}


#endif // AMOEBOTSIM_CORE_GLOBALPARTICLE_H_
