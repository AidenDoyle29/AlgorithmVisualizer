#ifndef ALGORITHM_VISUALIZER_MODEL_EDGE_H
#define ALGORITHM_VISUALIZER_MODEL_EDGE_H

#include <string>

namespace av::model {

enum class EdgeState {
    Normal,
    Frontier,
    Active,
    Traversed,
    Relaxed,
    MinimumSpanningTree
};

struct Edge {
    std::string id {};
    std::string from {};
    std::string to {};
    double weight {1.0};
    bool directed {false};
    EdgeState state {EdgeState::Normal};
    std::string annotation {};

    [[nodiscard]] std::string displayLabel() const;
};

[[nodiscard]] std::string toString(EdgeState state);

}  // namespace av::model

#endif
