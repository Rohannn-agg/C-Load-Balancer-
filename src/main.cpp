#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include "LoadBalancer.h"

void clientThread(LoadBalancer& lb, int numRequests) {
    for (int i = 0; i < numRequests; ++i) {
        lb.routeRequest();
        
        // Random jitter between requests
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5));
    }
}

int main() {
    std::cout << "Starting Load Balancer Simulation...\n";

    LoadBalancer lb(StrategyType::RoundRobin);

    // Create 10 servers with capacity 100 and varying weights
    for (int i = 1; i <= 10; ++i) {
        int weight = (i % 3) + 1; // Weights 1, 2, 3
        auto server = std::make_shared<Server>("S" + std::to_string(i), 100, weight);
        lb.addServer(server);
    }

    const int NUM_CLIENTS = 100;
    const int REQUESTS_PER_CLIENT = 100;
    
    std::vector<std::thread> clients;

    // Run Round Robin for a bit
    std::cout << "Running Round Robin...\n";
    for (int i = 0; i < NUM_CLIENTS / 2; ++i) {
        clients.emplace_back(clientThread, std::ref(lb), REQUESTS_PER_CLIENT);
    }

    // Wait for the first half to finish
    for (auto& t : clients) {
        if (t.joinable()) t.join();
    }
    clients.clear();
    
    lb.printMetrics();

    // Switch to Least Connections
    std::cout << "\nSwitching Strategy to Least Connections...\n";
    lb.setStrategy(StrategyType::LeastConnections);
    
    for (int i = 0; i < NUM_CLIENTS / 2; ++i) {
        clients.emplace_back(clientThread, std::ref(lb), REQUESTS_PER_CLIENT);
    }
    
    for (auto& t : clients) {
        if (t.joinable()) t.join();
    }
    clients.clear();

    lb.printMetrics();

    std::cout << "Simulation Complete.\n";
    return 0;
}
