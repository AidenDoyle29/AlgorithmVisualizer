/**
 * @file DijkstraAlgorithm.h
 * @brief Defines the Dijkstra's Algorithm strategy for shortest path calculation.
 *
 * This file implements the concrete strategy for Dijkstra's pathfinding 
 * algorithm, conforming to the IAlgorithm interface. It is designed to 
 * find the shortest path between nodes in a graph with non-negative edge weights.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_DIJKSTRA_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_DIJKSTRA_ALGORITHM_H

#include "Algorithm/Algorithm.h"

namespace av::algorithm {

/**
 * @class DijkstraAlgorithm
 * @brief Executes Dijkstra's shortest path search on a weighted graph.
 *
 * The DijkstraAlgorithm class is a concrete implementation of the IAlgorithm 
 * interface (Strategy Pattern). It utilizes a priority queue to continuously 
 * explore the lowest-cost outward edges, guaranteeing the shortest path from 
 * the start node to all other reachable nodes. 
 * * As it runs, it generates PlaybackFrames to visualize the frontier expansion, 
 * edge evaluations, and distance relaxations.
 * * This class is marked final to allow the compiler to devirtualize method calls 
 * for minor performance optimizations and to enforce architectural boundaries.
 */
class DijkstraAlgorithm final : public IAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Dijkstra's Algorithm".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of Dijkstra's algorithm.
     * @return std::string A brief description of the lowest-cost edge evaluation process.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Specifies that this algorithm strictly requires a weighted graph.
     * * Unlike BFS or DFS, Dijkstra's algorithm depends on edge weights to calculate 
     * the shortest path. The Controller should use this flag to validate the graph 
     * state or warn the user before execution.
     * * @return true Always returns true for this specific algorithm.
     */
    [[nodiscard]] bool requiresWeightedGraph() const noexcept override;

    /**
     * @brief Executes Dijkstra's algorithm on the provided graph.
     *
     * Starting from the node specified in the context, this method evaluates paths 
     * and relaxes edge weights. It yields Memento frames at each significant step 
     * (e.g., pulling a node from the priority queue, updating a neighbor's distance) 
     * so the rendering engine can accurately animate the cost calculations.
     *
     * @param graph The mathematical graph structure to traverse.
     * @param context Configuration parameters, including the starting node ID.
     * @return AlgorithmRunResult The complete chronological history of playback frames, final distances, and metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const override;
};

}  // namespace av::algorithm

#endif