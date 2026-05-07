#ifndef ALGORITHM_VISUALIZER_ALGORITHM_BREADTH_FIRST_SEARCH_H
#define ALGORITHM_VISUALIZER_ALGORITHM_BREADTH_FIRST_SEARCH_H

#include "Algorithm/Algorithm.h"

namespace av::algorithm {

class BreadthFirstSearch final : public IAlgorithm {
  public:
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] std::string description() const override;
    [[nodiscard]] AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const override;
};

}  // namespace av::algorithm

#endif
