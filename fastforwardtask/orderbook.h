#ifndef FASTFORWARDTASK_ORDERBOOK_H_
#define FASTFORWARDTASK_ORDERBOOK_H_

#include <map>
#include <optional>
#include <vector>
#include "nonnegativefixedprecision.h"
#include "level.h"

class OrderBook {
    // TODO: switch to sorted array because it must be faster on small amounts of data
    std::map<NonNegativeFixedPrecision, NonNegativeFixedPrecision> bids_;
    std::map<NonNegativeFixedPrecision, NonNegativeFixedPrecision> asks_;

public:
    OrderBook(const std::vector<Level>& bids, const std::vector<Level>& asks);

    void UpdateBids(const std::vector<Level>& bids);
    void UpdateAsks(const std::vector<Level>& asks);

    std::optional<Level> GetBestBid() const;
    std::optional<Level> GetBestAsk() const;
};

#endif  // FASTFORWARDTASK_ORDERBOOK_H_
