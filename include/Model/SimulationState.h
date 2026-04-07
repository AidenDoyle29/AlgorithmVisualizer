/**
 * @file SimulationState.h
 * @brief Header file for the simulation state. Each state will contain crucial information about the graph that
 * the view will use for rendering.
 */

#pragma once
#include <vector>
#include <unordered_map>

/**
 * @enum NodeStatus
 * @brief An enum class that indicates the colors of unvisited, frontier, current, and visit node statuses.
 */
enum class NodeStatus {
    UNVISITED,
    FRONTIER,
    CURRENT,
    VISIT
};

/**
 * @struct SimulationState
 * @brief A struct that contains one state of the simulation. This state includes hashmaps for node statuses and active edges, 
 * as well as the state's respective step number.
 */
struct SimulationState {
    std::unordered_map<int, NodeStatus> nodeStatuses;
    std::unordered_map<int, bool> activeEdges;

    int stepNumber = 0;
};