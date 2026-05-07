#ifndef ALGORITHM_VISUALIZER_MODEL_NODE_H
#define ALGORITHM_VISUALIZER_MODEL_NODE_H

#include <string>

namespace av::model {

enum class NodeState {
    Normal,
    Start,
    Frontier,
    Active,
    Visited,
    Path,
    MinimumSpanningTree
};

struct Node {
    std::string id {};
    std::string label {};
    double x {0.0};
    double y {0.0};
    NodeState state {NodeState::Normal};
    std::string annotation {};

    [[nodiscard]] std::string displayLabel() const;
};

[[nodiscard]] std::string toString(NodeState state);

}  // namespace av::model

#endif
