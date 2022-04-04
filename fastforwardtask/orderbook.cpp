#include "orderbook.h"
#include <algorithm>

OrderBook::OrderBook(const std::vector<Level> &bids, const std::vector<Level> &asks) {
    std::transform(bids.begin(), bids.end(),
                   std::inserter(bids_, bids_.end()),
                   [](const Level& lvl) { return std::make_pair(lvl.price, lvl.amount); });
    std::transform(asks.begin(), asks.end(),
                   std::inserter(asks_, asks_.end()),
                   [](const Level& lvl) { return std::make_pair(lvl.price, lvl.amount); });
}

void OrderBook::UpdateBids(const std::vector<Level>& bids) {
    for (const Level& lvl : bids) {
        if (0 == lvl.amount.GetNominator()) {
            const auto& it = bids_.find(lvl.price);
            if (bids_.end() != it) {
                bids_.erase(it);
            }
        } else {
            bids_[lvl.price] = lvl.amount;
        }
    }
}

void OrderBook::UpdateAsks(const std::vector<Level>& asks) {
    for (const Level& lvl : asks) {
        if (0 == lvl.amount.GetNominator()) {
            const auto& it = asks_.find(lvl.price);
            if (asks_.end() != it) {
                asks_.erase(it);
            }
        } else {
            asks_[lvl.price] = lvl.amount;
        }
    }
}

std::optional<Level> OrderBook::GetBestBid() const {
    const auto it = bids_.rbegin();
    if (bids_.rend() == it) {
        return std::optional<Level>();
    }
    return std::optional<Level>({it->first, it->second});
}

std::optional<Level> OrderBook::GetBestAsk() const {
    const auto it = asks_.begin();
    if (asks_.end() == it) {
        return std::optional<Level>();
    }
    return std::optional<Level>({it->first, it->second});
}
