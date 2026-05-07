#include "Algorithm/Algorithm.h"

namespace av::algorithm {

/**
 * @brief Checks if the algorithm supports the given graph structure.
 * 
 * By default, this returns true. Derived algorithms can override this to
 * restrict execution based on graph properties (e.g., directed vs. undirected).
 * 
 * @param graph The graph model to validate.
 * @return true if the algorithm can be executed on the graph, false otherwise.
 */
bool IAlgorithm::supports(const model::Graph& /* graph */) const {
    return true;
}

/**
 * @brief Indicates whether the algorithm needs a specific starting node to begin execution.
 * 
 * @return true if a source node is required (e.g., Dijkstra's, BFS), false otherwise.
 */
bool IAlgorithm::requiresStartNode() const noexcept {
    return true;
}

/**
 * @brief Indicates whether the algorithm requires edge weights to function correctly.
 * 
 * @return true if weights are necessary, false if the algorithm works on unweighted graphs.
 */
bool IAlgorithm::requiresWeightedGraph() const noexcept {
    return false;
}

/**
 * @brief Records a new frame of a graph-based visualization for playback.
 * 
 * Captures the current state of the graph and associated metrics to be rendered 
 * by the visualizer's playback engine.
 * 
 * @param frames The collection of frames to which the new frame is appended.
 * @param graph The current state of the graph.
 * @param title A short descriptive string for the current step.
 * @param details Detailed log messages or step information.
 * @param metrics The current performance or progress metrics.
 * @param terminal Set to true if this represents the final state of the algorithm.
 */
void appendFrame(
    std::vector<PlaybackFrame>& frames,
    const model::Graph& graph,
    std::string title,
    std::vector<std::string> details,
    const AlgorithmMetrics& metrics,
    const bool terminal) {
    frames.push_back(
        PlaybackFrame {VisualizationType::Graph, graph, {}, {}, std::move(title), std::move(details), metrics, terminal});
}

/**
 * @brief Records a new frame of an array-based visualization (e.g., sorting).
 * 
 * @param frames The collection of frames to which the new frame is appended.
 * @param values The current elements in the array.
 * @param states Visual states (colors/highlights) for each array cell.
 * @param title A short descriptive string for the current step.
 * @param details Detailed log messages or step information.
 * @param metrics The current performance or progress metrics.
 * @param terminal Set to true if this represents the final state of the algorithm.
 */
void appendArrayFrame(
    std::vector<PlaybackFrame>& frames,
    std::vector<int> values,
    std::vector<ArrayCellState> states,
    std::string title,
    std::vector<std::string> details,
    const AlgorithmMetrics& metrics,
    const bool terminal) {
    frames.push_back(PlaybackFrame {
        VisualizationType::Array,
        model::Graph(false),
        std::move(values),
        std::move(states),
        std::move(title),
        std::move(details),
        metrics,
        terminal});
}

/**
 * @brief Generates a vector of uniform states for an array of a specific size.
 * 
 * Often used to initialize or reset the visual state of an array (e.g., setting 
 * all cells to 'Default' or 'Sorted').
 * 
 * @param size The number of elements in the array.
 * @param state The state to apply to every element.
 * @return A vector of ArrayCellState objects.
 */
std::vector<ArrayCellState> makeArrayStates(const std::size_t size, const ArrayCellState state) {
    return std::vector<ArrayCellState>(size, state);
}

/**
 * @brief Calculates and records the final execution time in the metrics object.
 * 
 * @param metrics The metrics structure to update.
 * @param startTime The time point recorded at the start of the algorithm.
 */
void finalizeMetrics(AlgorithmMetrics& metrics, const std::chrono::steady_clock::time_point& startTime) {
    metrics.elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(
                                      std::chrono::steady_clock::now() - startTime)
                                      .count();
}

/**
 * @brief Provides a standard representation of infinity for distance calculations.
 * 
 * Typically used for initializing distances in shortest-path algorithms.
 * 
 * @return The double representation of positive infinity.
 */
double infinity() {
    return std::numeric_limits<double>::infinity();
}

}  // namespace av::algorithm