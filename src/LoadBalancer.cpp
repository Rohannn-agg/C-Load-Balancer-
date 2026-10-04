#include "LoadBalancer.h"
#include "RoundRobinStrategy.h"
#include "LeastConnectionsStrategy.h"
#include "RandomStrategy.h"
#include "WeightedRoundRobinStrategy.h"
#include <iostream>

LoadBalancer::LoadBalancer(StrategyType strategyType)
    : stopHealthCheck_(false),
      totalRequestsHandled_(0),
      failedRequests_(0) {
    strategy_ = createStrategy(strategyType);
    
    // Start background health check thread
    healthCheckThread_ = std::thread(&LoadBalancer::healthCheckLoop, this);
}

LoadBalancer::~LoadBalancer() {
    stopHealthCheck_.store(true, std::memory_order_release);
    if (healthCheckThread_.joinable()) {
        healthCheckThread_.join();
    }
}

std::unique_ptr<ILoadBalancingStrategy> LoadBalancer::createStrategy(StrategyType type) {
    switch (type) {
        case StrategyType::LeastConnections:
            return std::make_unique<LeastConnectionsStrategy>();
        case StrategyType::Random:
            return std::make_unique<RandomStrategy>();
        case StrategyType::WeightedRoundRobin:
            return std::make_unique<WeightedRoundRobinStrategy>();
        case StrategyType::RoundRobin:
        default:
            return std::make_unique<RoundRobinStrategy>();
    }
}

void LoadBalancer::setStrategy(StrategyType strategyType) {
    std::lock_guard<std::mutex> lock(strategyMutex_);
    strategy_ = createStrategy(strategyType);
}

void LoadBalancer::addServer(std::shared_ptr<Server> server) {
    std::unique_lock<std::shared_mutex> lock(rwMutex_);
    // Check for duplicates
    for (const auto& s : servers_) {
        if (s->getId() == server->getId()) {
            std::cerr << "Warning: Server with ID " << server->getId() << " already exists." << std::endl;
            return; 
        }
    }
    servers_.push_back(server);
}

bool LoadBalancer::removeServer(const std::string& id) {
    std::unique_lock<std::shared_mutex> lock(rwMutex_);
    for (auto it = servers_.begin(); it != servers_.end(); ++it) {
        if ((*it)->getId() == id) {
            servers_.erase(it);
            return true;
        }
    }
    std::cerr << "Warning: Attempted to remove non-existent server ID " << id << std::endl;
    return false;
}

bool LoadBalancer::routeRequest() {
    std::shared_ptr<Server> selectedServer = nullptr;

    {
        // Acquire read lock for server selection
        std::shared_lock<std::shared_mutex> rwLock(rwMutex_);
        std::lock_guard<std::mutex> stratLock(strategyMutex_); // Protect the strategy logic
        
        selectedServer = strategy_->getServer(servers_);
    }

    if (!selectedServer) {
        failedRequests_.fetch_add(1, std::memory_order_relaxed);
        return false; // No healthy servers available or all at capacity
    }

    // Process request
    selectedServer->incrementConnections();
    
    bool success = selectedServer->processRequest();
    
    selectedServer->decrementConnections();

    if (success) {
        totalRequestsHandled_.fetch_add(1, std::memory_order_relaxed);
    } else {
        failedRequests_.fetch_add(1, std::memory_order_relaxed);
    }

    return success;
}

void LoadBalancer::healthCheckLoop() {
    while (!stopHealthCheck_.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Randomly simulate server failures and recoveries for testing purposes
        // In a real LB, this would ping a /health endpoint
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        for (auto& server : servers_) {
            // 2% chance to toggle health
            if (rand() % 100 < 2) {
                bool isHealthy = server->isHealthy();
                server->setHealthy(!isHealthy);
            }
        }
    }
}

int LoadBalancer::getTotalRequestsHandled() const {
    return totalRequestsHandled_.load(std::memory_order_relaxed);
}

int LoadBalancer::getFailedRequests() const {
    return failedRequests_.load(std::memory_order_relaxed);
}

void LoadBalancer::printMetrics() const {
    std::shared_lock<std::shared_mutex> lock(rwMutex_);
    std::cout << "\n--- Load Balancer Metrics ---\n";
    std::cout << "Total Requests Handled: " << getTotalRequestsHandled() << "\n";
    std::cout << "Failed Requests: " << getFailedRequests() << "\n\n";
    
    std::cout << "Server Distribution:\n";
    for (const auto& server : servers_) {
        std::cout << "Server " << server->getId() 
                  << " [Weight: " << server->getWeight() << "]"
                  << " - Handled: " << server->getTotalRequestsHandled() 
                  << " - Health: " << (server->isHealthy() ? "UP" : "DOWN") << "\n";
    }
    std::cout << "-----------------------------\n";
}
