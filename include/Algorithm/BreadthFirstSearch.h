/**
 * @file BreadthFirstSearch.h
 * @brief Defines the Breadth-First Search (BFS) algorithm strategy.
 *
 * This file implements the concrete strategy for the BFS pathfinding and 
 * traversal algorithm, conforming to the IAlgorithm interface.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_BREADTH_FIRST_SEARCH_H
#define ALGORITHM_VISUALIZER_ALGORITHM_BREADTH_FIRST_SEARCH_H

#include "Algorithm/Algorithm.h"

namespace av::algorithm {

/**
 * @class BreadthFirstSearch
 * @brief Executes a breadth-first traversal or pathfinding search on a graph.
 *
 * The BreadthFirstSearch class is a concrete implementation of the IAlgorithm 
 * interface (Strategy Pattern). It explores the graph level by level, guaranteeing 
 * the shortest path in an unweighted graph. As it runs, it generates PlaybackFrames 
 * to visualize the frontier expansion and visited nodes.
 * * This class is marked final to allow the compiler to devirtualize method calls 
 * for minor performance optimizations and to enforce architectural boundaries.
 */
class BreadthFirstSearch final : public IAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Breadth-First Search".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of the BFS algorithm.
     * @return std::string A brief description of how the algorithm explores nodes.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Executes the BFS algorithm on the provided graph.
     *
     * Starting from the node specified in the context, this method explores all 
     * neighboring nodes at the present depth prior to moving on to the nodes at 
     * the next depth level. It yields Memento frames at each significant step 
     * (e.g., visiting a node, adding to the frontier) for the rendering engine.
     *
     * @param graph The mathematical graph structure to traverse.
     * @param context Configuration parameters, including the starting node ID.
     * @return AlgorithmRunResult The complete chronological history of playback frames and final metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const override;
};

}  // namespace av::algorithm

#endif