#pragma once

#include "ILoadBalancingStrategy.h"
#include <atomic>

class RoundRobinStrategy : public ILoadBalancingStrategy {
public:
    RoundRobinStrategy();
    std::shared_ptr<Server> getServer(const std::vector<std::shared_ptr<Server>>& servers) override;

private:
    std::atomic<size_t> currentIndex_;
};
