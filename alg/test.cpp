#include "alg/test.h"



TestParticle::TestParticle(const Node& head,
                           const int globalTailDir,
                           const int orientation,
                           AmoebotSystem& system)
    : AmoebotParticle(head, globalTailDir, orientation, system),
    botL (4), botR (5), left (3), right (0), topL (2), topR (1){}

void TestParticle::activate() {
    bool exp = isExpanded();
   //si la particule est contractée
    if (!exp){
        if (!hasNbrAtLabel(GetLabel(botL)) && hasNbrAtLabel(GetLabel(botR))) {//test d'une place au botL
            expand(GetLabel(botL));
            exp = true;
        }else if(!hasNbrAtLabel(GetLabel(botR)) && hasNbrAtLabel(GetLabel(botL))) {//test d'une place au botR
            expand(GetLabel(botR));
            exp = true;
        }
    }
    //si la particule est étendue
    if (exp) {
        bool canContract = true;
        switch (globalTailDir) {
        case 2://fleche vers bot right
            for (int i = 1; i < 4; i++){
                if (hasNbrAtLabel(GetLabel(i))) {
                    TestParticle& other = NbrAtLabel(GetLabel(i));
                    if (other.isContracted()){//si il y a un voisin tirable
                        pull(GetLabel(i));
                        exp = false;
                        break;
                    }else if (i == 3){
                        if(!hasNbrAtLabel(GetLabel(botL)) && canContract){//si je suis arrivé a 3 qu'il est étendu et qu'il y personne entre nous
                            contractTail();
                            expand(GetLabel(left));
                            canContract = false;
                        }
                        break;
                    }
                    canContract = false;
                }
            }
            break;
        case 3:   //fleche vers right
            for (int i = 1; i < 4; i++){
                if (hasNbrAtLabel(GetLabel(i))) {
                    TestParticle& other = NbrAtLabel(GetLabel(i));
                    if (other.isContracted()){//si il y a un voisin tirable
                        pull(GetLabel(i));
                        exp = false;
                        break;
                    }else if (i == 3){
                        if (other.globalTailDir == 0 && canContract){//si je suis arrivé a 3 qu'il est étendu à l'opposé
                            contractTail();
                            expand(GetLabel(left));
                        }
                    }
                    canContract = false;
                }
            }
            if (exp && canContract && hasNbrAtLabel(GetLabel(botL)) && !hasNbrAtLabel(GetLabel(botR))){
                contractTail();
                expand(GetLabel(botL));
                canContract = false;
            }
            break;

        case 1://fleche vers bot left
            for (int i = 2; i > -1; i--){
                if (hasNbrAtLabel(GetLabel(i))) {
                    TestParticle& other = NbrAtLabel(GetLabel(i));
                    if (other.isContracted()){//si il y a un voisin tirable
                        pull(GetLabel(i));
                        exp = false;
                        break;
                    }else if (i == 0){
                        if(!hasNbrAtLabel(GetLabel(botR)) && canContract){//si je suis arrivé a 0 qu'il est étendu et qu'il y personne entre nous
                            contractTail();
                            expand(GetLabel(right));
                            canContract = false;
                        }
                        break;
                    }
                    canContract = false;
                }
            }
            break;
        case 0:   //fleche vers left
            for (int i = 2; i > -1; i--){
                if (hasNbrAtLabel(GetLabel(i))) {
                    TestParticle& other = NbrAtLabel(GetLabel(i));
                    if (other.isContracted()){//si il y a un voisin tirable
                        pull(GetLabel(i));
                        exp = false;
                        break;
                    }else if (i == 0){
                        if (other.globalTailDir == 3 && canContract){//si je suis arrivé a 0 qu'il est étendu à l'opposé
                            contractTail();
                            expand(GetLabel(right));
                        }
                    }
                    canContract = false;
                }
            }
            if (exp && canContract && hasNbrAtLabel(GetLabel(botR)) && !hasNbrAtLabel(GetLabel(botL))){
                contractTail();
                expand(GetLabel(botR));
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

QString TestParticle::inspectionText() const
{
    QString text;
    text += "Global Info:\n";
    text += "  head: (" + QString::number(head.x) + ", "
            + QString::number(head.y) + ")\n";
    text += "  orientation: " + QString::number(orientation) + "\n";
    text += "  globalTailDir: " + QString::number(globalTailDir) + "\n";
    text += "  leftLab: " + QString::number(GetLabel(left));
    text += "  left?: " + QString(hasNbrAtLabel(GetLabel(left)) ? "oui" : "non") + "\n";
    text += "  rightLab: " + QString::number(GetLabel(right));
    text += "  right?: " + QString(hasNbrAtLabel(GetLabel(right)) ? "oui" : "non")+ "\n";
    text += "  topleftLab: " + QString::number(GetLabel(topL));
    text += "  topleft?: " + QString(hasNbrAtLabel(GetLabel(topL)) ? "oui" : "non")+ "\n";
    text += "  botleftLab: " + QString::number(GetLabel(botL));
    text += "  botleft?: " + QString(hasNbrAtLabel(GetLabel(botL)) ? "oui" : "non")+ "\n";
    text += "  toprightLab: " + QString::number(GetLabel(topR));
    text += "  topright?: " + QString(hasNbrAtLabel(GetLabel(topR)) ? "oui" : "non")+ "\n";
    text += "  botrightLab: " + QString::number(GetLabel(botR));
    text += "  botright?: " + QString(hasNbrAtLabel(GetLabel(botR)) ? "oui" : "non")+ "\n";


    return text;
}

int TestParticle::headMarkColor() const
{
    return 0x1010FF ; // Blue
}

int TestParticle::tailMarkColor() const
{
    return 0xfffff0;
}


TestParticle& TestParticle::NbrAtLabel(int label) {
    return AmoebotParticle::nbrAtLabel<TestParticle>(label);
}

int TestParticle::GetLabel(int dir) const{
    if (isContracted())
        return dirToHeadLabel(globalToLocalDir(dir));
    else
        return dirToTailLabel(globalToLocalDir(dir));

}

bool TestParticle::isTail(int label){
    assert(hasNbrAtLabel(label));
    TestParticle& other = NbrAtLabel(label);
    assert(other.isExpanded());
    int previousHeadDir = (other.globalTailDir + 3) % 6;
    other.contractHead();
    if (hasNbrAtLabel(label)){
        other.expand(other.GetLabel(previousHeadDir));
        return true;
    }else{
        other.expand(other.GetLabel(previousHeadDir));
        return false;
    }
}
TestParticleSystem::TestParticleSystem(unsigned int numParticles, double holeProb){
    Q_ASSERT(numParticles > 0);
    Q_ASSERT(0 <= holeProb && holeProb <= 1);

    // Insert the seed at (0,0).
    insert(new TestParticle(Node(0, 0), -1, randDir(), *this));
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
            insert(new TestParticle(randomCandidate, -1, randDir(), *this));
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
