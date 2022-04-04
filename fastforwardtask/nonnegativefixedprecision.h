#ifndef FASTFORWARDTASK_NONNEGATIVEFIXEDPRECISION_H_
#define FASTFORWARDTASK_NONNEGATIVEFIXEDPRECISION_H_

#include <string_view>
#include <string>

class NonNegativeFixedPrecision {
    std::uint64_t nominator_ = 0;
    std::uint64_t denominator_ = 1;

public:
    NonNegativeFixedPrecision() = default;
    NonNegativeFixedPrecision(std::string_view from);

    std::string ToString() const;

    inline float ToFloat() const { return float(nominator_) / denominator_; }
    inline std::uint64_t GetNominator() const { return nominator_; }
    inline std::uint64_t GetDenominator() const { return denominator_; }
    inline bool operator <(const NonNegativeFixedPrecision& rhs) const { return ToFloat() < rhs.ToFloat(); }
};

#endif  // FASTFORWARDTASK_NONNEGATIVEFIXEDPRECISION_H_
