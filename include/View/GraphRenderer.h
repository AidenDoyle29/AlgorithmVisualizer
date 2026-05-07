/**
 * @file GraphRenderer.h
 * @brief Defines the View layer responsible for translating graph and algorithm state into visual output.
 *
 * This file implements the GraphRenderer, which acts as the visual engine for the 
 * application. It listens for underlying data mutations via the Observer pattern 
 * and provides translation methods to convert mathematical models and historical 
 * playback frames into renderable strings or draw commands.
 */

#ifndef ALGORITHM_VISUALIZER_VIEW_GRAPH_RENDERER_H
#define ALGORITHM_VISUALIZER_VIEW_GRAPH_RENDERER_H

#include "Algorithm/Algorithm.h"
#include "Model/Graph.h"

#include <string>

namespace av::view {

/**
 * @struct ComparisonView
 * @brief A payload containing formatted text or draw commands for a side-by-side algorithm comparison.
 *
 * This struct is utilized by the UI to populate split-screen panels, allowing 
 * users to evaluate the visual differences and performance summaries between 
 * two competing algorithms executed on the same data.
 */
struct ComparisonView {
    std::string left;       /**< The formatted rendering output for the first algorithm. */
    std::string right;      /**< The formatted rendering output for the second algorithm. */
    std::string summary;    /**< A formatted textual comparison of their performance metrics. */
};

/**
 * @class GraphRenderer
 * @brief The core rendering engine for Graph-based workspaces.
 *
 * The GraphRenderer is a concrete implementation of the GraphObserver interface. 
 * It reacts passively to Model updates, ensuring the UI remains perfectly synced 
 * with the underlying data. It also processes algorithm PlaybackFrames (Mementos) 
 * to draw step-by-step visualizations. This class is marked final to enforce 
 * strict architectural boundaries in the View layer.
 */
class GraphRenderer final : public model::GraphObserver {
  public:
    /**
     * @brief Triggered automatically whenever the observed Graph mutates.
     *
     * This method catches the GraphEvent broadcasted by the Model layer and 
     * caches or redraws the new state, ensuring the View is never out of sync.
     *
     * @param event The payload containing the updated graph state and the reason for the mutation.
     */
    void onGraphChanged(const model::GraphEvent& event) override;

    /**
     * @brief Translates a static, mathematical Graph into a renderable output format.
     * @param graph The current state of the mathematical model.
     * @return std::string The formatted string or draw-command payload representing the graph.
     */
    [[nodiscard]] std::string renderGraph(const model::Graph& graph) const;

    /**
     * @brief Translates a single historical Memento frame into a renderable output format.
     *
     * Used exclusively during active algorithm playback to draw specific algorithmic 
     * states (e.g., frontier edges, active nodes, or array states).
     *
     * @param frame The immutable snapshot of the algorithm at a specific moment in time.
     * @return std::string The formatted string or draw-command payload representing the frame.
     */
    [[nodiscard]] std::string renderFrame(const algorithm::PlaybackFrame& frame) const;

    /**
     * @brief Generates a structured layout payload for side-by-side algorithm evaluation.
     *
     * @param left The complete execution result of the first algorithm.
     * @param right The complete execution result of the second algorithm.
     * @return ComparisonView A structured package containing the separated render outputs and a metric summary.
     */
    [[nodiscard]] ComparisonView renderComparison(
        const algorithm::AlgorithmRunResult& left,
        const algorithm::AlgorithmRunResult& right) const;

    /**
     * @brief Retrieves the most recently generated render payload.
     * @return const std::string& An immutable reference to the cached render output.
     */
    [[nodiscard]] const std::string& lastRenderedOutput() const noexcept;

  private:
    std::string lastRenderedOutput_ {}; /**< Internal cache of the latest successful rendering operation. */
};

}  // namespace av::view

#endif