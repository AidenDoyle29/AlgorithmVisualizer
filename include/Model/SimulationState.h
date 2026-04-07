#pragma once
#include <vector>
#include <unordered_map>

enum class NodeStatus {
    UNVISITED,
    FRONTIER,
    CURRENT,
    VISIT
};

struct SimulationState {
    std::unordered_map<int, NodeStatus> nodeStatuses;
    std::unordered_map<int, bool> activeEdges;

    int stepNumber = 0;
};