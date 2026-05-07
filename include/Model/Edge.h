/**
 * @file Edge.h
 * @brief Defines the foundational Edge entity and its visual states for the mathematical graph model.
 *
 * This file implements the core Edge structure used to connect Nodes within the 
 * Graph model. It also defines the specific states an edge can inhabit during 
 * algorithm execution, bridging the gap between mathematical representation 
 * and UI visualization.
 */

#ifndef ALGORITHM_VISUALIZER_MODEL_EDGE_H
#define ALGORITHM_VISUALIZER_MODEL_EDGE_H

#include <string>

namespace av::model {

/**
 * @enum EdgeState
 * @brief Represents the current logical and visual state of an edge during algorithm execution.
 *
 * These states are captured within PlaybackFrames (Memento pattern) to instruct 
 * the View layer on how to color or animate specific edges over time.
 */
enum class EdgeState {
    Normal,               /**< The default, unvisited state of the edge. */
    Frontier,             /**< The edge is adjacent to a visited node and is ready for evaluation. */
    Active,               /**< The edge is currently being evaluated or traversed by the algorithm. */
    Traversed,            /**< The edge has been successfully crossed and is part of the traversed path. */
    Relaxed,              /**< The edge's weight was successfully used to find a shorter path (Used in Dijkstra's). */
    MinimumSpanningTree   /**< The edge has been permanently added to the MST (Used in Kruskal's). */
};

/**
 * @struct Edge
 * @brief Represents a mathematical connection between two nodes in a graph.
 *
 * The Edge struct acts as a primary data container for the Model layer. It holds 
 * both the mathematical properties required for pathfinding (weight, directionality) 
 * and the visual properties required for rendering (state, annotation).
 */
struct Edge {
    std::string id {};                            /**< The unique identifier for this specific edge. */
    std::string from {};                          /**< The unique ID of the source Node. */
    std::string to {};                            /**< The unique ID of the destination Node. */
    double weight {1.0};                          /**< The traversal cost of the edge (Used in weighted algorithms). */
    bool directed {false};                        /**< True if the edge is one-way from source to destination. */
    EdgeState state {EdgeState::Normal};          /**< The current visual status of the edge. */
    std::string annotation {};                    /**< Optional text to display alongside the edge during playback. */

    /**
     * @brief Formats a human-readable label for the edge to be rendered by the UI.
     * @return std::string The formatted label (typically displaying the weight and/or annotation).
     */
    [[nodiscard]] std::string displayLabel() const;
};

/**
 * @brief Translates an EdgeState enum into a human-readable string identifier.
 *
 * This utility function is primarily used by the rendering engine or logging 
 * system to convert the strict enum value into a UI-friendly string format.
 *
 * @param state The current visual state of the edge.
 * @return std::string A textual representation of the state.
 */
[[nodiscard]] std::string toString(EdgeState state);

}  // namespace av::model

#endif