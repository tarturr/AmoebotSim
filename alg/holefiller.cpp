#include "alg/holefiller.h"



HoleFillerParticle::HoleFillerParticle(const Node& head,
                     const int globalTailDir,
                     const int orientation,
                     AmoebotSystem& system)
  : GlobalParticle(head, globalTailDir, orientation, system) {}

HoleFillerParticle& HoleFillerParticle::nbrAtGlobalDir(int dir, bool head) const {
  return GlobalParticle::nbrAtGlobalDir<HoleFillerParticle>(dir, head);
}

void HoleFillerParticle::activate() {
  bool exp = isExpanded();

  // If the particle is contracted.
  if (!exp) {
    // If there is a neighbor on the bottom right and not on the bottom left.
    if (!hasNbrAtGlobalDir(Direction::BottomLeft) && hasNbrAtGlobalDir(Direction::BottomRight)) {
      expandToGlobalDir(Direction::BottomLeft);
      exp = true;
    // If there is a neighbor on the bottom left and not on the bottom right.
    } else if (!hasNbrAtGlobalDir(Direction::BottomRight) && hasNbrAtGlobalDir(Direction::BottomLeft)) {
      expandToGlobalDir(Direction::BottomRight);
      exp = true;
    }
  }

  // If the particle is expanded.
  if (exp) {
    bool canContract = true;

    switch (globalTailDir) {
    // Head expanded on the bottom right.
    case Direction::TopLeft:
      for (int i = 1; i <= 3; i++) {
        if (hasNbrAtGlobalDir(i, false)) {
          HoleFillerParticle& other = nbrAtGlobalDir(i, false);

          // If the neighbor cannot be pulled.
          if (other.isContracted()) {
            pullFromGlobalDir(i);
            exp = false;
            break;
          // If the present neighbor is on the left.
          } else if (i == Direction::Left) {
            // If I am expanded and nobody is on the bottom left.
            if (!hasNbrAtGlobalDir(Direction::BottomLeft, false) && canContract) {
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

    // Head expanded on the right.
    case Direction::Left:
      for (int i = 1; i <= 3; i++) {
        if (hasNbrAtGlobalDir(i, false)) {
          HoleFillerParticle& other = nbrAtGlobalDir(i, false);

          // If the neighbor cannot be pulled.
          if (other.isContracted()) {
            pullFromGlobalDir(i);
            exp = false;
            break;
          // If the present neighbor is on the left.
          } else if (i == Direction::Left) {
            // If the neighbor is expanded in a direction opposed to my head.
            if (other.globalTailDir == Direction::Right && canContract) {
              contractTail();
              expandToGlobalDir(Direction::Left);
            }
          }

          canContract = false;
        }
      }

      if (exp &&
          canContract &&
          hasNbrAtGlobalDir(Direction::BottomLeft, false) &&
          !hasNbrAtGlobalDir(Direction::BottomRight, false)) {
        contractTail();
        expandToGlobalDir(Direction::BottomLeft);
        canContract = false;
      }

      break;

    // Head expanded on the bottom left.
    case Direction::TopRight:
      for (int i = 2; i >= 0; i--) {
        if (hasNbrAtGlobalDir(i, false)) {
          HoleFillerParticle& other = nbrAtGlobalDir(i, false);

          // If the neighbor can be pulled.
          if (other.isContracted())
          {
            pullFromGlobalDir(i);
            exp = false;
            break;
          // If the present neighbor is on the right.
          } else if (i == Direction::Right) {
            // If I am expanded and nobody is on the bottom right.
            if(!hasNbrAtGlobalDir(Direction::BottomRight, false) && canContract) {
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

    // Head expanded on the left.
    case Direction::Right:
      for (int i = 2; i >= 0; i--) {
        if (hasNbrAtGlobalDir(i, false)) {
          HoleFillerParticle& other = nbrAtGlobalDir(i, false);

          // If the neighbor can be pulled.
          if (other.isContracted()) {
            pullFromGlobalDir(i);
            exp = false;
            break;
          // If the present neighbor is on the right.
          } else if (i == Direction::Right) {
            // If the neighbor is expanded in a direction opposed to my head.
            if (other.globalTailDir == Direction::Left && canContract) {
              contractTail();
              expandToGlobalDir(Direction::Right);
            }
          }

          canContract = false;
        }
      }

      if (exp &&
          canContract &&
          hasNbrAtGlobalDir(Direction::BottomRight, false) &&
          !hasNbrAtGlobalDir(Direction::BottomLeft, false)) {
        contractTail();
        expandToGlobalDir(Direction::BottomRight);
        canContract = false;
      }

      break;

    default:
      break;
    }

    if (exp && canContract) {
      contractTail();
    }
  }
}

QString HoleFillerParticle::inspectionText() const
{
  QString text;
  text += "Global Info:\n";
  text += "  head: (" + QString::number(head.x) + ", "
      + QString::number(head.y) + ")\n";
  text += "  orientation: " + QString::number(orientation) + "\n";
  text += "  globalTailDir: " + QString::number(globalTailDir) + "\n";
  text += "  left?: " + QString(hasNbrAtGlobalDir(Direction::Left) ? "oui" : "non") + "\n";
  text += "  right?: " + QString(hasNbrAtGlobalDir(Direction::Right) ? "oui" : "non") + "\n";
  text += "  topleft?: " + QString(hasNbrAtGlobalDir(Direction::TopLeft) ? "oui" : "non") + "\n";
  text += "  botleft?: " + QString(hasNbrAtGlobalDir(Direction::BottomLeft) ? "oui" : "non") + "\n";
  text += "  topright?: " + QString(hasNbrAtGlobalDir(Direction::TopRight) ? "oui" : "non") + "\n";
  text += "  botright?: " + QString(hasNbrAtGlobalDir(Direction::BottomRight) ? "oui" : "non") + "\n";

  return text;
}

int HoleFillerParticle::headMarkColor() const
{
  return 0x1010FF ; // Blue
}

int HoleFillerParticle::tailMarkColor() const
{
  return 0xfffff0;
}

HoleFillerSystem::HoleFillerSystem(unsigned int numParticles, double holeProb){
  Q_ASSERT(numParticles > 0);
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
