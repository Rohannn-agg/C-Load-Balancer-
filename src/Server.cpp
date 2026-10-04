#include "Server.h"

Server::Server(std::string id, int capacity, int weight)
    : id_(std::move(id)),
      isHealthy_(true),
      activeConnections_(0),
      capacity_(capacity),
      weight_(weight),
      totalRequestsHandled_(0) {}

std::string Server::getId() const {
    return id_;
}

bool Server::isHealthy() const {
    return isHealthy_.load(std::memory_order_acquire);
}

void Server::setHealthy(bool healthy) {
    isHealthy_.store(healthy, std::memory_order_release);
}

int Server::getActiveConnections() const {
    return activeConnections_.load(std::memory_order_acquire);
}

int Server::getCapacity() const {
    return capacity_;
}

int Server::getWeight() const {
    return weight_;
}

void Server::incrementConnections() {
    activeConnections_.fetch_add(1, std::memory_order_acq_rel);
}

void Server::decrementConnections() {
    activeConnections_.fetch_sub(1, std::memory_order_acq_rel);
}

int Server::getTotalRequestsHandled() const {
    return totalRequestsHandled_.load(std::memory_order_relaxed);
}

bool Server::processRequest() {
    if (!isHealthy()) return false;
    
    // We increment connections externally before calling this in the router,
    // but we can check if it exceeds capacity
    if (getActiveConnections() > capacity_) {
        return false; // Capacity overflow
    }

    // Simulate work (e.g. 5-15 milliseconds)
    // In a real application, this would be an actual network/IO call.
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    
    totalRequestsHandled_.fetch_add(1, std::memory_order_relaxed);
    
    return true;
}
