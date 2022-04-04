#ifndef FASTFORWARDTASK_MICROBENCHMARK_H_
#define FASTFORWARDTASK_MICROBENCHMARK_H_

#include <vector>
#include <cstdint>
#include "highresolutionsteadyclock.h"

class MicroBenchmark {
    double total_time_nsec_ = 0;  // TODO: support standard deviation
    std::size_t runs_count_ = 0;
    HighResolutionSteadyClock::time_point start_time_;

public:
    struct Result {
        double average_time_nsec;
    };

    void Start();
    void Stop();
    Result GetResult() const;
};

class MicroBenchmarkRunGuard {
    MicroBenchmark& benchmark_;

public:
    MicroBenchmarkRunGuard(MicroBenchmark& benchmark);
    ~MicroBenchmarkRunGuard();

    MicroBenchmarkRunGuard(const MicroBenchmarkRunGuard&) = delete;
    MicroBenchmarkRunGuard& operator =(const MicroBenchmarkRunGuard&) = delete;
};

#endif  // FASTFORWARDTASK_MICROBENCHMARK_H_
