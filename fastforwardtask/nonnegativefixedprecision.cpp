#include "nonnegativefixedprecision.h"
#include <cassert>
#include <iomanip>
#include <charconv>
#include <system_error>
#include <cmath>
#include <string>
#include <string_view>
#include <sstream>
#include "exceptions.h"

template <typename T>
T ParseNumber(std::string_view from) {
    T number;
    std::from_chars_result result = std::from_chars(from.begin(), from.end(), number);
    if (std::errc() != result.ec) {
        throw ValueError(std::make_error_code(result.ec).message());
    }
    return number;
}

NonNegativeFixedPrecision::NonNegativeFixedPrecision(std::string_view from) {
    const std::string_view::size_type dot_pos = from.find(".");
    const std::string_view::size_type e_pos = from.find("e");
    const std::string_view integer_part = from.substr(0, dot_pos);

    nominator_ = ParseNumber<std::uint64_t>(integer_part);

    if (std::string_view::npos != dot_pos) {
        const std::string_view::size_type num_len = (std::string_view::npos != e_pos ? e_pos : from.size()) - dot_pos - 1;
        denominator_ = static_cast<std::uint64_t>(std::pow(10, num_len));
        nominator_ = nominator_ * denominator_ + ParseNumber<std::uint64_t>(from.substr(dot_pos + 1, e_pos));
    }

    if (std::string_view::npos == e_pos) {
        return;
    }

    const std::string_view exp = from.substr(e_pos + 1);
    const int additional_denominator = ParseNumber<int>(exp);
    if (additional_denominator < 0) {
        denominator_ *= static_cast<std::uint64_t>(std::pow(10, std::abs(additional_denominator)));
    } else {
        nominator_ *= static_cast<std::uint64_t>(std::pow(10, additional_denominator));
    }
}

std::string NonNegativeFixedPrecision::ToString() const  {
    assert(0 != denominator_);
    const std::uint64_t integer_part = nominator_ / denominator_;
    const std::uint64_t fractional_part = nominator_ - integer_part * denominator_;

    std::ostringstream output;
    output << integer_part
           << "."
           << std::setw(std::log10(denominator_)) << std::setfill('0') << fractional_part;
    return output.str();
}
