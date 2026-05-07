/**
 * @file InputHandler.h
 * @brief Defines the core Controller layer for the MVC architecture.
 *
 * This file implements the main orchestrator of the application. It bridges 
 * the View (UI/Input) and the Model (Graph/Arrays), delegates execution to 
 * the appropriate Algorithm Factories, and manages advanced educational 
 * features like scenario serialization, side-by-side comparisons, and quizzes.
 */

#ifndef ALGORITHM_VISUALIZER_CONTROLLER_INPUT_HANDLER_H
#define ALGORITHM_VISUALIZER_CONTROLLER_INPUT_HANDLER_H

#include "Algorithm/AlgorithmFactory.h"
#include "Algorithm/SortingAlgorithmFactory.h"
#include "Playback/PlaybackSession.h"
#include "View/GraphRenderer.h"

#include <string>
#include <vector>

namespace av::controller {

/**
 * @enum WorkspaceMode
 * @brief Defines the current active mode of the application workspace.
 */
enum class WorkspaceMode {
    Graph,  /**< The workspace is configured for pathfinding and graph algorithms. */
    Sort    /**< The workspace is configured for array sorting algorithms. */
};

/**
 * @struct Scenario
 * @brief A serializable snapshot of the entire workspace state.
 *
 * Used to save and load specific configurations, allowing users to share 
 * complex graph setups, specific array structures, and chosen algorithms 
 * without having to recreate them manually.
 */
struct Scenario {
    std::string name;                           /**< The human-readable name of the scenario. */
    WorkspaceMode mode {WorkspaceMode::Graph};  /**< The workspace mode required for this scenario. */
    std::string selectedAlgorithm;              /**< The name of the algorithm primed to run. */
    algorithm::AlgorithmContext context;        /**< Execution context parameters (e.g., start node). */
    model::Graph graph;                         /**< The saved state of the mathematical graph. */
    std::vector<int> values;                    /**< The saved state of the sorting array. */
};

/**
 * @struct ComparisonResult
 * @brief Holds the results of two algorithms executed on the same data.
 *
 * Used by the UI to render side-by-side visual playbacks and metric 
 * dashboards, helping users understand relative time/space complexities.
 */
struct ComparisonResult {
    algorithm::AlgorithmRunResult left;     /**< The complete result package of the first algorithm. */
    algorithm::AlgorithmRunResult right;    /**< The complete result package of the second algorithm. */
};

/**
 * @struct QuizQuestion
 * @brief An educational construct to test the user's algorithm knowledge.
 *
 * Generates an anonymous visual playback. The user watches the steps 
 * and must guess which algorithm is currently running from a list of choices.
 */
struct QuizQuestion {
    std::string prompt;                     /**< The text prompt displayed to the user. */
    std::string correctAlgorithm;           /**< The hidden correct answer. */
    std::vector<std::string> choices;       /**< The multiple-choice options. */
    algorithm::AlgorithmRunResult visualization; /**< The playback frames the user must watch to guess the answer. */
};

/**
 * @class InputHandler
 * @brief The central Controller class orchestrating the application's logic.
 *
 * The InputHandler catches raw requests from the UI (like "Run BFS" or "Shuffle Array") 
 * and translates them into concrete actions on the Model or Playback engine. It maintains 
 * the singular source of truth for the active workspace mode, the current graph/array state, 
 * and the initialized factories.
 */
class InputHandler {
  public:
    /**
     * @brief Constructs the InputHandler, initializing factories and default states.
     */
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

    /**
     * @brief Clears the active workspace, resetting models and playback states.
     * @param directed If true, reinitializes the graph as directed; otherwise, undirected.
     */
    void resetWorkspace(bool directed);

    /**
     * @brief Populates the graph with a pre-configured sample layout.
     */
    void buildSampleGraph();

    /**
     * @brief Populates the sorting array with a pre-configured sample dataset.
     */
    void buildSampleArray();

    /**
     * @brief Randomizes the order of elements in the active sorting array.
     */
    void shuffleSortingValues();

    /**
     * @brief Generates a strictly ascending array dataset.
     */
    void generateSortedArray();

    /**
     * @brief Generates a strictly descending array dataset (worst-case for many algorithms).
     */
    void generateReverseSortedArray();

    /**
     * @brief Manually overrides the current sorting array with a custom dataset.
     * @param values The custom vector of integers to sort.
     */
    void setSortingValues(std::vector<int> values);

    /**
     * @brief Switches the application focus between Graph and Sorting modes.
     * @param mode The target WorkspaceMode enum.
     */
    void setMode(WorkspaceMode mode);

    /**
     * @brief Dynamically selects an algorithm regardless of the current mode.
     * @param name The exact string name of the algorithm.
     */
    void selectAlgorithm(const std::string& name);
    void selectGraphAlgorithm(const std::string& name);
    void selectSortingAlgorithm(const std::string& name);

    /**
     * @brief Executes the currently selected algorithm against the active data state.
     * @param context Configuration parameters (e.g., start node) for the run.
     * @return AlgorithmRunResult The generated Memento frames and statistical metrics.
     */
    algorithm::AlgorithmRunResult runSelectedAlgorithm(const algorithm::AlgorithmContext& context);

    /**
     * @brief Executes two algorithms sequentially to generate side-by-side data.
     * @param leftAlgorithm The exact string name of the first algorithm.
     * @param rightAlgorithm The exact string name of the second algorithm.
     * @param context Configuration parameters for both runs.
     * @return ComparisonResult A package containing both distinct run results.
     */
    ComparisonResult compareAlgorithms(
        const std::string& leftAlgorithm,
        const std::string& rightAlgorithm,
        const algorithm::AlgorithmContext& context) const;

    /**
     * @brief Generates a visual quiz by running an algorithm and obfuscating its name.
     * @param algorithmName The algorithm to run for the quiz.
     * @param context Configuration parameters for the run.
     * @return QuizQuestion A structured educational package containing the playback frames and multiple-choice options.
     */
    QuizQuestion createQuiz(const std::string& algorithmName, const algorithm::AlgorithmContext& context) const;

    /**
     * @brief Validates a user's guess against a generated quiz.
     * @param question The active quiz being solved.
     * @param guess The user's submitted string answer.
     * @return true If the guess matches the hidden correct algorithm.
     */
    [[nodiscard]] bool checkQuizAnswer(const QuizQuestion& question, const std::string& guess) const;

    /**
     * @brief Serializes the current workspace layout to the local filesystem.
     * @param path The destination file path.
     * @param scenario The packaged state of the workspace.
     */
    void saveScenario(const std::string& path, const Scenario& scenario) const;

    /**
     * @brief Deserializes a workspace layout from the local filesystem.
     * @param path The target file path to read from.
     * @return Scenario The packaged state ready to be applied.
     */
    [[nodiscard]] Scenario loadScenario(const std::string& path) const;

    /**
     * @brief Overwrites the active workspace with data from a loaded scenario.
     * @param scenario The structured state to inject into the active models.
     */
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