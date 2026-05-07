#include "View/GraphRenderer.h"

#include <iomanip>
#include <sstream>

namespace av::view {

namespace {

/**
 * @brief Converts algorithm metrics into a formatted string representation.
 * 
 * Combines all tracked metrics (steps, visited nodes, comparisons, swaps, etc.)
 * into a single comma-separated string suitable for display purposes.
 * 
 * @param metrics The AlgorithmMetrics object containing performance data.
 * @return A formatted string with all metrics in key=value format.
 */
std::string renderMetrics(const algorithm::AlgorithmMetrics& metrics) {
    std::ostringstream stream;
    stream << "steps=" << metrics.stepsTaken << ", visited=" << metrics.visitedNodes
           << ", edges=" << metrics.edgesConsidered << ", relaxations=" << metrics.relaxations
           << ", comparisons=" << metrics.comparisons << ", swaps=" << metrics.swaps << ", writes=" << metrics.writes
           << ", passes=" << metrics.passes << ", mst_edges=" << metrics.mstEdgesChosen
           << ", total_weight=" << metrics.totalWeight << ", elapsed_us=" << metrics.elapsedMicroseconds;
    return stream.str();
}

/**
 * @brief Converts an array cell state enum to its string representation.
 * 
 * Maps visual state enumerations (Normal, Active, Compared, etc.) to human-readable
 * strings for debugging and display purposes.
 * 
 * @param state The ArrayCellState enum value to convert.
 * @return A lowercase string representation of the state (e.g., "compared", "sorted").
 */
std::string renderArrayState(const algorithm::ArrayCellState state) {
    switch (state) {
        case algorithm::ArrayCellState::Normal:
            return "normal";
        case algorithm::ArrayCellState::Active:
            return "current";
        case algorithm::ArrayCellState::Compared:
            return "compared";
        case algorithm::ArrayCellState::Swapped:
            return "swapped";
        case algorithm::ArrayCellState::Sorted:
            return "sorted";
        case algorithm::ArrayCellState::Pivot:
            return "pivot";
    }

    return "normal";
}

}  // namespace

/**
 * @brief Handles graph change events and stores the rendered output.
 * 
 * Called when the underlying graph model changes. Updates the last rendered output
 * with the event reason and current graph state for display or debugging.
 * 
 * @param event The GraphEvent containing the reason and updated graph state.
 */
void GraphRenderer::onGraphChanged(const model::GraphEvent& event) {
    lastRenderedOutput_ = "Graph event: " + event.reason + "\n" + renderGraph(event.graph);
}

/**
 * @brief Renders a graph structure into a formatted text representation.
 * 
 * Creates a human-readable string showing all nodes and edges in the graph,
 * including their states, annotations, and positions. Indicates whether the
 * graph is directed or undirected.
 * 
 * @param graph The Graph object to render.
 * @return A formatted string representation of the graph structure.
 */
std::string GraphRenderer::renderGraph(const model::Graph& graph) const {
    std::ostringstream stream;
    stream << "Graph (" << (graph.isDirected() ? "directed" : "undirected") << ")\n";
    stream << "Nodes:\n";
    for (const auto& node : graph.nodes()) {
        stream << "  - " << node.id << " [" << model::toString(node.state) << "]";
        if (!node.annotation.empty()) {
            stream << " {" << node.annotation << "}";
        }
        stream << " @(" << node.x << ", " << node.y << ")\n";
    }

    stream << "Edges:\n";
    for (const auto& edge : graph.edges()) {
        stream << "  - " << edge.id << ": " << edge.from << (edge.directed ? " -> " : " -- ") << edge.to
               << " w=" << edge.weight << " [" << model::toString(edge.state) << "]";
        if (!edge.annotation.empty()) {
            stream << " {" << edge.annotation << "}";
        }
        stream << "\n";
    }
    return stream.str();
}

/**
 * @brief Renders a playback frame into a formatted text representation.
 * 
 * Converts a single frame from algorithm playback into human-readable output.
 * Handles both array visualization (showing element values and states) and
 * graph visualization (showing node and edge states). Includes frame title,
 * details, metrics, and completion status.
 * 
 * @param frame The PlaybackFrame to render.
 * @return A formatted string representation of the frame contents.
 */
std::string GraphRenderer::renderFrame(const algorithm::PlaybackFrame& frame) const {
    if (frame.visualizationType == algorithm::VisualizationType::Array) {
        std::ostringstream stream;
        stream << "=== " << frame.title << " ===\n";
        for (const auto& detail : frame.details) {
            stream << "* " << detail << "\n";
        }
        stream << "Array:\n";
        for (std::size_t index = 0; index < frame.arrayValues.size(); ++index) {
            stream << "  - [" << index << "] " << frame.arrayValues[index];
            if (index < frame.arrayStates.size()) {
                stream << " [" << renderArrayState(frame.arrayStates[index]) << "]";
            }
            stream << "\n";
        }
        stream << "Metrics: " << renderMetrics(frame.metrics) << "\n";
        if (frame.terminal) {
            stream << "Status: complete\n";
        }
        return stream.str();
    }

    std::ostringstream stream;
    stream << "=== " << frame.title << " ===\n";
    for (const auto& detail : frame.details) {
        stream << "* " << detail << "\n";
    }
    stream << renderGraph(frame.graph);
    stream << "Metrics: " << renderMetrics(frame.metrics) << "\n";
    if (frame.terminal) {
        stream << "Status: complete\n";
    }
    return stream.str();
}

/**
 * @brief Creates a side-by-side comparison view of two algorithm runs.
 * 
 * Generates a ComparisonView structure containing the final frames from both
 * algorithm executions along with a summary comparing their metrics and performance.
 * Useful for analyzing differences in algorithm behavior and efficiency.
 * 
 * @param left The AlgorithmRunResult from the left algorithm to compare.
 * @param right The AlgorithmRunResult from the right algorithm to compare.
 * @return A ComparisonView containing rendered frames and summary statistics.
 */
ComparisonView GraphRenderer::renderComparison(
    const algorithm::AlgorithmRunResult& left,
    const algorithm::AlgorithmRunResult& right) const {
    std::ostringstream summary;
    summary << "Comparison: " << left.algorithmName << " vs " << right.algorithmName << "\n";
    summary << "  Left frames: " << left.frames.size() << ", Right frames: " << right.frames.size() << "\n";
    summary << "  Left metrics: " << renderMetrics(left.metrics) << "\n";
    summary << "  Right metrics: " << renderMetrics(right.metrics) << "\n";

    return ComparisonView {
        left.frames.empty() ? "No frames" : renderFrame(left.frames.back()),
        right.frames.empty() ? "No frames" : renderFrame(right.frames.back()),
        summary.str()};
}

/**
 * @brief Retrieves the last rendered output string.
 * 
 * Returns a const reference to the most recent rendered output from any
 * render operation. Useful for debugging or retrieving intermediate results.
 * 
 * @return A const reference to the last rendered output string.
 */
const std::string& GraphRenderer::lastRenderedOutput() const noexcept {
    return lastRenderedOutput_;
}

}  // namespace av::view
