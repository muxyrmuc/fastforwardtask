#ifndef FASTFORWARDTASK_SERIALIZATIONUTILS_H_
#define FASTFORWARDTASK_SERIALIZATIONUTILS_H_

#include <string_view>
#include <cstdint>
#include <vector>
#include <optional>
#include "level.h"

struct Message {
    enum class Type {
        kSnapshot,
        kUpdate
    };

    std::vector<Level> bids;
    std::vector<Level> asks;
    std::uint64_t event_ts_mcsec;
    Type type;
};

Message ParseLogLine(std::string_view line);
std::string SaveToJson(std::uint64_t event_ts_mcsec,
                       const std::optional<Level>& best_bid,
                       const std::optional<Level>& best_ask);

#endif  // FASTFORWARDTASK_SERIALIZATIONUTILS_H_
