/**
 * @file DepthFirstSearch.h
 * @brief Defines the Depth-First Search (DFS) algorithm strategy.
 *
 * This file implements the concrete strategy for the DFS graph traversal 
 * and pathfinding algorithm, conforming to the IAlgorithm interface.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_DEPTH_FIRST_SEARCH_H
#define ALGORITHM_VISUALIZER_ALGORITHM_DEPTH_FIRST_SEARCH_H

#include "Algorithm/Algorithm.h"

namespace av::algorithm {

/**
 * @class DepthFirstSearch
 * @brief Executes a depth-first traversal or pathfinding search on a graph.
 *
 * The DepthFirstSearch class is a concrete implementation of the IAlgorithm 
 * interface (Strategy Pattern). It explores as far as possible along each 
 * branch before backtracking. As it executes, it generates PlaybackFrames 
 * to visualize the deep exploration and backtracking phases of the graph.
 * * This class is marked final to allow the compiler to devirtualize method calls 
 * for minor performance optimizations and to enforce strict architectural boundaries.
 */
class DepthFirstSearch final : public IAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Depth-First Search".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of the DFS algorithm.
     * @return std::string A brief description of how the algorithm explores nodes via backtracking.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Executes the DFS algorithm on the provided graph.
     *
     * Starting from the node specified in the context, this method plunges depth-first 
     * into the graph. It yields Memento frames at each significant step (e.g., discovering 
     * a new node, hitting a dead end, and backtracking) so the rendering engine can accurately 
     * animate the execution path.
     *
     * @param graph The mathematical graph structure to traverse.
     * @param context Configuration parameters, including the starting node ID.
     * @return AlgorithmRunResult The complete chronological history of playback frames and final metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const override;
};

}  // namespace av::algorithm

#endif