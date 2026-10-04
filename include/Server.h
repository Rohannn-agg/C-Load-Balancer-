#pragma once

#include <string>
#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>

class Server {
public:
    Server(std::string id, int capacity, int weight = 1);
    
    // Disable copy semantics to ensure thread safety over atomics
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    std::string getId() const;
    
    // Health status management
    bool isHealthy() const;
    void setHealthy(bool healthy);
    
    // Connection tracking
    int getActiveConnections() const;
    int getCapacity() const;
    int getWeight() const;
    
    // Request processing simulation
    bool processRequest();
    void incrementConnections();
    void decrementConnections();

    // Statistics tracking (bonus)
    int getTotalRequestsHandled() const;

private:
    std::string id_;
    std::atomic<bool> isHealthy_;
    std::atomic<int> activeConnections_;
    int capacity_;
    int weight_;
    std::atomic<int> totalRequestsHandled_;
};
