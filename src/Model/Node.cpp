#include "Model/Node.h"

namespace av::model {

/**
 * @brief Gets the display label for this node.
 * 
 * Returns the custom label if one is set, otherwise returns the node ID.
 * Used for rendering nodes in the UI with meaningful identifiers.
 * 
 * @return The display label string (custom label or node ID).
 */
std::string Node::displayLabel() const {
    return label.empty() ? id : label;
}

/**
 * @brief Converts a NodeState enum to its string representation.
 * 
 * Maps node state enumerations (Normal, Start, Visited, etc.) to human-readable
 * strings for debugging, logging, and display purposes.
 * 
 * @param state The NodeState enum value to convert.
 * @return A lowercase string representation of the state (e.g., "visited", "start").
 */
std::string toString(const NodeState state) {
    switch (state) {
        case NodeState::Normal:
            return "normal";
        case NodeState::Start:
            return "start";
        case NodeState::Frontier:
            return "frontier";
        case NodeState::Active:
            return "active";
        case NodeState::Visited:
            return "visited";
        case NodeState::Path:
            return "path";
        case NodeState::MinimumSpanningTree:
            return "mst";
    }

    return "unknown";
}

}  // namespace av::model
