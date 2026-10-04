#pragma once

#include <vector>
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <thread>
#include <atomic>
#include "Server.h"
#include "ILoadBalancingStrategy.h"

enum class StrategyType {
    RoundRobin,
    LeastConnections,
    Random,
    WeightedRoundRobin
};

class LoadBalancer {
public:
    LoadBalancer(StrategyType strategyType = StrategyType::RoundRobin);
    ~LoadBalancer();

    // Server pool management
    void addServer(std::shared_ptr<Server> server);
    bool removeServer(const std::string& id);
    
    // Strategy management
    void setStrategy(StrategyType strategyType);

    // Core functionality
    bool routeRequest();

    // Metrics
    int getTotalRequestsHandled() const;
    int getFailedRequests() const;
    void printMetrics() const;

private:
    void healthCheckLoop();
    std::unique_ptr<ILoadBalancingStrategy> createStrategy(StrategyType type);

    std::vector<std::shared_ptr<Server>> servers_;
    mutable std::shared_mutex rwMutex_; // Protects the servers_ list
    
    std::unique_ptr<ILoadBalancingStrategy> strategy_;
    std::mutex strategyMutex_; // Protects strategy swapping

    // Health checking
    std::thread healthCheckThread_;
    std::atomic<bool> stopHealthCheck_;

    // Metrics
    std::atomic<int> totalRequestsHandled_;
    std::atomic<int> failedRequests_;
};
