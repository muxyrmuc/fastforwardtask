#ifndef FASTFORWARDTASK_LEVEL_H_
#define FASTFORWARDTASK_LEVEL_H_

#include "nonnegativefixedprecision.h"

struct Level {
    NonNegativeFixedPrecision price;
    NonNegativeFixedPrecision amount;
};

#endif  // FASTFORWARDTASK_LEVEL_H_
