#ifndef HOLEFILLER_H
#define HOLEFILLER_H

#include "core/globalparticle.h"
#include "core/amoebotsystem.h"


class HoleFillerParticle : public GlobalParticle
{
public:
  using Direction = GlobalParticle::Direction;

  HoleFillerParticle(const Node& head, const int globalTailDir,const int orientation, AmoebotSystem& system);

  void activate() override;

  QString inspectionText() const override;
  int headMarkColor() const override;
  int tailMarkColor() const override;

private:
  HoleFillerParticle& nbrAtGlobalDir(int dir, bool head = true) const;

  friend class HoleFillerSystem;
};

class HoleFillerSystem : public AmoebotSystem {
public:
  HoleFillerSystem(unsigned int numParticles = 200, double holeProb = 0.2);
};

#endif // HOLEFILLER_H
