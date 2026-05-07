/**
 * @file KruskalAlgorithm.h
 * @brief Defines the Kruskal's Algorithm strategy for Minimum Spanning Tree generation.
 *
 * This file implements the concrete strategy for Kruskal's algorithm, 
 * conforming to the IAlgorithm interface. It is designed to find the 
 * lowest-cost Minimum Spanning Tree (MST) for a connected, weighted graph.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_KRUSKAL_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_KRUSKAL_ALGORITHM_H

#include "Algorithm/Algorithm.h"

namespace av::algorithm {

/**
 * @class KruskalAlgorithm
 * @brief Executes Kruskal's Minimum Spanning Tree algorithm on a weighted graph.
 *
 * The KruskalAlgorithm class is a concrete implementation of the IAlgorithm 
 * interface (Strategy Pattern). It operates by sorting all edges by weight and 
 * progressively adding the smallest edges to the MST, using a Disjoint Set 
 * (Union-Find) data structure to prevent cycles.
 * * As it runs, it generates PlaybackFrames tailored for MST visualization, 
 * highlighting global edge sorting, cycle evaluations, and final tree connections.
 * * This class is marked final to allow the compiler to devirtualize method calls 
 * for minor performance optimizations and enforce strict architectural boundaries.
 */
class KruskalAlgorithm final : public IAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Kruskal's Algorithm".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of Kruskal's algorithm.
     * @return std::string A brief description of the global edge-sorting and cycle-detection process.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Specifies that this algorithm does NOT require a starting node.
     * * Unlike pathfinding algorithms (BFS, Dijkstra), Kruskal's evaluates the 
     * entire graph globally to build a spanning tree. The Controller should use 
     * this flag to disable the "Select Start Node" UI requirement.
     * * @return false Always returns false for this specific algorithm.
     */
    [[nodiscard]] bool requiresStartNode() const noexcept override;

    /**
     * @brief Determines if the algorithm is capable of processing the provided graph.
     * * Checks structural requirements, such as ensuring the graph is connected 
     * and undirected, which are prerequisites for a valid MST.
     * * @param graph The graph to be evaluated.
     * @return true If the graph is valid for Kruskal's algorithm.
     */
    [[nodiscard]] bool supports(const model::Graph& graph) const override;

    /**
     * @brief Specifies that this algorithm strictly requires a weighted graph.
     * * Edge weights are required to determine the minimum cost spanning tree.
     * * @return true Always returns true for this specific algorithm.
     */
    [[nodiscard]] bool requiresWeightedGraph() const noexcept override;

    /**
     * @brief Executes Kruskal's algorithm on the provided graph.
     *
     * This method sorts all graph edges and utilizes a Union-Find structure to 
     * safely build the MST. It yields Memento frames at each significant step 
     * (e.g., evaluating an edge, detecting a cycle, merging sets) so the rendering 
     * engine can accurately animate the global tree construction.
     *
     * @param graph The mathematical graph structure to evaluate.
     * @param context Configuration parameters (start node is ignored here).
     * @return AlgorithmRunResult The complete chronological history of playback frames and final metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const override;
};

}  // namespace av::algorithm

#endif