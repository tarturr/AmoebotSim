#include "core/globalparticle.h"


int GlobalParticle::globalDirToLabel(int dir, bool head) const
{
    return head
        ? dirToHeadLabel(globalToLocalDir(dir))
        : dirToTailLabel(globalToLocalDir(dir));
}

bool GlobalParticle::hasNbrAtGlobalDir(int dir, bool head) const
{
    return hasNbrAtLabel(globalDirToLabel(dir, head));
}

void GlobalParticle::expandToGlobalDir(int dir)
{
    expand(globalDirToLabel(dir));
}

void GlobalParticle::pushToGlobalDir(int dir)
{
    push(globalDirToLabel(dir));
}

void GlobalParticle::pullFromGlobalDir(int dir)
{
    pull(globalDirToLabel(dir, false));
}
