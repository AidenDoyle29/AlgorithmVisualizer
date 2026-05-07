/**
 * @file Node.h
 * @brief Defines the foundational Node entity and its visual states for the mathematical graph model.
 *
 * This file implements the core Node structure (vertex) used within the Graph model. 
 * It encapsulates both the spatial coordinates for rendering and the specific 
 * algorithmic states required to visualize pathfinding and traversal over time.
 */

#ifndef ALGORITHM_VISUALIZER_MODEL_NODE_H
#define ALGORITHM_VISUALIZER_MODEL_NODE_H

#include <string>

namespace av::model {

/**
 * @enum NodeState
 * @brief Represents the current logical and visual state of a node during algorithm execution.
 *
 * These states are captured within PlaybackFrames (Memento pattern) to instruct 
 * the View layer on how to color, highlight, or animate specific vertices during playback.
 */
enum class NodeState {
    Normal,               /**< The default, undiscovered state of the node. */
    Start,                /**< The designated origin point for a pathfinding algorithm. */
    Frontier,             /**< The node is in the queue/stack waiting to be evaluated. */
    Active,               /**< The node is currently being processed or expanded by the algorithm. */
    Visited,              /**< The node has been fully evaluated and removed from the frontier. */
    Path,                 /**< The node is part of the final successfully calculated route. */
    MinimumSpanningTree   /**< The node is part of the finalized MST (Used in Kruskal's). */
};

/**
 * @struct Node
 * @brief Represents a single vertex in the mathematical graph.
 *
 * The Node struct acts as a primary data container for the Model layer. It holds 
 * the unique identifiers required by the graph hash maps, the 2D spatial coordinates 
 * for UI layout, and the dynamic visual properties updated by running algorithms.
 */
struct Node {
    std::string id {};                            /**< The unique, immutable identifier for this specific node. */
    std::string label {};                         /**< The human-readable name of the node (defaults to ID if empty). */
    double x {0.0};                               /**< The horizontal spatial coordinate for UI rendering. */
    double y {0.0};                               /**< The vertical spatial coordinate for UI rendering. */
    NodeState state {NodeState::Normal};          /**< The current visual status of the node. */
    std::string annotation {};                    /**< Optional text to display alongside the node (e.g., Dijkstra distance values). */

    /**
     * @brief Formats the human-readable label for the node to be rendered by the UI.
     * @return std::string The formatted label, prioritizing the 'label' field and falling back to 'id'.
     */
    [[nodiscard]] std::string displayLabel() const;
};

/**
 * @brief Translates a NodeState enum into a human-readable string identifier.
 *
 * This utility function is primarily used by the rendering engine or logging 
 * system to convert the strict enum value into a UI-friendly string format.
 *
 * @param state The current visual state of the node.
 * @return std::string A textual representation of the state.
 */
[[nodiscard]] std::string toString(NodeState state);

}  // namespace av::model

#endif