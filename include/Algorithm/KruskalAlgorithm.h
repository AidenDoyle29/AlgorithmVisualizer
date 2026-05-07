#ifndef ALGORITHM_VISUALIZER_ALGORITHM_KRUSKAL_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_KRUSKAL_ALGORITHM_H

#include "Algorithm/Algorithm.h"

namespace av::algorithm {

class KruskalAlgorithm final : public IAlgorithm {
  public:
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] std::string description() const override;
    [[nodiscard]] bool requiresStartNode() const noexcept override;
    [[nodiscard]] bool supports(const model::Graph& graph) const override;
    [[nodiscard]] bool requiresWeightedGraph() const noexcept override;
    [[nodiscard]] AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const override;
};

}  // namespace av::algorithm

#endif
