#ifndef ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_H

#include "Model/Graph.h"

#include <chrono>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::algorithm {

enum class VisualizationType {
    Graph,
    Array
};

enum class ArrayCellState {
    Normal,
    Active,
    Compared,
    Swapped,
    Sorted,
    Pivot
};

struct AlgorithmMetrics {
    std::size_t stepsTaken {0};
    std::size_t visitedNodes {0};
    std::size_t edgesConsidered {0};
    std::size_t relaxations {0};
    std::size_t comparisons {0};
    std::size_t swaps {0};
    std::size_t writes {0};
    std::size_t passes {0};
    std::size_t mstEdgesChosen {0};
    double totalWeight {0.0};
    long long elapsedMicroseconds {0};
};

struct PlaybackFrame {
    VisualizationType visualizationType {VisualizationType::Graph};
    model::Graph graph;
    std::vector<int> arrayValues;
    std::vector<ArrayCellState> arrayStates;
    std::string title;
    std::vector<std::string> details;
    AlgorithmMetrics metrics;
    bool terminal {false};
};

struct AlgorithmContext {
    std::string startNodeId;
    std::string goalNodeId;
};

struct AlgorithmRunResult {
    std::string algorithmName;
    std::vector<PlaybackFrame> frames;
    AlgorithmMetrics metrics;
    std::vector<std::string> traversalOrder;
    std::unordered_map<std::string, double> distances;
    std::vector<int> sortedValues;
};

class IAlgorithm {
  public:
    virtual ~IAlgorithm() = default;

    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;
    [[nodiscard]] virtual bool supports(const model::Graph& graph) const;
    [[nodiscard]] virtual bool requiresStartNode() const noexcept;
    [[nodiscard]] virtual bool requiresWeightedGraph() const noexcept;
    [[nodiscard]] virtual AlgorithmRunResult run(const model::Graph& graph, const AlgorithmContext& context) const = 0;
};

void appendFrame(
    std::vector<PlaybackFrame>& frames,
    const model::Graph& graph,
    std::string title,
    std::vector<std::string> details,
    const AlgorithmMetrics& metrics,
    bool terminal = false);

void appendArrayFrame(
    std::vector<PlaybackFrame>& frames,
    std::vector<int> values,
    std::vector<ArrayCellState> states,
    std::string title,
    std::vector<std::string> details,
    const AlgorithmMetrics& metrics,
    bool terminal = false);

[[nodiscard]] std::vector<ArrayCellState> makeArrayStates(std::size_t size, ArrayCellState state = ArrayCellState::Normal);

void finalizeMetrics(AlgorithmMetrics& metrics, const std::chrono::steady_clock::time_point& startTime);

double infinity();

}  // namespace av::algorithm

#endif
