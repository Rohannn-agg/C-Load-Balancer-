#pragma once

#include <vector>
#include <memory>
#include "Server.h"

class ILoadBalancingStrategy {
public:
    virtual ~ILoadBalancingStrategy() = default;
    
    /**
     * @brief Selects the next appropriate server based on the routing algorithm.
     * 
     * @param servers A list of servers. The strategy is responsible for picking
     *                a healthy one, or returning nullptr if none are available.
     * @return std::shared_ptr<Server> The selected server, or nullptr.
     */
    virtual std::shared_ptr<Server> getServer(const std::vector<std::shared_ptr<Server>>& servers) = 0;
};
