/**
 * @file Algorithm.h
 * @brief Defines the core interfaces, data structures, and utility functions for algorithm execution and visualization.
 * * This file contains the foundation of the Strategy and Memento patterns used in the visualizer.
 * It provides the generic `IAlgorithm` interface for pathfinding and sorting algorithms, 
 * as well as the `PlaybackFrame` struct used to record snapshots of the algorithm's state over time.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_H

#include "Model/Graph.h"

#include <chrono>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::algorithm {

/**
 * @enum VisualizationType
 * @brief Dictates the rendering mode required for a specific playback frame.
 */
enum class VisualizationType {
    Graph,  /**< Renders nodes and edges for graph algorithms (e.g., BFS, Dijkstra). */
    Array   /**< Renders bars or blocks for sorting algorithms (e.g., QuickSort, MergeSort). */
};

/**
 * @enum ArrayCellState
 * @brief Represents the visual state of an individual element in an array during a sorting algorithm.
 */
enum class ArrayCellState {
    Normal,     /**< Default state, no active operations occurring on this element. */
    Active,     /**< Element is currently being evaluated or accessed. */
    Compared,   /**< Element is actively being compared against another element. */
    Swapped,    /**< Element has just been swapped to a new position. */
    Sorted,     /**< Element has reached its final sorted position. */
    Pivot       /**< Element is acting as the pivot (used in QuickSort). */
};

/**
 * @struct AlgorithmMetrics
 * @brief A comprehensive collection of statistical metrics gathered during the execution of an algorithm.
 * * These metrics are captured per-frame and continuously updated, allowing the UI 
 * to display a live dashboard of the algorithm's performance and time complexity.
 */
struct AlgorithmMetrics {
    std::size_t stepsTaken {0};             /**< Total number of discrete steps recorded. */
    std::size_t visitedNodes {0};           /**< Number of graph vertices successfully evaluated. */
    std::size_t edgesConsidered {0};        /**< Number of graph edges traversed or checked. */
    std::size_t relaxations {0};            /**< Number of successful edge relaxations (used in pathfinding). */
    std::size_t comparisons {0};            /**< Number of value comparisons made (used in sorting). */
    std::size_t swaps {0};                  /**< Number of memory swaps executed (used in sorting). */
    std::size_t writes {0};                 /**< Number of array writes/overwrites made (used in sorting). */
    std::size_t passes {0};                 /**< Number of full passes over a dataset. */
    std::size_t mstEdgesChosen {0};         /**< Number of edges added to a Minimum Spanning Tree. */
    double totalWeight {0.0};               /**< Cumulative weight of the discovered path or MST. */
    long long elapsedMicroseconds {0};      /**< Wall-clock time spent computing the algorithm. */
};

/**
 * @struct PlaybackFrame
 * @brief A snapshot of the algorithm's state at a single moment in time (Memento Pattern).
 * * This struct encapsulates all necessary data required by the View to render 
 * a specific step of the algorithm without needing to recompute any logic.
 */
struct PlaybackFrame {
    VisualizationType visualizationType {VisualizationType::Graph}; /**< The type of visualizer to use for this frame. */
    model::Graph graph;                                             /**< The state of the graph at this step. */
    std::vector<int> arrayValues;                                   /**< The state of the array at this step. */
    std::vector<ArrayCellState> arrayStates;                        /**< The visual color/status of each array element. */
    std::string title;                                              /**< A short description of the current action (e.g., "Evaluating Node A"). */
    std::vector<std::string> details;                               /**< Granular explanations of the algorithm's internal logic for this step. */
    AlgorithmMetrics metrics;                                       /**< The performance metrics recorded up to this frame. */
    bool terminal {false};                                          /**< Flag indicating if this is the final frame of the simulation. */
};

/**
 * @struct AlgorithmContext
 * @brief Configuration parameters passed into an algorithm before execution.
 */
struct AlgorithmContext {
    std::string startNodeId;    /**< The unique ID of the node to begin traversal from. */
    std::string goalNodeId;     /**< The unique ID of the target node (if applicable). */
};

/**
 * @struct AlgorithmRunResult
 * @brief The final output package generated after an algorithm completely finishes execution.
 */
struct AlgorithmRunResult {
    std::string algorithmName;                              /**< The human-readable name of the executed algorithm. */
    std::vector<PlaybackFrame> frames;                      /**< The complete chronological history of states for playback. */
    AlgorithmMetrics metrics;                               /**< The final performance metrics upon completion. */
    std::vector<std::string> traversalOrder;                /**< The sequential order in which nodes were visited. */
    std::unordered_map<std::string, double> distances;      /**< A map of shortest distances from the start node to all others. */
    std::vector<int> sortedValues;                          /**< The final sorted array (if a sorting algorithm was run). */
};

/**
 * @class IAlgorithm
 * @brief The core interface for all visualizable algorithms (Strategy Pattern).
 * * Concrete classes (e.g., BFS, Dijkstra, QuickSort) inherit from this interface. 
 * This allows the simulation manager to execute any algorithm dynamically without 
 * knowing its internal implementation details.
 */
class IAlgorithm {
  public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~IAlgorithm() = default;

    /**
     * @brief Retrieves the human-readable name of the algorithm.
     * @return std::string The algorithm's name (e.g., "Breadth-First Search").
     */
    [[nodiscard]] virtual std::string name() const = 0;

    /**
     * @brief Retrieves a short explanation of how the algorithm operates.
     * @return std::string The algorithm's description.
     */
    [[nodiscard]] virtual std::string description() const = 0;

    /**
     * @brief Determines if the algorithm is capable of processing the provided graph.
     * @param graph The graph to be evaluated.
     * @return true If the algorithm can run safely on this graph.
     * @return false If the graph contains invalid elements for this algorithm (e.g., negative weights for Dijkstra).
     */
    [[nodiscard]] virtual bool supports(const model::Graph& graph) const;

    /**
     * @brief Checks if the algorithm requires a specific starting node to function.
     * @return true If a start node must be provided in the AlgorithmContext.
     * @return false If the algorithm evaluates the whole graph regardless of start point (e.g., Kruskal's).
     */
    [[nodiscard]] virtual bool requiresStartNode() const noexcept;

    /**
     * @brief Checks if the algorithm evaluates edge weights during execution.
     * @return true If the algorithm depends on weights (e.g., Dijkstra).
     * @return false If the algorithm treats all edges uniformly (e.g., BFS).
     */
    [[nodiscard]] virtual bool requiresWeightedGraph() const noexcept;

    /**
     * @brief The core execution function. Runs the algorithm to completion.
     * * This function parses the data structure, records step-by-step history via 
     * PlaybackFrames, and returns the comprehensive result of the run.
     * * @param graph The mathematical graph to execute the algorithm against.
     * @param context The configuration arguments (start node, goal node).
     * @return AlgorithmRunResult A complete package containing the playback frames and final metrics.
     */
    [[nodiscard]] virtual AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const = 0;
};

/**
 * @brief Helper function to quickly construct and append a Graph playback frame.
 * * @param frames The vector of history frames to append to.
 * @param graph The current state of the graph.
 * @param title A short description of the current action.
 * @param details Granular explanations of the logic.
 * @param metrics The current state of the algorithm's performance metrics.
 * @param terminal Set to true if this is the final frame of the algorithm.
 */
void appendFrame(
    std::vector<PlaybackFrame>& frames,
    const model::Graph& graph,
    std::string title,
    std::vector<std::string> details,
    const AlgorithmMetrics& metrics,
    bool terminal = false);

/**
 * @brief Helper function to quickly construct and append an Array playback frame.
 * * @param frames The vector of history frames to append to.
 * @param values The current numeric values in the array.
 * @param states The visual status for each element in the array.
 * @param title A short description of the current action.
 * @param details Granular explanations of the logic.
 * @param metrics The current state of the algorithm's performance metrics.
 * @param terminal Set to true if this is the final frame of the algorithm.
 */
void appendArrayFrame(
    std::vector<PlaybackFrame>& frames,
    std::vector<int> values,
    std::vector<ArrayCellState> states,
    std::string title,
    std::vector<std::string> details,
    const AlgorithmMetrics& metrics,
    bool terminal = false);

/**
 * @brief Generates a vector of uniform array states.
 * * @param size The total number of elements in the array.
 * @param state The default state to apply to all elements (defaults to Normal).
 * @return std::vector<ArrayCellState> A vector populated with the specified state.
 */
[[nodiscard]] std::vector<ArrayCellState> makeArrayStates(std::size_t size, ArrayCellState state = ArrayCellState::Normal);

/**
 * @brief Calculates the elapsed execution time and locks the final metrics.
 * * @param metrics The metrics object to finalize.
 * @param startTime The high-resolution time point captured at the start of the algorithm.
 */
void finalizeMetrics(AlgorithmMetrics& metrics, const std::chrono::steady_clock::time_point& startTime);

/**
 * @brief Returns a conceptual representation of mathematical infinity.
 * * Typically used to initialize unvisited node distances in pathfinding algorithms like Dijkstra.
 * * @return double The maximum possible double value.
 */
double infinity();

}  // namespace av::algorithm

#endif