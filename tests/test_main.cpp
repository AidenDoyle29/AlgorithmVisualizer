#include "Controller/InputHandler.h"

#include <cassert>
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace {

void testBreadthFirstSearch() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();
    handler.selectAlgorithm("BFS");
    av::algorithm::AlgorithmContext context;
    context.startNodeId = "A";
    const auto result = handler.runSelectedAlgorithm(context);

    assert(!result.frames.empty());
    assert(result.traversalOrder.size() == handler.graph().nodeCount());
    assert(result.traversalOrder.front() == "A");
    assert(result.metrics.visitedNodes == handler.graph().nodeCount());
}

void testDepthFirstSearch() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();
    handler.selectAlgorithm("DFS");
    av::algorithm::AlgorithmContext context;
    context.startNodeId = "A";
    const auto result = handler.runSelectedAlgorithm(context);

    assert(!result.frames.empty());
    assert(result.traversalOrder.front() == "A");
    assert(result.metrics.visitedNodes == handler.graph().nodeCount());
}

void testDijkstraDistances() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();
    handler.selectAlgorithm("Dijkstra");
    av::algorithm::AlgorithmContext context;
    context.startNodeId = "A";
    context.goalNodeId = "E";
    const auto result = handler.runSelectedAlgorithm(context);

    assert(result.distances.at("A") == 0.0);
    assert(result.distances.at("B") == 2.0);
    assert(result.distances.at("D") == 3.0);
    assert(result.distances.at("E") == 6.0);
}

void testKruskalWeight() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();
    handler.selectAlgorithm("Kruskal");
    av::algorithm::AlgorithmContext context;
    const auto result = handler.runSelectedAlgorithm(context);

    assert(result.metrics.mstEdgesChosen == handler.graph().nodeCount() - 1);
    assert(result.metrics.totalWeight == 8.0);
}

void testComparisonAndQuiz() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();

    av::algorithm::AlgorithmContext context;
    context.startNodeId = "A";
    const auto comparison = handler.compareAlgorithms("BFS", "DFS", context);
    assert(!comparison.left.frames.empty());
    assert(!comparison.right.frames.empty());

    const auto quiz = handler.createQuiz("BFS", context);
    assert(handler.checkQuizAnswer(quiz, "BFS"));
    assert(!handler.checkQuizAnswer(quiz, "DFS"));
}

void testScenarioRoundTrip() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();

    const auto path = std::filesystem::temp_directory_path() / "algorithm_visualizer.scn";
    av::controller::Scenario scenario;
    scenario.name = "roundtrip";
    scenario.selectedAlgorithm = "Dijkstra";
    scenario.context.startNodeId = "A";
    scenario.context.goalNodeId = "E";
    scenario.graph = handler.graph();
    handler.saveScenario(path.string(), scenario);
    const auto loaded = handler.loadScenario(path.string());

    assert(loaded.name == "roundtrip");
    assert(loaded.selectedAlgorithm == "Dijkstra");
    assert(loaded.context.startNodeId == "A");
    assert(loaded.context.goalNodeId == "E");
    assert(loaded.graph.nodeCount() == handler.graph().nodeCount());
    assert(loaded.graph.edgeCount() == handler.graph().edgeCount());
    std::filesystem::remove(path);
}

void testPlaybackControls() {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();
    handler.selectAlgorithm("BFS");
    av::algorithm::AlgorithmContext context;
    context.startNodeId = "A";
    const auto result = handler.runSelectedAlgorithm(context);
    (void)result;

    auto session = handler.playbackSession();
    assert(!session.empty());
    assert(session.currentIndex() == 0);
    assert(session.stepForward());
    assert(session.currentIndex() == 1);
    assert(session.stepBackward());
    assert(session.currentIndex() == 0);
    session.resume();
    assert(!session.isPaused());
    assert(session.advanceTick());
    session.reset();
    assert(session.currentIndex() == 0);
}

void testSortingAlgorithms() {
    const std::vector<std::string> algorithms {"Bubble", "Selection", "Insertion", "Merge", "Quick"};
    const std::vector<std::vector<int>> inputs {{5, 1, 4, 2, 8}, {1, 2, 3, 4, 5}, {9, 7, 5, 3, 1}, {4, 2, 4, 1, 3}, {}, {42}};

    for (const auto& algorithm : algorithms) {
        for (const auto& input : inputs) {
            av::controller::InputHandler handler;
            handler.setMode(av::controller::WorkspaceMode::Sort);
            handler.setSortingValues(input);
            handler.selectAlgorithm(algorithm);
            const auto result = handler.runSelectedAlgorithm({});
            assert(std::is_sorted(result.sortedValues.begin(), result.sortedValues.end()));
            assert(!result.frames.empty());
            assert(result.frames.back().terminal);
            assert(result.frames.back().visualizationType == av::algorithm::VisualizationType::Array);
        }
    }
}

void testSortingScenarioRoundTrip() {
    av::controller::InputHandler handler;
    handler.setMode(av::controller::WorkspaceMode::Sort);
    handler.setSortingValues({4, 1, 3, 2});
    handler.selectAlgorithm("Merge");

    const auto path = std::filesystem::temp_directory_path() / "algorithm_visualizer_sort.scn";
    av::controller::Scenario scenario;
    scenario.name = "sort_roundtrip";
    scenario.mode = av::controller::WorkspaceMode::Sort;
    scenario.selectedAlgorithm = "Merge";
    scenario.values = handler.sortingValues();
    handler.saveScenario(path.string(), scenario);
    const auto loaded = handler.loadScenario(path.string());

    assert(loaded.mode == av::controller::WorkspaceMode::Sort);
    assert(loaded.selectedAlgorithm == "Merge");
    assert(loaded.values == std::vector<int>({4, 1, 3, 2}));
    std::filesystem::remove(path);
}

}  // namespace

int main() {
    testBreadthFirstSearch();
    testDepthFirstSearch();
    testDijkstraDistances();
    testKruskalWeight();
    testComparisonAndQuiz();
    testScenarioRoundTrip();
    testPlaybackControls();
    testSortingAlgorithms();
    testSortingScenarioRoundTrip();

    std::cout << "All tests passed.\n";
    return 0;
}
