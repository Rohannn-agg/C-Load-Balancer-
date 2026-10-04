#pragma once

#include "ILoadBalancingStrategy.h"

class LeastConnectionsStrategy : public ILoadBalancingStrategy {
public:
    std::shared_ptr<Server> getServer(const std::vector<std::shared_ptr<Server>>& servers) override;
};
