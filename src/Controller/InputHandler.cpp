#include "Controller/InputHandler.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>

namespace av::controller {

namespace {

void ensureGraphAlgorithmCompatible(const algorithm::IAlgorithm& algorithm, const model::Graph& graph) {
    if (!algorithm.supports(graph)) {
        throw std::invalid_argument("algorithm " + algorithm.name() + " does not support the current graph");
    }

    if (algorithm.requiresWeightedGraph() && graph.edgeCount() == 0U) {
        throw std::invalid_argument("algorithm " + algorithm.name() + " requires a graph with edges");
    }
}

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

InputHandler::InputHandler() : graph_(false) {
    graph_.addObserver(&renderer_);
}

model::Graph& InputHandler::graph() noexcept {
    return graph_;
}

const model::Graph& InputHandler::graph() const noexcept {
    return graph_;
}

const view::GraphRenderer& InputHandler::renderer() const noexcept {
    return renderer_;
}

playback::PlaybackSession& InputHandler::playbackSession() noexcept {
    return playbackSession_;
}

const playback::PlaybackSession& InputHandler::playbackSession() const noexcept {
    return playbackSession_;
}

WorkspaceMode InputHandler::mode() const noexcept {
    return mode_;
}

std::vector<std::string> InputHandler::availableAlgorithms() const {
    return mode_ == WorkspaceMode::Graph ? availableGraphAlgorithms() : availableSortingAlgorithms();
}

std::vector<std::string> InputHandler::availableGraphAlgorithms() const {
    return algorithmFactory_.availableAlgorithms();
}

std::vector<std::string> InputHandler::availableSortingAlgorithms() const {
    return sortingAlgorithmFactory_.availableAlgorithms();
}

const std::string& InputHandler::selectedAlgorithm() const noexcept {
    return mode_ == WorkspaceMode::Graph ? selectedGraphAlgorithm_ : selectedSortingAlgorithm_;
}

const std::string& InputHandler::selectedGraphAlgorithm() const noexcept {
    return selectedGraphAlgorithm_;
}

const std::string& InputHandler::selectedSortingAlgorithm() const noexcept {
    return selectedSortingAlgorithm_;
}

const std::vector<int>& InputHandler::sortingValues() const noexcept {
    return sortingValues_;
}

void InputHandler::resetWorkspace(const bool directed) {
    graph_ = model::Graph(directed);
    graph_.addObserver(&renderer_);
    mode_ = WorkspaceMode::Graph;
    playbackSession_.load({});
}

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

void InputHandler::buildSampleArray() {
    mode_ = WorkspaceMode::Sort;
    sortingValues_ = {7, 3, 8, 2, 6, 4, 1, 5};
    playbackSession_.load({});
}

void InputHandler::shuffleSortingValues() {
    if (sortingValues_.empty()) {
        buildSampleArray();
        return;
    }

    std::mt19937 generator(std::random_device {}());
    std::shuffle(sortingValues_.begin(), sortingValues_.end(), generator);
}

void InputHandler::generateSortedArray() {
    if (sortingValues_.empty()) {
        sortingValues_.resize(8);
    }

    for (std::size_t index = 0; index < sortingValues_.size(); ++index) {
        sortingValues_[index] = static_cast<int>(index + 1U);
    }
}

void InputHandler::generateReverseSortedArray() {
    if (sortingValues_.empty()) {
        sortingValues_.resize(8);
    }

    for (std::size_t index = 0; index < sortingValues_.size(); ++index) {
        sortingValues_[index] = static_cast<int>(sortingValues_.size() - index);
    }
}

void InputHandler::setSortingValues(std::vector<int> values) {
    sortingValues_ = std::move(values);
}

void InputHandler::setMode(const WorkspaceMode mode) {
    mode_ = mode;
    playbackSession_.load({});
    if (mode_ == WorkspaceMode::Sort && sortingValues_.empty()) {
        buildSampleArray();
    }
}

void InputHandler::selectAlgorithm(const std::string& name) {
    if (mode_ == WorkspaceMode::Graph) {
        selectGraphAlgorithm(name);
    } else {
        selectSortingAlgorithm(name);
    }
}

void InputHandler::selectGraphAlgorithm(const std::string& name) {
    selectedGraphAlgorithm_ = requireAlgorithm(name)->name();
}

void InputHandler::selectSortingAlgorithm(const std::string& name) {
    selectedSortingAlgorithm_ = requireSortingAlgorithm(name)->name();
}

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

bool InputHandler::checkQuizAnswer(const QuizQuestion& question, const std::string& guess) const {
    return question.correctAlgorithm == guess;
}

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

std::shared_ptr<const algorithm::IAlgorithm> InputHandler::requireAlgorithm(const std::string& name) const {
    return algorithmFactory_.get(name);
}

std::shared_ptr<const algorithm::ISortingAlgorithm> InputHandler::requireSortingAlgorithm(const std::string& name) const {
    return sortingAlgorithmFactory_.get(name);
}

}  // namespace av::controller
