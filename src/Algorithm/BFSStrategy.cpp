#include "BFSStrategy.h"
#include "Graph.h"
#include "SimulationState.h"
#include <unordered_map>

std::vector<SimulationState> BFSStrategy::run(Graph &graph, const AlgorithmConfig &config) {
    std::vector<SimulationState> states;
    int startNode = config.startNodeID;
    
    const auto& allNodes = graph.getNodes();
    if (allNodes.find(startNode) == allNodes.end()) {
        return states; // Empty if start node invalid
    }

    std::queue<int> q;
    std::unordered_map<int, NodeStatus> currentStatuses;
    
    // Initialize all to UNVISITED
    for (auto const& [id, node] : allNodes) {
        currentStatuses[id] = NodeStatus::UNVISITED;
    }

    // Start BFS
    q.push(startNode);
    currentStatuses[startNode] = NodeStatus::FRONTIER;

    int step = 0;
    
    // Initial state
    states.push_back({currentStatuses, {}, step++});

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        currentStatuses[u] = NodeStatus::CURRENT;
        states.push_back({currentStatuses, {}, step++});

        for (int v : graph.getNeighbors(u)) {
            if (currentStatuses[v] == NodeStatus::UNVISITED) {
                currentStatuses[v] = NodeStatus::FRONTIER;
                q.push(v);
                
                // Show exploration step
                states.push_back({currentStatuses, {}, step++});
            }
        }

        currentStatuses[u] = NodeStatus::VISIT;
        states.push_back({currentStatuses, {}, step++});
    }

    return states;
}
