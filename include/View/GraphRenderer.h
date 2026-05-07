#ifndef ALGORITHM_VISUALIZER_VIEW_GRAPH_RENDERER_H
#define ALGORITHM_VISUALIZER_VIEW_GRAPH_RENDERER_H

#include "Algorithm/Algorithm.h"
#include "Model/Graph.h"

#include <string>

namespace av::view {

struct ComparisonView {
    std::string left;
    std::string right;
    std::string summary;
};

class GraphRenderer final : public model::GraphObserver {
  public:
    void onGraphChanged(const model::GraphEvent& event) override;

    [[nodiscard]] std::string renderGraph(const model::Graph& graph) const;
    [[nodiscard]] std::string renderFrame(const algorithm::PlaybackFrame& frame) const;
    [[nodiscard]] ComparisonView renderComparison(
        const algorithm::AlgorithmRunResult& left,
        const algorithm::AlgorithmRunResult& right) const;
    [[nodiscard]] const std::string& lastRenderedOutput() const noexcept;

  private:
    std::string lastRenderedOutput_ {};
};

}  // namespace av::view

#endif
