#include "RoundRobinStrategy.h"

RoundRobinStrategy::RoundRobinStrategy() : currentIndex_(0) {}

std::shared_ptr<Server> RoundRobinStrategy::getServer(const std::vector<std::shared_ptr<Server>>& servers) {
    if (servers.empty()) return nullptr;

    size_t numServers = servers.size();
    size_t startIndex = currentIndex_.fetch_add(1, std::memory_order_relaxed) % numServers;
    size_t index = startIndex;

    // Find the first healthy server
    do {
        if (servers[index]->isHealthy() && servers[index]->getActiveConnections() < servers[index]->getCapacity()) {
            return servers[index];
        }
        index = (index + 1) % numServers;
    } while (index != startIndex);

    return nullptr;
}
