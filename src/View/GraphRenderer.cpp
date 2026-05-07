#include "View/GraphRenderer.h"

#include <iomanip>
#include <sstream>

namespace av::view {

namespace {

std::string renderMetrics(const algorithm::AlgorithmMetrics& metrics) {
    std::ostringstream stream;
    stream << "steps=" << metrics.stepsTaken << ", visited=" << metrics.visitedNodes
           << ", edges=" << metrics.edgesConsidered << ", relaxations=" << metrics.relaxations
           << ", comparisons=" << metrics.comparisons << ", swaps=" << metrics.swaps << ", writes=" << metrics.writes
           << ", passes=" << metrics.passes << ", mst_edges=" << metrics.mstEdgesChosen
           << ", total_weight=" << metrics.totalWeight << ", elapsed_us=" << metrics.elapsedMicroseconds;
    return stream.str();
}

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

void GraphRenderer::onGraphChanged(const model::GraphEvent& event) {
    lastRenderedOutput_ = "Graph event: " + event.reason + "\n" + renderGraph(event.graph);
}

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

const std::string& GraphRenderer::lastRenderedOutput() const noexcept {
    return lastRenderedOutput_;
}

}  // namespace av::view
