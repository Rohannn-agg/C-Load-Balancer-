#pragma once

#include "ILoadBalancingStrategy.h"
#include <random>

class RandomStrategy : public ILoadBalancingStrategy {
public:
    RandomStrategy();
    std::shared_ptr<Server> getServer(const std::vector<std::shared_ptr<Server>>& servers) override;

private:
    std::mt19937 generator_;
};
