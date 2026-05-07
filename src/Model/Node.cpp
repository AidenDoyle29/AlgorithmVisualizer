#include "Model/Node.h"

namespace av::model {

std::string Node::displayLabel() const {
    return label.empty() ? id : label;
}

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
