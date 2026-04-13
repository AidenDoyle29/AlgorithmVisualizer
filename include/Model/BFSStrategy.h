#pragma once
#include "Algorithm.h"
#include <queue>

/**
 * @class BFSStrategy
 * @brief Concrete implementation of the BFS algorithm following the Strategy Pattern.
 */
class BFSStrategy : public Algorithm {
public:
    /**
     * @brief Runs the BFS algorithm and captures snapshots for visualization (Memento Pattern).
     */
    std::vector<SimulationState> run(Graph &graph, const AlgorithmConfig &config) override;
};
