#ifndef FASTFORWARDTASK_HIGHRESOLUTIONSTEADYCLOCK_H_
#define FASTFORWARDTASK_HIGHRESOLUTIONSTEADYCLOCK_H_

#if defined(__linux__) || _MSC_VER >= 1910
#include <chrono>
using HighResolutionSteadyClock = std::chrono::steady_clock;
#else
#error Unknown high resolution steady clock type for your platform
#endif

#endif  // FASTFORWARDTASK_HIGHRESOLUTIONSTEADYCLOCK_H_
