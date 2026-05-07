#include "Controller/InputHandler.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>

namespace av::controller {

namespace {

/**
 * @brief Validates that an algorithm is compatible with a graph.
 * 
 * Checks if the algorithm supports the current graph type and has necessary
 * prerequisites (e.g., weighted edges for weighted algorithms).
 * 
 * @param algorithm The algorithm to validate.
 * @param graph The graph to validate against.
 * @throws std::invalid_argument if the algorithm is incompatible.
 */
void ensureGraphAlgorithmCompatible(const algorithm::IAlgorithm& algorithm, const model::Graph& graph) {
    if (!algorithm.supports(graph)) {
        throw std::invalid_argument("algorithm " + algorithm.name() + " does not support the current graph");
    }

    if (algorithm.requiresWeightedGraph() && graph.edgeCount() == 0U) {
        throw std::invalid_argument("algorithm " + algorithm.name() + " requires a graph with edges");
    }
}

/**
 * @brief Parses space-separated integer values from a string.
 * 
 * Reads integers from the input string using whitespace as delimiters.
 * 
 * @param payload A string containing space-separated integers.
 * @return A vector of parsed integer values.
 */
std::vector<int> parseArrayValues(const std::string& payload) {
    std::istringstream stream(payload);
    std::vector<int> values;
    int value = 0;
    while (stream >> value) {
        values.push_back(value);
    }
    return values;
}

}  // namespace

/**
 * @brief Constructs an InputHandler with an empty undirected graph.
 * 
 * Initializes the graph, registers the renderer as an observer, and sets
 * the workspace mode to Graph.
 */
InputHandler::InputHandler() : graph_(false) {
    graph_.addObserver(&renderer_);
}

/**
 * @brief Gets mutable reference to the working graph.
 * 
 * @return Reference to the internal Graph object.
 */
model::Graph& InputHandler::graph() noexcept {
    return graph_;
}

/**
 * @brief Gets const reference to the working graph.
 * 
 * @return Const reference to the internal Graph object.
 */
const model::Graph& InputHandler::graph() const noexcept {
    return graph_;
}

/**
 * @brief Gets const reference to the graph renderer.
 * 
 * @return Const reference to the GraphRenderer.
 */
const view::GraphRenderer& InputHandler::renderer() const noexcept {
    return renderer_;
}

/**
 * @brief Gets mutable reference to the playback session.
 * 
 * @return Reference to the PlaybackSession managing algorithm frame playback.
 */
playback::PlaybackSession& InputHandler::playbackSession() noexcept {
    return playbackSession_;
}

/**
 * @brief Gets const reference to the playback session.
 * 
 * @return Const reference to the PlaybackSession.
 */
const playback::PlaybackSession& InputHandler::playbackSession() const noexcept {
    return playbackSession_;
}

/**
 * @brief Gets the current workspace mode.
 * 
 * @return The current WorkspaceMode (Graph or Sort).
 */
WorkspaceMode InputHandler::mode() const noexcept {
    return mode_;
}

/**
 * @brief Gets list of available algorithms for the current mode.
 * 
 * Returns graph algorithms if in Graph mode, sorting algorithms if in Sort mode.
 * 
 * @return Vector of algorithm names available for the current workspace mode.
 */
std::vector<std::string> InputHandler::availableAlgorithms() const {
    return mode_ == WorkspaceMode::Graph ? availableGraphAlgorithms() : availableSortingAlgorithms();
}

/**
 * @brief Gets list of available graph algorithms.
 * 
 * @return Vector of graph algorithm names.
 */
std::vector<std::string> InputHandler::availableGraphAlgorithms() const {
    return algorithmFactory_.availableAlgorithms();
}

/**
 * @brief Gets list of available sorting algorithms.
 * 
 * @return Vector of sorting algorithm names.
 */
std::vector<std::string> InputHandler::availableSortingAlgorithms() const {
    return sortingAlgorithmFactory_.availableAlgorithms();
}

/**
 * @brief Gets the currently selected algorithm for the active mode.
 * 
 * @return The name of the selected algorithm.
 */
const std::string& InputHandler::selectedAlgorithm() const noexcept {
    return mode_ == WorkspaceMode::Graph ? selectedGraphAlgorithm_ : selectedSortingAlgorithm_;
}

/**
 * @brief Gets the currently selected graph algorithm.
 * 
 * @return The name of the selected graph algorithm.
 */
const std::string& InputHandler::selectedGraphAlgorithm() const noexcept {
    return selectedGraphAlgorithm_;
}

/**
 * @brief Gets the currently selected sorting algorithm.
 * 
 * @return The name of the selected sorting algorithm.
 */
const std::string& InputHandler::selectedSortingAlgorithm() const noexcept {
    return selectedSortingAlgorithm_;
}

/**
 * @brief Gets the current array to be sorted.
 * 
 * @return Const reference to the sorting values vector.
 */
const std::vector<int>& InputHandler::sortingValues() const noexcept {
    return sortingValues_;
}

/**
 * @brief Clears the workspace and optionally creates a new graph.
 * 
 * Removes all nodes and edges, resets algorithm selection, and clears playback.
 * Sets workspace mode to Graph.
 * 
 * @param directed If true, creates a directed graph; if false, undirected.
 */
void InputHandler::resetWorkspace(const bool directed) {
    graph_ = model::Graph(directed);
    graph_.addObserver(&renderer_);
    mode_ = WorkspaceMode::Graph;
    playbackSession_.load({});
}

/**
 * @brief Populates the graph with a sample 5-node network.
 * 
 * Creates nodes A-E with edges demonstrating various weights and connections.
 * Switches workspace to Graph mode.
 */
void InputHandler::buildSampleGraph() {
    mode_ = WorkspaceMode::Graph;
    resetWorkspace(false);
    graph_.addNode("A", "A", 140.0, 180.0);
    graph_.addNode("B", "B", 340.0, 140.0);
    graph_.addNode("C", "C", 620.0, 180.0);
    graph_.addNode("D", "D", 420.0, 370.0);
    graph_.addNode("E", "E", 700.0, 360.0);
    graph_.addEdge("AB", "A", "B", 2.0);
    graph_.addEdge("AC", "A", "C", 5.0);
    graph_.addEdge("BD", "B", "D", 1.0);
    graph_.addEdge("CD", "C", "D", 2.0);
    graph_.addEdge("DE", "D", "E", 3.0);
    graph_.addEdge("BE", "B", "E", 6.0);
}

/**
 * @brief Populates the sorting array with sample data.
 * 
 * Creates an 8-element array with preset values. Switches workspace to Sort mode.
 */
void InputHandler::buildSampleArray() {
    mode_ = WorkspaceMode::Sort;
    sortingValues_ = {7, 3, 8, 2, 6, 4, 1, 5};
    playbackSession_.load({});
}

/**
 * @brief Randomly shuffles the sorting array.
 * 
 * Uses std::shuffle with a random seed. If array is empty, builds sample array first.
 */
void InputHandler::shuffleSortingValues() {
    if (sortingValues_.empty()) {
        buildSampleArray();
        return;
    }

    std::mt19937 generator(std::random_device {}());
    std::shuffle(sortingValues_.begin(), sortingValues_.end(), generator);
}

/**
 * @brief Fills the sorting array with ascending integers 1..n.
 * 
 * Resizes array to 8 if empty, then populates with values 1 through size.
 */
void InputHandler::generateSortedArray() {
    if (sortingValues_.empty()) {
        sortingValues_.resize(8);
    }

    for (std::size_t index = 0; index < sortingValues_.size(); ++index) {
        sortingValues_[index] = static_cast<int>(index + 1U);
    }
}

/**
 * @brief Fills the sorting array with descending integers n..1.
 * 
 * Resizes array to 8 if empty, then populates with values in reverse order.
 */
void InputHandler::generateReverseSortedArray() {
    if (sortingValues_.empty()) {
        sortingValues_.resize(8);
    }

    for (std::size_t index = 0; index < sortingValues_.size(); ++index) {
        sortingValues_[index] = static_cast<int>(sortingValues_.size() - index);
    }
}

/**
 * @brief Sets the sorting array to new values.
 * 
 * @param values The new array values (will be moved).
 */
void InputHandler::setSortingValues(std::vector<int> values) {
    sortingValues_ = std::move(values);
}

/**
 * @brief Switches between Graph and Sort workspace modes.
 * 
 * Clears playback history. If switching to Sort mode with empty array,
 * builds a sample array automatically.
 * 
 * @param mode The new workspace mode to activate.
 */
void InputHandler::setMode(const WorkspaceMode mode) {
    mode_ = mode;
    playbackSession_.load({});
    if (mode_ == WorkspaceMode::Sort && sortingValues_.empty()) {
        buildSampleArray();
    }
}

/**
 * @brief Selects an algorithm based on current workspace mode.
 * 
 * Dispatches to selectGraphAlgorithm or selectSortingAlgorithm accordingly.
 * 
 * @param name The algorithm name to select.
 * @throws std::invalid_argument if the algorithm does not exist.
 */
void InputHandler::selectAlgorithm(const std::string& name) {
    if (mode_ == WorkspaceMode::Graph) {
        selectGraphAlgorithm(name);
    } else {
        selectSortingAlgorithm(name);
    }
}

/**
 * @brief Selects a graph algorithm by name.
 * 
 * @param name The graph algorithm name to select.
 * @throws std::invalid_argument if the algorithm does not exist.
 */
void InputHandler::selectGraphAlgorithm(const std::string& name) {
    selectedGraphAlgorithm_ = requireAlgorithm(name)->name();
}

/**
 * @brief Selects a sorting algorithm by name.
 * 
 * @param name The sorting algorithm name to select.
 * @throws std::invalid_argument if the algorithm does not exist.
 */
void InputHandler::selectSortingAlgorithm(const std::string& name) {
    selectedSortingAlgorithm_ = requireSortingAlgorithm(name)->name();
}

/**
 * @brief Executes the currently selected algorithm.
 * 
 * Runs graph or sorting algorithm depending on mode. Loads resulting frames
 * into the playback session and returns detailed results.
 * 
 * @param context Algorithm context (start node, goal node, etc.).
 * @return AlgorithmRunResult containing frames, metrics, and output.
 * @throws std::invalid_argument if the algorithm is incompatible with current data.
 */
algorithm::AlgorithmRunResult InputHandler::runSelectedAlgorithm(const algorithm::AlgorithmContext& context) {
    algorithm::AlgorithmRunResult result;
    if (mode_ == WorkspaceMode::Graph) {
        auto algorithm = requireAlgorithm(selectedGraphAlgorithm_);
        ensureGraphAlgorithmCompatible(*algorithm, graph_);
        result = algorithm->run(graph_, context);
    } else {
        auto algorithm = requireSortingAlgorithm(selectedSortingAlgorithm_);
        result = algorithm->run(sortingValues_);
    }
    playbackSession_.load(result.frames);
    return result;
}

/**
 * @brief Compares results from two algorithms side-by-side.
 * 
 * Runs both algorithms on current graph/array and returns their results
 * for comparison analysis.
 * 
 * @param leftAlgorithm Name of the left algorithm to compare.
 * @param rightAlgorithm Name of the right algorithm to compare.
 * @param context Algorithm context (start node, goal node, etc.).
 * @return ComparisonResult containing both algorithm outputs.
 * @throws std::invalid_argument if either algorithm is invalid.
 */
ComparisonResult InputHandler::compareAlgorithms(
    const std::string& leftAlgorithm,
    const std::string& rightAlgorithm,
    const algorithm::AlgorithmContext& context) const {
    if (mode_ == WorkspaceMode::Graph) {
        const auto left = requireAlgorithm(leftAlgorithm);
        const auto right = requireAlgorithm(rightAlgorithm);
        ensureGraphAlgorithmCompatible(*left, graph_);
        ensureGraphAlgorithmCompatible(*right, graph_);
        return ComparisonResult {left->run(graph_, context), right->run(graph_, context)};
    }

    const auto left = requireSortingAlgorithm(leftAlgorithm);
    const auto right = requireSortingAlgorithm(rightAlgorithm);
    return ComparisonResult {left->run(sortingValues_), right->run(sortingValues_)};
}

/**
 * @brief Creates a quiz question about algorithm identification.
 * 
 * Generates a multiple-choice question asking the user to identify which
 * algorithm produced a given visualization.
 * 
 * @param algorithmName Name of the algorithm to quiz about.
 * @param context Algorithm context (start node, goal node, etc.).
 * @return A QuizQuestion with choices and the correct answer.
 * @throws std::invalid_argument if the algorithm is invalid or incompatible.
 */
QuizQuestion InputHandler::createQuiz(const std::string& algorithmName, const algorithm::AlgorithmContext& context) const {
    if (mode_ == WorkspaceMode::Graph) {
        const auto algorithm = requireAlgorithm(algorithmName);
        ensureGraphAlgorithmCompatible(*algorithm, graph_);
        auto choices = availableGraphAlgorithms();
        return QuizQuestion {
            "Study the visualization and identify which algorithm produced it.",
            algorithm->name(),
            std::move(choices),
            algorithm->run(graph_, context)};
    }

    const auto algorithm = requireSortingAlgorithm(algorithmName);
    auto choices = availableSortingAlgorithms();
    return QuizQuestion {
        "Study the array visualization and identify which algorithm produced it.",
        algorithm->name(),
        std::move(choices),
        algorithm->run(sortingValues_)};
}

/**
 * @brief Validates a quiz answer.
 * 
 * Checks if the user's guess matches the correct algorithm for a quiz question.
 * 
 * @param question The quiz question with the correct answer.
 * @param guess The user's guess (algorithm name).
 * @return true if the guess is correct, false otherwise.
 */
bool InputHandler::checkQuizAnswer(const QuizQuestion& question, const std::string& guess) const {
    return question.correctAlgorithm == guess;
}

/**
 * @brief Saves the current workspace state to a scenario file.
 * 
 * Serializes the graph, algorithms, and array data to a text file
 * that can be loaded later.
 * 
 * @param path File path where the scenario will be written.
 * @param scenario The workspace state to save.
 * @throws std::runtime_error if the file cannot be opened for writing.
 */
void InputHandler::saveScenario(const std::string& path, const Scenario& scenario) const {
    std::ofstream stream(path);
    if (!stream.is_open()) {
        throw std::runtime_error("unable to write scenario file: " + path);
    }

    stream << "name " << scenario.name << "\n";
    stream << "mode " << (scenario.mode == WorkspaceMode::Graph ? "graph" : "sort") << "\n";
    stream << "directed " << (scenario.graph.isDirected() ? 1 : 0) << "\n";
    stream << "algorithm " << scenario.selectedAlgorithm << "\n";
    stream << "start " << scenario.context.startNodeId << "\n";
    stream << "goal " << scenario.context.goalNodeId << "\n";
    stream << "array";
    for (const int value : scenario.values) {
        stream << " " << value;
    }
    stream << "\n";

    for (const auto& node : scenario.graph.nodes()) {
        const std::string label = node.label.empty() ? "_" : node.label;
        stream << "node " << node.id << " " << label << " " << node.x << " " << node.y << "\n";
    }

    for (const auto& edge : scenario.graph.edges()) {
        stream << "edge " << edge.id << " " << edge.from << " " << edge.to << " " << edge.weight << "\n";
    }
}

/**
 * @brief Loads a workspace state from a scenario file.
 * 
 * Deserializes graph, algorithms, and array data from a previously saved
 * scenario file.
 * 
 * @param path File path to read the scenario from.
 * @return A Scenario object with loaded workspace state.
 * @throws std::runtime_error if the file cannot be opened or contains invalid tokens.
 */
Scenario InputHandler::loadScenario(const std::string& path) const {
    std::ifstream stream(path);
    if (!stream.is_open()) {
        throw std::runtime_error("unable to open scenario file: " + path);
    }

    Scenario scenario;
    scenario.graph = model::Graph(false);
    scenario.mode = WorkspaceMode::Graph;
    std::string keyword;
    while (stream >> keyword) {
        if (keyword == "name") {
            stream >> scenario.name;
        } else if (keyword == "mode") {
            std::string modeValue;
            stream >> modeValue;
            scenario.mode = modeValue == "sort" ? WorkspaceMode::Sort : WorkspaceMode::Graph;
        } else if (keyword == "directed") {
            int directedFlag = 0;
            stream >> directedFlag;
            scenario.graph.setDirected(directedFlag != 0);
        } else if (keyword == "algorithm") {
            stream >> scenario.selectedAlgorithm;
        } else if (keyword == "start") {
            stream >> scenario.context.startNodeId;
        } else if (keyword == "goal") {
            stream >> scenario.context.goalNodeId;
        } else if (keyword == "array") {
            std::string remainder;
            std::getline(stream, remainder);
            scenario.values = parseArrayValues(remainder);
        } else if (keyword == "node") {
            model::Node node;
            stream >> node.id >> node.label >> node.x >> node.y;
            if (node.label == "_") {
                node.label.clear();
            }
            scenario.graph.addNode(node);
        } else if (keyword == "edge") {
            model::Edge edge;
            stream >> edge.id >> edge.from >> edge.to >> edge.weight;
            scenario.graph.addEdge(edge);
        } else {
            throw std::runtime_error("unsupported scenario token: " + keyword);
        }
    }

    return scenario;
}

/**
 * @brief Applies a saved scenario to the current workspace.
 * 
 * Restores the graph, algorithm selection, and array data from a scenario.
 * Updates all internal state and observers.
 * 
 * @param scenario The scenario to apply.
 */
void InputHandler::applyScenario(const Scenario& scenario) {
    graph_ = scenario.graph;
    graph_.addObserver(&renderer_);
    playbackSession_.load({});
    mode_ = scenario.mode;
    sortingValues_ = scenario.values;

    if (!scenario.selectedAlgorithm.empty()) {
        if (mode_ == WorkspaceMode::Graph) {
            selectedGraphAlgorithm_ = requireAlgorithm(scenario.selectedAlgorithm)->name();
        } else {
            selectedSortingAlgorithm_ = requireSortingAlgorithm(scenario.selectedAlgorithm)->name();
        }
    }
}

/**
 * @brief Gets a graph algorithm instance by name.
 * 
 * @param name The algorithm name to retrieve.
 * @return Shared pointer to the requested algorithm.
 * @throws std::invalid_argument if the algorithm does not exist.
 */
std::shared_ptr<const algorithm::IAlgorithm> InputHandler::requireAlgorithm(const std::string& name) const {
    return algorithmFactory_.get(name);
}

/**
 * @brief Gets a sorting algorithm instance by name.
 * 
 * @param name The algorithm name to retrieve.
 * @return Shared pointer to the requested sorting algorithm.
 * @throws std::invalid_argument if the algorithm does not exist.
 */
std::shared_ptr<const algorithm::ISortingAlgorithm> InputHandler::requireSortingAlgorithm(const std::string& name) const {
    return sortingAlgorithmFactory_.get(name);
}

}  // namespace av::controller
