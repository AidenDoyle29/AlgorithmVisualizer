#include "Controller/InputHandler.h"
#include "View/UITheme.h"
#include "View/SFMLRenderer.h"
#include "View/Sidebar.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

using av::algorithm::AlgorithmContext;
using av::algorithm::AlgorithmMetrics;
using av::algorithm::AlgorithmRunResult;
using av::algorithm::ArrayCellState;
using av::algorithm::PlaybackFrame;
using av::controller::InputHandler;
using av::controller::Scenario;
using av::controller::WorkspaceMode;
using av::model::Edge;
using av::model::EdgeState;
using av::model::Graph;
using av::model::Node;
using av::model::NodeState;
using av::playback::PlaybackSession;
using av::view::theme::nodeColor;
using av::view::theme::edgeColor;
using av::view::theme::arrayColor;
using av::view::theme::withAlpha;

enum class InteractionMode {
    Select,
    AddNode,
    AddEdge,
    Delete
};

struct CanvasSpec {
    sf::FloatRect rect;
    std::string title;
};

bool contains(const sf::FloatRect& rect, const sf::Vector2f point) {
    return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x && point.y >= rect.position.y &&
           point.y <= rect.position.y + rect.size.y;
}


std::string metricsSummary(const AlgorithmMetrics& metrics) {
    std::ostringstream stream;
    stream << "Steps: " << metrics.stepsTaken << "\n";
    stream << "Visited nodes: " << metrics.visitedNodes << "\n";
    stream << "Edges considered: " << metrics.edgesConsidered << "\n";
    stream << "Relaxations: " << metrics.relaxations << "\n";
    stream << "Comparisons: " << metrics.comparisons << "\n";
    stream << "Swaps: " << metrics.swaps << "\n";
    stream << "Writes: " << metrics.writes << "\n";
    stream << "Passes: " << metrics.passes << "\n";
    stream << "MST edges: " << metrics.mstEdgesChosen << "\n";
    stream << "Total weight: " << metrics.totalWeight << "\n";
    stream << "Elapsed us: " << metrics.elapsedMicroseconds;
    return stream.str();
}

std::string speedLabel(const float stepSeconds) {
    std::ostringstream stream;
    stream.setf(std::ios::fixed);
    stream.precision(2);
    stream << "Speed: " << stepSeconds << "s/step";
    return stream.str();
}

std::string sanitizeScenarioName(std::string_view rawName) {
    std::string output;
    output.reserve(rawName.size());

    for (const char character : rawName) {
        if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
            (character >= '0' && character <= '9')) {
            output.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(character))));
        } else if (character == ' ' || character == '-' || character == '_') {
            output.push_back('_');
        }
    }

    while (!output.empty() && output.front() == '_') {
        output.erase(output.begin());
    }

    while (!output.empty() && output.back() == '_') {
        output.pop_back();
    }

    return output;
}

std::string alphaId(std::size_t index) {
    std::string result;

    do {
        const std::size_t remainder = index % 26;
        result.insert(result.begin(), static_cast<char>('A' + static_cast<int>(remainder)));
        if (index < 26) {
            break;
        }
        index = (index / 26) - 1;
    } while (true);

    return result;
}

float pointSegmentDistance(const sf::Vector2f point, const sf::Vector2f start, const sf::Vector2f end) {
    const sf::Vector2f delta = end - start;
    const float lengthSquared = delta.x * delta.x + delta.y * delta.y;
    if (lengthSquared <= 0.0001F) {
        const float dx = point.x - start.x;
        const float dy = point.y - start.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    const float projection =
        std::clamp(((point.x - start.x) * delta.x + (point.y - start.y) * delta.y) / lengthSquared, 0.0F, 1.0F);
    const sf::Vector2f closest {start.x + delta.x * projection, start.y + delta.y * projection};
    const float dx = point.x - closest.x;
    const float dy = point.y - closest.y;
    return std::sqrt(dx * dx + dy * dy);
}

class SimulationApp {
  public:
    SimulationApp()
        : window_(
              sf::VideoMode({1440U, 920U}),
              "Algorithm Visualizer",
              sf::Style::Default,
              sf::State::Windowed),
              renderer_(window_) {
        window_.setFramerateLimit(60);
        handler_.buildSampleGraph();
        startNodeId_ = "A";
        goalNodeId_ = "E";
        statusMessage_ = "Loaded sample graph. Click nodes to select them, then run an algorithm.";
    }

    int run() {
        while (window_.isOpen()) {
            processEvents();
            advancePlaybackIfNeeded();
            render();
        }

        return 0;
    }

  private:
    sf::RenderWindow window_;
    av::view::SFMLRenderer renderer_;
    av::view::Sidebar sidebar_;
    InputHandler handler_;
    PlaybackSession primarySession_ {};
    PlaybackSession secondarySession_ {};
    AlgorithmRunResult primaryRun_ {};
    AlgorithmRunResult secondaryRun_ {};
    sf::Clock playbackClock_ {};
    float playbackStepSeconds_ {0.75F};
    bool autoPlay_ {false};
    bool comparisonEnabled_ {false};
    bool hasPrimaryRun_ {false};
    bool hasComparisonRun_ {false};
    
    bool draggingNode_ {false};
    std::string draggedNodeId_ {};
    InteractionMode interactionMode_ {InteractionMode::Select};
    std::string primaryAlgorithm_ {"BFS"};
    std::string secondaryAlgorithm_ {"DFS"};
    std::string arrayInput_ {"7, 3, 8, 2, 6, 4, 1, 5"};
    std::string selectedNodeId_ {};
    std::string selectedEdgeId_ {};
    std::string pendingEdgeStartId_ {};
    std::string startNodeId_ {};
    std::string goalNodeId_ {};
    std::optional<av::controller::QuizQuestion> quizQuestion_ {};
    std::string quizFeedback_ {};
    std::string statusMessage_ {};
    const std::filesystem::path scenarioDirectory_ = std::filesystem::current_path() / "saved";
    std::string scenarioNameInput_ {"my_graph"};
    std::string selectedScenarioName_ {};
    bool editingScenarioName_ {false};
    bool editingArrayInput_ {false};

    [[nodiscard]] sf::Vector2f currentMousePosition() const {
        return sf::Vector2f(sf::Mouse::getPosition(window_));
    }

    [[nodiscard]] bool quizActive() const noexcept {
        return quizQuestion_.has_value();
    }

    [[nodiscard]] bool sortingMode() const noexcept {
        return handler_.mode() == WorkspaceMode::Sort;
    }

    [[nodiscard]] std::string primaryDisplayTitle() const {
        return quizActive() ? "Quiz Challenge" : primaryAlgorithm_;
    }

    [[nodiscard]] std::string secondaryDisplayTitle() const {
        return secondaryAlgorithm_;
    }

    [[nodiscard]] std::vector<int> currentArrayValues() const {
        if (hasPrimaryRun_ && !primarySession_.empty() && currentPrimaryFrame()->visualizationType == av::algorithm::VisualizationType::Array) {
            return currentPrimaryFrame()->arrayValues;
        }

        return handler_.sortingValues();
    }

    [[nodiscard]] std::vector<ArrayCellState> currentArrayStates() const {
        if (hasPrimaryRun_ && !primarySession_.empty() && currentPrimaryFrame()->visualizationType == av::algorithm::VisualizationType::Array) {
            return currentPrimaryFrame()->arrayStates;
        }

        return std::vector<ArrayCellState>(handler_.sortingValues().size(), ArrayCellState::Normal);
    }

    [[nodiscard]] std::string primaryFrameLabel(const PlaybackFrame& frame) const {
        if (quizActive()) {
            return "Quiz frame: hidden algorithm step";
        }

        return primaryAlgorithm_ + " frame: " + frame.title;
    }

    void processEvents() {
        while (const std::optional event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window_.close();
                continue;
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                window_.setView(sf::View(sf::FloatRect({0.0F, 0.0F}, sf::Vector2f(resized->size))));
                continue;
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyPress(*key);
                continue;
            }

            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                handleTextEntered(*textEntered);
                continue;
            }

            if (const auto* scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
                handleSidebarScroll(sf::Vector2f(scrolled->position), scrolled->delta);
                continue;
            }

            if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (pressed->button == sf::Mouse::Button::Left) {
                    handleLeftMousePressed(sf::Vector2f(pressed->position));
                }
                continue;
            }

            if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (released->button == sf::Mouse::Button::Left) {
                    draggingNode_ = false;
                    draggedNodeId_.clear();
                }
                continue;
            }

            if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
                handleMouseMoved(sf::Vector2f(moved->position));
                continue;
            }
        }
    }

    void handleKeyPress(const sf::Event::KeyPressed& key) {
        if (editingScenarioName_) {
            if (key.code == sf::Keyboard::Key::Enter) {
                editingScenarioName_ = false;
                statusMessage_ = "Scenario name set to " + scenarioNameInput_ + ".";
                return;
            }

            if (key.code == sf::Keyboard::Key::Escape) {
                editingScenarioName_ = false;
                return;
            }

            if (key.code == sf::Keyboard::Key::Backspace) {
                if (!scenarioNameInput_.empty()) {
                    scenarioNameInput_.pop_back();
                }
                return;
            }
        }

        if (editingArrayInput_) {
            if (key.code == sf::Keyboard::Key::Enter) {
                applyArrayInput();
                editingArrayInput_ = false;
                return;
            }

            if (key.code == sf::Keyboard::Key::Escape) {
                editingArrayInput_ = false;
                return;
            }

            if (key.code == sf::Keyboard::Key::Backspace) {
                if (!arrayInput_.empty()) {
                    arrayInput_.pop_back();
                }
                return;
            }
        }

        if (key.code == sf::Keyboard::Key::Space) {
            if (!hasPrimaryRun_) {
                runVisualization();
            } else {
                togglePlayback();
            }
        } else if (key.code == sf::Keyboard::Key::Right) {
            stepForward();
        } else if (key.code == sf::Keyboard::Key::Left) {
            stepBackward();
        } else if (key.code == sf::Keyboard::Key::Delete || key.code == sf::Keyboard::Key::Backspace) {
            deleteSelection();
        } else if (key.code == sf::Keyboard::Key::Num1) {
            primaryAlgorithm_ = sortingMode() ? "Bubble" : "BFS";
        } else if (key.code == sf::Keyboard::Key::Num2) {
            primaryAlgorithm_ = sortingMode() ? "Selection" : "DFS";
        } else if (key.code == sf::Keyboard::Key::Num3) {
            primaryAlgorithm_ = sortingMode() ? "Insertion" : "Dijkstra";
        } else if (key.code == sf::Keyboard::Key::Num4) {
            primaryAlgorithm_ = sortingMode() ? "Merge" : "Kruskal";
        } else if (key.code == sf::Keyboard::Key::Num5 && sortingMode()) {
            primaryAlgorithm_ = "Quick";
        } else if (key.code == sf::Keyboard::Key::S && key.control) {
            saveScenario();
        } else if (key.code == sf::Keyboard::Key::L && key.control) {
            loadScenario();
        }
    }

    void handleTextEntered(const sf::Event::TextEntered& event) {
        if (event.unicode < 32 || event.unicode > 126) {
            return;
        }

        const char character = static_cast<char>(event.unicode);
        if (editingScenarioName_) {
            if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
                (character >= '0' && character <= '9') || character == ' ' || character == '-' || character == '_') {
                scenarioNameInput_.push_back(character);
            }
            return;
        }

        if (editingArrayInput_) {
            if ((character >= '0' && character <= '9') || character == '-' || character == ',' || character == ' ' ||
                character == ';') {
                arrayInput_.push_back(character);
            }
        }
    }

    void handleLeftMousePressed(const sf::Vector2f position) {
        if (handleSidebarClick(position)) {
            return;
        }

        if (sortingMode()) {
            return;
        }

        if (!contains(mainCanvasRect(), position)) {
            return;
        }

        if (comparisonEnabled_ && hasComparisonRun_) {
            return;
        }

        Graph& graph = handler_.graph();
        const auto clickedNode = findNodeAt(position, mainCanvasRect(), graph);
        const auto clickedEdge = findEdgeAt(position, mainCanvasRect(), graph);

        switch (interactionMode_) {
            case InteractionMode::Select:
                if (clickedNode.has_value()) {
                    selectedNodeId_ = *clickedNode;
                    selectedEdgeId_.clear();
                    draggingNode_ = true;
                    draggedNodeId_ = *clickedNode;
                } else if (clickedEdge.has_value()) {
                    selectedEdgeId_ = *clickedEdge;
                    selectedNodeId_.clear();
                } else {
                    selectedNodeId_.clear();
                    selectedEdgeId_.clear();
                }
                break;
            case InteractionMode::AddNode:
                addNodeAt(position);
                break;
            case InteractionMode::AddEdge:
                if (!clickedNode.has_value()) {
                    pendingEdgeStartId_.clear();
                    statusMessage_ = "Click two nodes to create an edge.";
                    return;
                }
                if (pendingEdgeStartId_.empty()) {
                    pendingEdgeStartId_ = *clickedNode;
                    selectedNodeId_ = *clickedNode;
                    statusMessage_ = "Select the second endpoint for the new edge.";
                } else if (pendingEdgeStartId_ != *clickedNode) {
                    addEdgeBetween(pendingEdgeStartId_, *clickedNode);
                    pendingEdgeStartId_.clear();
                }
                break;
            case InteractionMode::Delete:
                if (clickedNode.has_value()) {
                    deleteNode(*clickedNode);
                } else if (clickedEdge.has_value()) {
                    deleteEdge(*clickedEdge);
                }
                break;
        }
    }

    void handleMouseMoved(const sf::Vector2f position) {
        if (sortingMode()) {
            return;
        }

        if (!draggingNode_ || draggedNodeId_.empty()) {
            return;
        }

        Graph& graph = handler_.graph();
        if (!graph.hasNode(draggedNodeId_)) {
            draggingNode_ = false;
            draggedNodeId_.clear();
            return;
        }

        const sf::Vector2f world = screenToWorld(position, mainCanvasRect());
        graph.node(draggedNodeId_).x = std::clamp(world.x, 40.0F, av::view::theme::WorkspaceWidth - 40.0F);
        graph.node(draggedNodeId_).y = std::clamp(world.y, 40.0F, av::view::theme::WorkspaceHeight - 40.0F);
        invalidateRuns();
    }

    void handleSidebarScroll(const sf::Vector2f position, const float delta) {
        if (!contains(sidebarRect(), position)) {
            return;
        }
        sidebar_.handleScroll(delta, handler_, primaryAlgorithm_, secondaryAlgorithm_, sf::Vector2f(window_.getSize()));
    }

    bool handleSidebarClick(const sf::Vector2f position) {
        std::string clickedId = sidebar_.handleMouseClick(position, handler_, primaryAlgorithm_, secondaryAlgorithm_, sf::Vector2f(window_.getSize()));
        if (clickedId.empty()) {
            return false;
        }
        handleButton(clickedId);
        return true;
    }

    void handleButton(const std::string& id) {
        if (id == "mode_graph") {
            handler_.setMode(WorkspaceMode::Graph);
            interactionMode_ = InteractionMode::Select;
            primaryAlgorithm_ = "BFS";
            secondaryAlgorithm_ = "DFS";
            selectedNodeId_.clear();
            selectedEdgeId_.clear();
            pendingEdgeStartId_.clear();
            invalidateRuns();
            statusMessage_ = "Graph mode enabled.";
        } else if (id == "mode_sort") {
            handler_.setMode(WorkspaceMode::Sort);
            interactionMode_ = InteractionMode::Select;
            if (handler_.sortingValues().empty()) {
                handler_.buildSampleArray();
            }
            primaryAlgorithm_ = "Bubble";
            secondaryAlgorithm_ = "Selection";
            selectedNodeId_.clear();
            selectedEdgeId_.clear();
            pendingEdgeStartId_.clear();
            invalidateRuns();
            syncArrayInputFromValues();
            statusMessage_ = "Sorting mode enabled.";
        } else if (id == "array_input_field") {
            editingArrayInput_ = true;
            statusMessage_ = "Type a comma-separated list of integers, then press Enter.";
        } else if (id == "array_apply") {
            applyArrayInput();
        } else if (id == "array_shuffle") {
            handler_.shuffleSortingValues();
            syncArrayInputFromValues();
            invalidateRuns();
            statusMessage_ = "Array shuffled.";
        } else if (id == "array_sample") {
            handler_.buildSampleArray();
            primaryAlgorithm_ = "Bubble";
            secondaryAlgorithm_ = "Selection";
            syncArrayInputFromValues();
            invalidateRuns();
            statusMessage_ = "Loaded the sample array.";
        } else if (id == "array_sorted") {
            handler_.generateSortedArray();
            syncArrayInputFromValues();
            invalidateRuns();
            statusMessage_ = "Loaded a sorted array preset.";
        } else if (id == "array_reverse") {
            handler_.generateReverseSortedArray();
            syncArrayInputFromValues();
            invalidateRuns();
            statusMessage_ = "Loaded a reverse-sorted array preset.";
        } else if (id == "array_size_minus") {
            resizeSortingArray(-1);
        } else if (id == "array_size_plus") {
            resizeSortingArray(1);
        } else if (id == "array_randomize") {
            handler_.shuffleSortingValues();
            syncArrayInputFromValues();
            invalidateRuns();
            statusMessage_ = "Array randomized.";
        } else if (id == "load_sample") {
            handler_.buildSampleGraph();
            startNodeId_ = "A";
            goalNodeId_ = "E";
            selectedNodeId_.clear();
            selectedEdgeId_.clear();
            pendingEdgeStartId_.clear();
            invalidateRuns();
            statusMessage_ = "Loaded the sample graph.";
        } else if (id == "mode_select") {
            interactionMode_ = InteractionMode::Select;
            statusMessage_ = "Selection mode enabled.";
        } else if (id == "mode_add_node") {
            interactionMode_ = InteractionMode::AddNode;
            statusMessage_ = "Click on the canvas to add a node.";
        } else if (id == "mode_add_edge") {
            interactionMode_ = InteractionMode::AddEdge;
            pendingEdgeStartId_.clear();
            statusMessage_ = "Click two nodes to add an edge.";
        } else if (id == "mode_delete") {
            interactionMode_ = InteractionMode::Delete;
            statusMessage_ = "Click a node or edge to delete it.";
        } else if (id == "clear_graph") {
            handler_.resetWorkspace(false);
            startNodeId_.clear();
            goalNodeId_.clear();
            selectedNodeId_.clear();
            selectedEdgeId_.clear();
            pendingEdgeStartId_.clear();
            invalidateRuns();
            statusMessage_ = "Workspace cleared.";
        } else if (id == "toggle_directed") {
            handler_.graph().setDirected(!handler_.graph().isDirected());
            invalidateRuns();
            statusMessage_ = handler_.graph().isDirected() ? "Graph is now directed." : "Graph is now undirected.";
        } else if (id == "set_start") {
            if (selectedNodeId_.empty()) {
                statusMessage_ = "Select a node first, then set it as the start node.";
            } else {
                startNodeId_ = selectedNodeId_;
                statusMessage_ = "Start node set to " + startNodeId_ + ".";
            }
        } else if (id == "set_goal") {
            if (selectedNodeId_.empty()) {
                statusMessage_ = "Select a node first, then set it as the goal node.";
            } else {
                goalNodeId_ = selectedNodeId_;
                statusMessage_ = "Goal node set to " + goalNodeId_ + ".";
            }
        } else if (id == "weight_down") {
            changeSelectedEdgeWeight(-1.0);
        } else if (id == "weight_up") {
            changeSelectedEdgeWeight(1.0);
        } else if (id == "delete_selected") {
            deleteSelection();
        } else if (id == "algorithm_bfs") {
            primaryAlgorithm_ = "BFS";
        } else if (id == "algorithm_dfs") {
            primaryAlgorithm_ = "DFS";
        } else if (id == "algorithm_dijkstra") {
            primaryAlgorithm_ = "Dijkstra";
        } else if (id == "algorithm_kruskal") {
            primaryAlgorithm_ = "Kruskal";
        } else if (id == "algorithm_bubble") {
            primaryAlgorithm_ = "Bubble";
        } else if (id == "algorithm_selection") {
            primaryAlgorithm_ = "Selection";
        } else if (id == "algorithm_insertion") {
            primaryAlgorithm_ = "Insertion";
        } else if (id == "algorithm_merge") {
            primaryAlgorithm_ = "Merge";
        } else if (id == "algorithm_quick") {
            primaryAlgorithm_ = "Quick";
        } else if (id == "compare_toggle") {
            comparisonEnabled_ = !comparisonEnabled_;
            invalidateRuns();
            statusMessage_ = comparisonEnabled_ ? "Comparison mode enabled." : "Comparison mode disabled.";
        } else if (id == "secondary_bfs") {
            secondaryAlgorithm_ = "BFS";
        } else if (id == "secondary_dfs") {
            secondaryAlgorithm_ = "DFS";
        } else if (id == "secondary_dijkstra") {
            secondaryAlgorithm_ = "Dijkstra";
        } else if (id == "secondary_kruskal") {
            secondaryAlgorithm_ = "Kruskal";
        } else if (id == "secondary_bubble") {
            secondaryAlgorithm_ = "Bubble";
        } else if (id == "secondary_selection") {
            secondaryAlgorithm_ = "Selection";
        } else if (id == "secondary_insertion") {
            secondaryAlgorithm_ = "Insertion";
        } else if (id == "secondary_merge") {
            secondaryAlgorithm_ = "Merge";
        } else if (id == "secondary_quick") {
            secondaryAlgorithm_ = "Quick";
        } else if (id == "run") {
            runVisualization();
        } else if (id == "play_pause") {
            togglePlayback();
        } else if (id == "step_back") {
            stepBackward();
        } else if (id == "step_forward") {
            stepForward();
        } else if (id == "speed_slower") {
            playbackStepSeconds_ = std::min(3.0F, playbackStepSeconds_ + 0.25F);
            statusMessage_ = speedLabel(playbackStepSeconds_);
        } else if (id == "speed_faster") {
            playbackStepSeconds_ = std::max(0.10F, playbackStepSeconds_ - 0.25F);
            statusMessage_ = speedLabel(playbackStepSeconds_);
        } else if (id == "reset") {
            resetPlayback();
        } else if (id == "save") {
            saveNamedScenario();
        } else if (id == "load") {
            loadSelectedScenario();
        } else if (id == "scenario_name_field") {
            editingScenarioName_ = true;
            statusMessage_ = "Type a scenario name, then press Enter.";
        } else if (id == "scenario_overwrite") {
            overwriteSelectedScenario();
        } else if (id.rfind("scenario_select:", 0) == 0) {
            selectedScenarioName_ = id.substr(std::string("scenario_select:").size());
            scenarioNameInput_ = selectedScenarioName_;
            editingScenarioName_ = false;
            statusMessage_ = "Selected saved scenario " + selectedScenarioName_ + ".";
        } else if (id == "quiz") {
            startQuiz();
        } else if (id.rfind("quiz_choice:", 0) == 0 && quizQuestion_.has_value()) {
            const std::string guess = id.substr(std::string("quiz_choice:").size());
            const bool correct = handler_.checkQuizAnswer(*quizQuestion_, guess);
            quizFeedback_ = correct ? "Correct: " + guess : "Incorrect. Correct answer: " + quizQuestion_->correctAlgorithm;
            statusMessage_ = quizFeedback_;
        }
    }

    std::vector<std::filesystem::path> savedScenarioFiles() const {
        std::vector<std::filesystem::path> files;
        if (!std::filesystem::exists(scenarioDirectory_)) {
            return files;
        }

        for (const auto& entry : std::filesystem::directory_iterator(scenarioDirectory_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".scn") {
                files.push_back(entry.path());
            }
        }

        std::sort(files.begin(), files.end(), [](const std::filesystem::path& left, const std::filesystem::path& right) {
            return left.stem().string() < right.stem().string();
        });
        return files;
    }

    sf::FloatRect sidebarRect() const {
        const sf::Vector2f size(window_.getSize());
        return {{size.x - av::view::theme::SidebarWidth, 0.0F}, {av::view::theme::SidebarWidth, size.y}};
    }

    sf::FloatRect mainCanvasRect() const {
        const sf::Vector2f size(window_.getSize());
        return {{20.0F, 84.0F}, {size.x - av::view::theme::SidebarWidth - 40.0F, size.y - 120.0F}};
    }

    std::pair<CanvasSpec, CanvasSpec> comparisonCanvases() const {
        const sf::FloatRect base = mainCanvasRect();
        const float gap = 16.0F;
        const float width = (base.size.x - gap) / 2.0F;

        return {
            CanvasSpec {{{base.position.x, base.position.y}, {width, base.size.y}}, primaryAlgorithm_},
            CanvasSpec {{{base.position.x + width + gap, base.position.y}, {width, base.size.y}}, secondaryAlgorithm_}};
    }

    sf::Vector2f screenToWorld(const sf::Vector2f point, const sf::FloatRect& canvas) const {
        return {
            std::clamp(
                (point.x - canvas.position.x - av::view::theme::CanvasPadding) / (canvas.size.x - 2.0F * av::view::theme::CanvasPadding) * av::view::theme::WorkspaceWidth,
                0.0F,
                av::view::theme::WorkspaceWidth),
            std::clamp(
                (point.y - canvas.position.y - av::view::theme::CanvasPadding) / (canvas.size.y - 2.0F * av::view::theme::CanvasPadding) * av::view::theme::WorkspaceHeight,
                0.0F,
                av::view::theme::WorkspaceHeight)};
    }

    std::optional<std::string> findNodeAt(const sf::Vector2f screen, const sf::FloatRect& canvas, const Graph& graph) const {
        for (const auto& node : graph.nodes()) {
            const sf::Vector2f position = renderer_.worldToScreen({static_cast<float>(node.x), static_cast<float>(node.y)}, canvas);
            const float dx = screen.x - position.x;
            const float dy = screen.y - position.y;
            if (std::sqrt(dx * dx + dy * dy) <= av::view::theme::NodeRadius + 4.0F) {
                return node.id;
            }
        }

        return std::nullopt;
    }

    std::optional<std::string> findEdgeAt(const sf::Vector2f screen, const sf::FloatRect& canvas, const Graph& graph) const {
        std::optional<std::string> bestEdge;
        float bestDistance = 18.0F;

        for (const auto& edge : graph.edges()) {
            const Node& from = graph.node(edge.from);
            const Node& to = graph.node(edge.to);
            const sf::Vector2f start = renderer_.worldToScreen({static_cast<float>(from.x), static_cast<float>(from.y)}, canvas);
            const sf::Vector2f end = renderer_.worldToScreen({static_cast<float>(to.x), static_cast<float>(to.y)}, canvas);
            const float distance = pointSegmentDistance(screen, start, end);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestEdge = edge.id;
            }
        }

        return bestEdge;
    }

    void addNodeAt(const sf::Vector2f position) {
        Graph& graph = handler_.graph();
        const sf::Vector2f world = screenToWorld(position, mainCanvasRect());

        std::string id;
        for (std::size_t index = graph.nodeCount();; ++index) {
            id = alphaId(index);
            if (!graph.hasNode(id)) {
                break;
            }
        }

        graph.addNode(id, id, world.x, world.y);
        selectedNodeId_ = id;
        invalidateRuns();
        statusMessage_ = "Added node " + id + ".";
    }

    void addEdgeBetween(const std::string& from, const std::string& to) {
        Graph& graph = handler_.graph();
        std::string edgeId;
        for (std::size_t index = graph.edgeCount() + 1;; ++index) {
            edgeId = "e" + std::to_string(index);
            if (!graph.hasEdge(edgeId)) {
                break;
            }
        }

        graph.addEdge(edgeId, from, to, 1.0);
        selectedEdgeId_ = edgeId;
        selectedNodeId_.clear();
        invalidateRuns();
        statusMessage_ = "Added edge " + edgeId + " between " + from + " and " + to + ".";
    }

    void deleteNode(const std::string& nodeId) {
        handler_.graph().removeNode(nodeId);
        if (startNodeId_ == nodeId) {
            startNodeId_.clear();
        }
        if (goalNodeId_ == nodeId) {
            goalNodeId_.clear();
        }
        selectedNodeId_.clear();
        selectedEdgeId_.clear();
        invalidateRuns();
        statusMessage_ = "Deleted node " + nodeId + ".";
    }

    void deleteEdge(const std::string& edgeId) {
        handler_.graph().removeEdge(edgeId);
        selectedEdgeId_.clear();
        invalidateRuns();
        statusMessage_ = "Deleted edge " + edgeId + ".";
    }

    void deleteSelection() {
        if (!selectedNodeId_.empty() && handler_.graph().hasNode(selectedNodeId_)) {
            deleteNode(selectedNodeId_);
            return;
        }

        if (!selectedEdgeId_.empty() && handler_.graph().hasEdge(selectedEdgeId_)) {
            deleteEdge(selectedEdgeId_);
            return;
        }

        statusMessage_ = "Nothing is selected.";
    }

    void syncArrayInputFromValues() {
        std::ostringstream stream;
        const auto& values = handler_.sortingValues();
        for (std::size_t index = 0; index < values.size(); ++index) {
            if (index > 0U) {
                stream << ", ";
            }
            stream << values[index];
        }
        arrayInput_ = stream.str();
    }

    void applyArrayInput() {
        std::vector<int> values;
        std::string token;
        std::istringstream stream(arrayInput_);
        while (std::getline(stream, token, ',')) {
            std::istringstream tokenStream(token);
            int value = 0;
            while (tokenStream >> value) {
                values.push_back(value);
            }
        }

        if (values.empty()) {
            statusMessage_ = "Enter at least one integer.";
            return;
        }

        handler_.setSortingValues(std::move(values));
        invalidateRuns();
        statusMessage_ = "Array updated.";
    }

    void resizeSortingArray(const int delta) {
        std::vector<int> values = handler_.sortingValues();
        if (values.empty()) {
            values = {1, 2, 3, 4, 5, 6, 7, 8};
        }

        if (delta > 0) {
            values.push_back(static_cast<int>(values.size() + 1U));
        } else if (delta < 0 && values.size() > 1U) {
            values.pop_back();
        }

        handler_.setSortingValues(std::move(values));
        syncArrayInputFromValues();
        invalidateRuns();
        statusMessage_ = "Array size updated.";
    }

    void changeSelectedEdgeWeight(const double delta) {
        if (selectedEdgeId_.empty() || !handler_.graph().hasEdge(selectedEdgeId_)) {
            statusMessage_ = "Select an edge first.";
            return;
        }

        Edge& edge = handler_.graph().edge(selectedEdgeId_);
        edge.weight = std::max(0.0, edge.weight + delta);
        invalidateRuns();

        std::ostringstream stream;
        stream << "Edge " << selectedEdgeId_ << " weight is now " << edge.weight << ".";
        statusMessage_ = stream.str();
    }

    void invalidateRuns() {
        autoPlay_ = false;
        hasPrimaryRun_ = false;
        hasComparisonRun_ = false;
        primarySession_.load({});
        secondarySession_.load({});
        quizQuestion_.reset();
        quizFeedback_.clear();
    }

    bool algorithmNeedsStart(const std::string& name) const {
        return !sortingMode() && name != "Kruskal";
    }

    AlgorithmContext resolvedContextFor(const std::string& algorithmName) {
        if (sortingMode()) {
            return {};
        }

        AlgorithmContext context;
        context.startNodeId = startNodeId_;
        context.goalNodeId = goalNodeId_;

        if (algorithmNeedsStart(algorithmName)) {
            if (context.startNodeId.empty()) {
                if (handler_.graph().empty()) {
                    throw std::runtime_error("Create at least one node before running the algorithm.");
                }

                context.startNodeId = handler_.graph().nodes().front().id;
                startNodeId_ = context.startNodeId;
                statusMessage_ = "No start node was set. Using " + startNodeId_ + ".";
            }
        } else {
            context.startNodeId.clear();
        }

        if (!context.goalNodeId.empty() && !handler_.graph().hasNode(context.goalNodeId)) {
            context.goalNodeId.clear();
        }

        return context;
    }

    void runVisualization() {
        try {
            if (quizActive() && hasPrimaryRun_) {
                autoPlay_ = true;
                playbackClock_.restart();
                statusMessage_ = "Playing quiz visualization. Watch the behavior, then choose a guess.";
                return;
            }

            quizQuestion_.reset();
            quizFeedback_.clear();

            if (comparisonEnabled_) {
                if (sortingMode()) {
                    if (handler_.sortingValues().empty()) {
                        throw std::runtime_error("Create an array before running a visualization.");
                    }
                    const auto comparison = handler_.compareAlgorithms(primaryAlgorithm_, secondaryAlgorithm_, {});
                    primaryRun_ = comparison.left;
                    secondaryRun_ = comparison.right;
                } else {
                    if (handler_.graph().empty()) {
                        throw std::runtime_error("Create a graph before running a visualization.");
                    }
                    const AlgorithmContext primaryContext = resolvedContextFor(primaryAlgorithm_);
                    const auto comparison = handler_.compareAlgorithms(primaryAlgorithm_, secondaryAlgorithm_, primaryContext);
                    primaryRun_ = comparison.left;
                    secondaryRun_ = comparison.right;
                }
                primarySession_.load(primaryRun_.frames);
                secondarySession_.load(secondaryRun_.frames);
                hasPrimaryRun_ = true;
                hasComparisonRun_ = true;
                autoPlay_ = true;
                playbackClock_.restart();
                statusMessage_ = "Running " + primaryAlgorithm_ + " and " + secondaryAlgorithm_ + " side by side.";
            } else {
                if (sortingMode()) {
                    if (handler_.sortingValues().empty()) {
                        throw std::runtime_error("Create an array before running a visualization.");
                    }
                    handler_.selectAlgorithm(primaryAlgorithm_);
                    primaryRun_ = handler_.runSelectedAlgorithm({});
                } else {
                    if (handler_.graph().empty()) {
                        throw std::runtime_error("Create a graph before running a visualization.");
                    }
                    handler_.selectAlgorithm(primaryAlgorithm_);
                    primaryRun_ = handler_.runSelectedAlgorithm(resolvedContextFor(primaryAlgorithm_));
                }
                primarySession_.load(primaryRun_.frames);
                secondarySession_.load({});
                hasPrimaryRun_ = true;
                hasComparisonRun_ = false;
                autoPlay_ = true;
                playbackClock_.restart();
                statusMessage_ = "Running " + primaryAlgorithm_ + ".";
            }
        } catch (const std::exception& error) {
            autoPlay_ = false;
            statusMessage_ = error.what();
        }
    }

    void togglePlayback() {
        if (!hasPrimaryRun_) {
            runVisualization();
            return;
        }

        autoPlay_ = !autoPlay_;
        playbackClock_.restart();
        statusMessage_ = autoPlay_ ? "Playback resumed." : "Playback paused.";
    }

    void stepForward() {
        if (!hasPrimaryRun_) {
            runVisualization();
            autoPlay_ = false;
            return;
        }

        autoPlay_ = false;
        const bool advancedPrimary = primarySession_.stepForward();
        bool advancedSecondary = false;
        if (hasComparisonRun_) {
            advancedSecondary = secondarySession_.stepForward();
        }

        if (!advancedPrimary && !advancedSecondary) {
            statusMessage_ = "Already at the last frame.";
        }
    }

    void stepBackward() {
        if (!hasPrimaryRun_) {
            statusMessage_ = "Run an algorithm first.";
            return;
        }

        autoPlay_ = false;
        const bool rewoundPrimary = primarySession_.stepBackward();
        bool rewoundSecondary = false;
        if (hasComparisonRun_) {
            rewoundSecondary = secondarySession_.stepBackward();
        }

        if (!rewoundPrimary && !rewoundSecondary) {
            statusMessage_ = "Already at the first frame.";
        }
    }

    void resetPlayback() {
        autoPlay_ = false;
        primarySession_.reset();
        secondarySession_.reset();
        statusMessage_ = "Playback reset.";
    }

    void saveScenario() {
        saveNamedScenario();
    }

    void saveNamedScenario() {
        try {
            std::filesystem::create_directories(scenarioDirectory_);
            const std::string sanitized = sanitizeScenarioName(scenarioNameInput_);
            if (sanitized.empty()) {
                throw std::runtime_error("Enter a valid scenario name before saving.");
            }

            Scenario scenario;
            scenario.name = sanitized;
            scenario.mode = handler_.mode();
            scenario.selectedAlgorithm = primaryAlgorithm_;
            scenario.context.startNodeId = startNodeId_;
            scenario.context.goalNodeId = goalNodeId_;
            scenario.graph = handler_.graph();
            scenario.values = handler_.sortingValues();
            const std::filesystem::path targetPath = scenarioDirectory_ / (sanitized + ".scn");
            handler_.saveScenario(targetPath.string(), scenario);
            selectedScenarioName_ = sanitized;
            scenarioNameInput_ = sanitized;
            editingScenarioName_ = false;
            statusMessage_ = "Scenario saved as " + sanitized + ".";
        } catch (const std::exception& error) {
            statusMessage_ = error.what();
        }
    }

    void overwriteSelectedScenario() {
        if (selectedScenarioName_.empty()) {
            statusMessage_ = "Select a saved scenario first.";
            return;
        }

        scenarioNameInput_ = selectedScenarioName_;
        saveNamedScenario();
    }

    void loadScenario() {
        loadSelectedScenario();
    }

    void loadSelectedScenario() {
        try {
            if (selectedScenarioName_.empty()) {
                throw std::runtime_error("Select a saved scenario before loading.");
            }

            const std::filesystem::path targetPath = scenarioDirectory_ / (selectedScenarioName_ + ".scn");
            const Scenario scenario = handler_.loadScenario(targetPath.string());
            handler_.applyScenario(scenario);
            if (scenario.mode == WorkspaceMode::Sort) {
                primaryAlgorithm_ = scenario.selectedAlgorithm.empty() ? "Bubble" : scenario.selectedAlgorithm;
                secondaryAlgorithm_ = "Selection";
                syncArrayInputFromValues();
            } else if (!scenario.selectedAlgorithm.empty()) {
                primaryAlgorithm_ = scenario.selectedAlgorithm;
            }
            startNodeId_ = scenario.context.startNodeId;
            goalNodeId_ = scenario.context.goalNodeId;
            scenarioNameInput_ = selectedScenarioName_;
            selectedNodeId_.clear();
            selectedEdgeId_.clear();
            pendingEdgeStartId_.clear();
            invalidateRuns();
            statusMessage_ = "Loaded scenario " + selectedScenarioName_ + ".";
        } catch (const std::exception& error) {
            statusMessage_ = error.what();
        }
    }

    void startQuiz() {
        try {
            std::vector<std::string> candidates;
            for (const auto& algorithm : handler_.availableAlgorithms()) {
                try {
                    (void)handler_.createQuiz(algorithm, resolvedContextFor(algorithm));
                    candidates.push_back(algorithm);
                } catch (const std::exception&) {
                }
            }

            if (candidates.empty()) {
                throw std::runtime_error("No compatible algorithms are available for the current graph.");
            }

            std::mt19937 generator(std::random_device {}());
            std::uniform_int_distribution<std::size_t> distribution(0, candidates.size() - 1);
            const std::string chosen = candidates[distribution(generator)];
            quizQuestion_ = handler_.createQuiz(chosen, resolvedContextFor(chosen));
            quizFeedback_.clear();
            primaryRun_ = quizQuestion_->visualization;
            primarySession_.load(primaryRun_.frames);
            hasPrimaryRun_ = true;
            hasComparisonRun_ = false;
            secondarySession_.load({});
            autoPlay_ = false;
            statusMessage_ = "Quiz ready. Study the visualization and guess the algorithm.";
        } catch (const std::exception& error) {
            statusMessage_ = error.what();
        }
    }

    void advancePlaybackIfNeeded() {
        if (!autoPlay_ || !hasPrimaryRun_) {
            return;
        }

        if (playbackClock_.getElapsedTime().asSeconds() < playbackStepSeconds_) {
            return;
        }

        playbackClock_.restart();
        const bool advancedPrimary = primarySession_.stepForward();
        bool advancedSecondary = false;
        if (hasComparisonRun_) {
            advancedSecondary = secondarySession_.stepForward();
        }

        if (!advancedPrimary && !advancedSecondary) {
            autoPlay_ = false;
        }
    }

    const Graph& currentPrimaryGraph() const {
        if (hasPrimaryRun_ && !primarySession_.empty()) {
            return primarySession_.currentFrame().graph;
        }

        return handler_.graph();
    }

    const Graph& currentSecondaryGraph() const {
        if (hasComparisonRun_ && !secondarySession_.empty()) {
            return secondarySession_.currentFrame().graph;
        }

        return handler_.graph();
    }

    [[nodiscard]] std::vector<int> currentPrimaryArrayValues() const {
        if (hasPrimaryRun_ && !primarySession_.empty()) {
            const auto& frame = primarySession_.currentFrame();
            if (frame.visualizationType == av::algorithm::VisualizationType::Array) {
                return frame.arrayValues;
            }
        }

        return handler_.sortingValues();
    }

    [[nodiscard]] std::vector<ArrayCellState> currentPrimaryArrayStates() const {
        if (hasPrimaryRun_ && !primarySession_.empty()) {
            const auto& frame = primarySession_.currentFrame();
            if (frame.visualizationType == av::algorithm::VisualizationType::Array) {
                return frame.arrayStates;
            }
        }

        return std::vector<ArrayCellState>(handler_.sortingValues().size(), ArrayCellState::Normal);
    }

    [[nodiscard]] std::vector<int> currentSecondaryArrayValues() const {
        if (hasComparisonRun_ && !secondarySession_.empty()) {
            const auto& frame = secondarySession_.currentFrame();
            if (frame.visualizationType == av::algorithm::VisualizationType::Array) {
                return frame.arrayValues;
            }
        }

        return handler_.sortingValues();
    }

    [[nodiscard]] std::vector<ArrayCellState> currentSecondaryArrayStates() const {
        if (hasComparisonRun_ && !secondarySession_.empty()) {
            const auto& frame = secondarySession_.currentFrame();
            if (frame.visualizationType == av::algorithm::VisualizationType::Array) {
                return frame.arrayStates;
            }
        }

        return std::vector<ArrayCellState>(handler_.sortingValues().size(), ArrayCellState::Normal);
    }

    std::optional<PlaybackFrame> currentPrimaryFrame() const {
        if (hasPrimaryRun_ && !primarySession_.empty()) {
            return primarySession_.currentFrame();
        }

        return std::nullopt;
    }

    std::optional<PlaybackFrame> currentSecondaryFrame() const {
        if (hasComparisonRun_ && !secondarySession_.empty()) {
            return secondarySession_.currentFrame();
        }

        return std::nullopt;
    }

    void render() {
        window_.clear(sf::Color(246, 245, 240));
        renderer_.drawBackdrop();
        renderer_.drawHeader(quizActive() ? "QUIZ" : primaryAlgorithm_, sortingMode() ? "SORT" : "EDIT");

        if (comparisonEnabled_ && hasComparisonRun_) {
            const auto [left, right] = comparisonCanvases();
            if (sortingMode()) {
                renderer_.drawArrayCanvas(left.rect, currentPrimaryArrayValues(), currentPrimaryArrayStates(), left.title);
                renderer_.drawArrayCanvas(right.rect, currentSecondaryArrayValues(), currentSecondaryArrayStates(), right.title);
            } else {
                renderer_.drawGraphCanvas(left.rect, currentPrimaryGraph(), left.title, selectedNodeId_, selectedEdgeId_, startNodeId_, goalNodeId_);
                renderer_.drawGraphCanvas(right.rect, currentSecondaryGraph(), right.title, {}, {}, {}, {});
            }
        } else {
            if (sortingMode()) {
                renderer_.drawArrayCanvas(mainCanvasRect(), currentPrimaryArrayValues(), currentPrimaryArrayStates(), primaryDisplayTitle());
            } else {
                renderer_.drawGraphCanvas(mainCanvasRect(), currentPrimaryGraph(), primaryDisplayTitle(), selectedNodeId_, selectedEdgeId_, startNodeId_, goalNodeId_);
            }
        }

        sidebar_.draw(renderer_, handler_, primarySession_, secondarySession_, primaryAlgorithm_, secondaryAlgorithm_, statusMessage_, sf::Vector2f(window_.getSize()));
        if (!pendingEdgeStartId_.empty()) {
            const auto& startNode = handler_.graph().node(pendingEdgeStartId_);
            renderer_.drawPendingEdgePreview(
                {static_cast<float>(startNode.x), static_cast<float>(startNode.y)},
                currentMousePosition(), 
                mainCanvasRect()
            );
        }
        window_.display();
    }

};

}  // namespace

int main() {
    SimulationApp app;
    return app.run();
}