#include "RandomStrategy.h"

RandomStrategy::RandomStrategy() {
    std::random_device rd;
    generator_.seed(rd());
}

std::shared_ptr<Server> RandomStrategy::getServer(const std::vector<std::shared_ptr<Server>>& servers) {
    if (servers.empty()) return nullptr;

    std::vector<std::shared_ptr<Server>> availableServers;
    for (const auto& server : servers) {
        if (server->isHealthy() && server->getActiveConnections() < server->getCapacity()) {
            availableServers.push_back(server);
        }
    }

    if (availableServers.empty()) return nullptr;

    std::uniform_int_distribution<size_t> distribution(0, availableServers.size() - 1);
    return availableServers[distribution(generator_)];
}
