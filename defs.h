#ifndef DEFS_H
#define DEFS_H

#include <QDebug>
#include "constants.h"

inline int randint(int lo, int hi)
{
    return lo + qrand() % (hi - lo);
}

inline double randf(double lo, double hi)
{
    return lo + (hi - lo) * qrand() / RAND_MAX;
}

#endif // DEFS_H
