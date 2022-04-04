#include "microbenchmark.h"

void MicroBenchmark::Start() {
    ++runs_count_;
    start_time_ = HighResolutionSteadyClock::now();
}

void MicroBenchmark::Stop() {
    const HighResolutionSteadyClock::time_point stop_time = HighResolutionSteadyClock::now();
    const auto nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time_);
    total_time_nsec_ += nanosec.count();
}

MicroBenchmark::Result MicroBenchmark::GetResult() const {
    return runs_count_ ? Result{total_time_nsec_ / runs_count_} : Result{0};
}

MicroBenchmarkRunGuard::MicroBenchmarkRunGuard(MicroBenchmark &benchmark)
    : benchmark_(benchmark) {
    benchmark_.Start();
}

MicroBenchmarkRunGuard::~MicroBenchmarkRunGuard() {
    benchmark_.Stop();
}
