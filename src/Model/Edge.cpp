#include "Model/Edge.h"

#include <sstream>

namespace av::model {

/**
 * @brief Gets a formatted display label for this edge.
 * 
 * Returns a string representation showing the edge connection and weight.
 * Format: "from -> to (weight)" for directed, "from -- to (weight)" for undirected.
 * Used for rendering edges in the UI.
 * 
 * @return The formatted edge label string.
 */
std::string Edge::displayLabel() const {
    std::ostringstream stream;
    stream << from << (directed ? " -> " : " -- ") << to << " (" << weight << ")";
    return stream.str();
}

/**
 * @brief Converts an EdgeState enum to its string representation.
 * 
 * Maps edge state enumerations (Normal, Active, Traversed, etc.) to human-readable
 * strings for debugging, logging, and display purposes.
 * 
 * @param state The EdgeState enum value to convert.
 * @return A lowercase string representation of the state (e.g., "traversed", "mst").
 */
std::string toString(const EdgeState state) {
    switch (state) {
        case EdgeState::Normal:
            return "normal";
        case EdgeState::Frontier:
            return "frontier";
        case EdgeState::Active:
            return "active";
        case EdgeState::Traversed:
            return "traversed";
        case EdgeState::Relaxed:
            return "relaxed";
        case EdgeState::MinimumSpanningTree:
            return "mst";
    }

    return "unknown";
}

}  // namespace av::model
