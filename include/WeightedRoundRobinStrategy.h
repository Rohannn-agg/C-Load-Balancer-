#pragma once

#include "ILoadBalancingStrategy.h"
#include <atomic>

class WeightedRoundRobinStrategy : public ILoadBalancingStrategy {
public:
    WeightedRoundRobinStrategy();
    std::shared_ptr<Server> getServer(const std::vector<std::shared_ptr<Server>>& servers) override;

private:
    std::atomic<size_t> currentIndex_;
    std::atomic<int> currentWeight_;
};
