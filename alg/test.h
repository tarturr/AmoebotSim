#ifndef TEST_H
#define TEST_H

#include "core/amoebotparticle.h"
#include "core/amoebotsystem.h"




class TestParticle : public AmoebotParticle
{
public:

    TestParticle(const Node& head, const int globalTailDir,const int orientation, AmoebotSystem& system);


    void activate() override;

    QString inspectionText() const override;
    int headMarkColor() const override;
    int tailMarkColor() const override;

protected:
    int botL;
    int botR;
    int left;
    int right;
    int topL;
    int topR;
    TestParticle& NbrAtLabel(int label);
    int GetLabel(int dir) const;
    bool isTail(int label);

private:
    friend class TestParticleSystem;
};

class TestParticleSystem : public AmoebotSystem {
public:
    TestParticleSystem(unsigned int numParticles = 200, double holeProb = 0.2);
};

#endif // TEST_H
