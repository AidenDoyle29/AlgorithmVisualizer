#ifndef ALGORITHM_VISUALIZER_CONTROLLER_INPUT_HANDLER_H
#define ALGORITHM_VISUALIZER_CONTROLLER_INPUT_HANDLER_H

#include "Algorithm/AlgorithmFactory.h"
#include "Algorithm/SortingAlgorithmFactory.h"
#include "Playback/PlaybackSession.h"
#include "View/GraphRenderer.h"

#include <string>
#include <vector>

namespace av::controller {

enum class WorkspaceMode {
    Graph,
    Sort
};

struct Scenario {
    std::string name;
    WorkspaceMode mode {WorkspaceMode::Graph};
    std::string selectedAlgorithm;
    algorithm::AlgorithmContext context;
    model::Graph graph;
    std::vector<int> values;
};

struct ComparisonResult {
    algorithm::AlgorithmRunResult left;
    algorithm::AlgorithmRunResult right;
};

struct QuizQuestion {
    std::string prompt;
    std::string correctAlgorithm;
    std::vector<std::string> choices;
    algorithm::AlgorithmRunResult visualization;
};

class InputHandler {
  public:
    InputHandler();

    [[nodiscard]] model::Graph& graph() noexcept;
    [[nodiscard]] const model::Graph& graph() const noexcept;
    [[nodiscard]] const view::GraphRenderer& renderer() const noexcept;
    [[nodiscard]] playback::PlaybackSession& playbackSession() noexcept;
    [[nodiscard]] const playback::PlaybackSession& playbackSession() const noexcept;
    [[nodiscard]] WorkspaceMode mode() const noexcept;
    [[nodiscard]] std::vector<std::string> availableAlgorithms() const;
    [[nodiscard]] std::vector<std::string> availableGraphAlgorithms() const;
    [[nodiscard]] std::vector<std::string> availableSortingAlgorithms() const;
    [[nodiscard]] const std::string& selectedAlgorithm() const noexcept;
    [[nodiscard]] const std::string& selectedGraphAlgorithm() const noexcept;
    [[nodiscard]] const std::string& selectedSortingAlgorithm() const noexcept;
    [[nodiscard]] const std::vector<int>& sortingValues() const noexcept;

    void resetWorkspace(bool directed);
    void buildSampleGraph();
    void buildSampleArray();
    void shuffleSortingValues();
    void generateSortedArray();
    void generateReverseSortedArray();
    void setSortingValues(std::vector<int> values);
    void setMode(WorkspaceMode mode);
    void selectAlgorithm(const std::string& name);
    void selectGraphAlgorithm(const std::string& name);
    void selectSortingAlgorithm(const std::string& name);

    algorithm::AlgorithmRunResult runSelectedAlgorithm(const algorithm::AlgorithmContext& context);
    ComparisonResult compareAlgorithms(
        const std::string& leftAlgorithm,
        const std::string& rightAlgorithm,
        const algorithm::AlgorithmContext& context) const;

    QuizQuestion createQuiz(const std::string& algorithmName, const algorithm::AlgorithmContext& context) const;
    [[nodiscard]] bool checkQuizAnswer(const QuizQuestion& question, const std::string& guess) const;

    void saveScenario(const std::string& path, const Scenario& scenario) const;
    [[nodiscard]] Scenario loadScenario(const std::string& path) const;
    void applyScenario(const Scenario& scenario);

  private:
    model::Graph graph_;
    view::GraphRenderer renderer_;
    algorithm::AlgorithmFactory algorithmFactory_;
    algorithm::SortingAlgorithmFactory sortingAlgorithmFactory_;
    playback::PlaybackSession playbackSession_ {};
    WorkspaceMode mode_ {WorkspaceMode::Graph};
    std::string selectedGraphAlgorithm_ {"BFS"};
    std::string selectedSortingAlgorithm_ {"Bubble"};
    std::vector<int> sortingValues_ {7, 3, 8, 2, 6, 4, 1, 5};

    [[nodiscard]] std::shared_ptr<const algorithm::IAlgorithm> requireAlgorithm(const std::string& name) const;
    [[nodiscard]] std::shared_ptr<const algorithm::ISortingAlgorithm> requireSortingAlgorithm(const std::string& name) const;
};

}  // namespace av::controller

#endif
