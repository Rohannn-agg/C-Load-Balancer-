#include "WeightedRoundRobinStrategy.h"
#include <algorithm>

WeightedRoundRobinStrategy::WeightedRoundRobinStrategy() 
    : currentIndex_(0), currentWeight_(0) {}

std::shared_ptr<Server> WeightedRoundRobinStrategy::getServer(const std::vector<std::shared_ptr<Server>>& servers) {
    if (servers.empty()) return nullptr;

    size_t numServers = servers.size();
    
    // Calculate max weight and GCD of weights to avoid doing it every request
    int maxWeight = 0;
    for (const auto& s : servers) {
        if (s->isHealthy()) {
            maxWeight = std::max(maxWeight, s->getWeight());
        }
    }

    if (maxWeight == 0) return nullptr; // No healthy servers with weight > 0

    // Prevent infinite loop if all servers are at capacity
    size_t attempts = 0;
    size_t maxAttempts = numServers * maxWeight;

    while (attempts < maxAttempts) {
        size_t index = currentIndex_.load(std::memory_order_relaxed);
        
        if (index == 0) {
            currentWeight_.fetch_sub(1, std::memory_order_relaxed);
            if (currentWeight_.load(std::memory_order_relaxed) <= 0) {
                currentWeight_.store(maxWeight, std::memory_order_relaxed);
                if (maxWeight == 0) return nullptr;
            }
        }

        size_t nextIndex = (index + 1) % numServers;
        currentIndex_.store(nextIndex, std::memory_order_relaxed);

        auto server = servers[index];
        if (server->isHealthy() && server->getWeight() >= currentWeight_.load(std::memory_order_relaxed)
            && server->getActiveConnections() < server->getCapacity()) {
            return server;
        }
        attempts++;
    }

    return nullptr;
}
