#include "Model/Edge.h"

#include <sstream>

namespace av::model {

std::string Edge::displayLabel() const {
    std::ostringstream stream;
    stream << from << (directed ? " -> " : " -- ") << to << " (" << weight << ")";
    return stream.str();
}

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
