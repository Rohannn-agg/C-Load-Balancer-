#include "LeastConnectionsStrategy.h"
#include <limits>

std::shared_ptr<Server> LeastConnectionsStrategy::getServer(const std::vector<std::shared_ptr<Server>>& servers) {
    if (servers.empty()) return nullptr;

    std::shared_ptr<Server> bestServer = nullptr;
    int minConnections = std::numeric_limits<int>::max();

    for (const auto& server : servers) {
        if (server->isHealthy() && server->getActiveConnections() < server->getCapacity()) {
            int currentConnections = server->getActiveConnections();
            if (currentConnections < minConnections) {
                minConnections = currentConnections;
                bestServer = server;
            }
        }
    }

    return bestServer;
}
