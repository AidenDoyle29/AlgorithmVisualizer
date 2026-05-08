#include "Controller/InputHandler.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

void printHelp() {
    std::cout << "AlgorithmVisualizer CLI\n";
    std::cout << "Commands:\n";
    std::cout << "  help\n";
    std::cout << "  demo\n";
    std::cout << "  mode <graph|sort>\n";
    std::cout << "  algorithms\n";
    std::cout << "  show\n";
    std::cout << "  select <algorithm>\n";
    std::cout << "  array <values...>\n";
    std::cout << "  sample-array\n";
    std::cout << "  shuffle-array\n";
    std::cout << "  run [start] [goal]\n";
    std::cout << "  compare <left> <right> [start] [goal]\n";
    std::cout << "  quiz <algorithm> [start] [guess]\n";
    std::cout << "  save <file>\n";
    std::cout << "  load <file>\n";
}

void renderRun(av::controller::InputHandler& handler, const av::algorithm::AlgorithmContext& context) {
    const auto result = handler.runSelectedAlgorithm(context);
    for (const auto& frame : result.frames) {
        std::cout << handler.renderer().renderFrame(frame) << "\n";
    }
}

void renderWorkspace(av::controller::InputHandler& handler) {
    if (handler.mode() == av::controller::WorkspaceMode::Sort) {
        av::algorithm::PlaybackFrame frame;
        frame.visualizationType = av::algorithm::VisualizationType::Array;
        frame.arrayValues = handler.sortingValues();
        frame.arrayStates = std::vector<av::algorithm::ArrayCellState>(frame.arrayValues.size(), av::algorithm::ArrayCellState::Normal);
        frame.title = "Current array";
        std::cout << handler.renderer().renderFrame(frame);
        return;
    }

    std::cout << handler.renderer().renderGraph(handler.graph());
}

}  // namespace

int main(int argc, char** argv) {
    av::controller::InputHandler handler;
    handler.buildSampleGraph();

    if (argc == 1) {
        printHelp();
        return 0;
    }

    try {
        const std::string command = argv[1];

        if (command == "help") {
            printHelp();
            return 0;
        }

        if (command == "demo") {
            handler.selectAlgorithm("Dijkstra");
            av::algorithm::AlgorithmContext context;
            context.startNodeId = "A";
            context.goalNodeId = "E";
            renderRun(handler, context);
            return 0;
        }

        if (command == "mode") {
            if (argc < 3) {
                throw std::invalid_argument("mode requires <graph|sort>");
            }
            const std::string modeName = argv[2];
            if (modeName == "graph") {
                handler.setMode(av::controller::WorkspaceMode::Graph);
                std::cout << "Mode set to graph\n";
            } else if (modeName == "sort") {
                handler.setMode(av::controller::WorkspaceMode::Sort);
                std::cout << "Mode set to sort\n";
            } else {
                throw std::invalid_argument("unknown mode: " + modeName);
            }
            return 0;
        }

        if (command == "algorithms") {
            for (const auto& name : handler.availableAlgorithms()) {
                std::cout << name << "\n";
            }
            return 0;
        }

        if (command == "show") {
            renderWorkspace(handler);
            return 0;
        }

        if (command == "select") {
            if (argc < 3) {
                throw std::invalid_argument("select requires an algorithm name");
            }
            handler.selectAlgorithm(argv[2]);
            std::cout << "Selected algorithm: " << handler.selectedAlgorithm() << "\n";
            return 0;
        }

        if (command == "array") {
            if (argc < 3) {
                throw std::invalid_argument("array requires at least one integer");
            }
            std::vector<int> values;
            for (int index = 2; index < argc; ++index) {
                values.push_back(std::stoi(argv[index]));
            }
            handler.setMode(av::controller::WorkspaceMode::Sort);
            handler.setSortingValues(std::move(values));
            std::cout << "Loaded sorting array with " << handler.sortingValues().size() << " values\n";
            return 0;
        }

        if (command == "sample-array") {
            handler.setMode(av::controller::WorkspaceMode::Sort);
            handler.buildSampleArray();
            std::cout << "Loaded sample sorting array\n";
            return 0;
        }

        if (command == "shuffle-array") {
            handler.setMode(av::controller::WorkspaceMode::Sort);
            handler.shuffleSortingValues();
            std::cout << "Shuffled sorting array\n";
            return 0;
        }

        if (command == "run") {
            if (handler.mode() == av::controller::WorkspaceMode::Graph) {
                if (argc < 3) {
                    throw std::invalid_argument("run requires a start node");
                }
                av::algorithm::AlgorithmContext context;
                context.startNodeId = argv[2];
                if (argc > 3) {
                    context.goalNodeId = argv[3];
                }
                renderRun(handler, context);
            } else {
                renderRun(handler, {});
            }
            return 0;
        }

        if (command == "compare") {
            if (handler.mode() == av::controller::WorkspaceMode::Graph) {
                if (argc < 5) {
                    throw std::invalid_argument("compare requires <left> <right> <start> [goal]");
                }

                av::algorithm::AlgorithmContext context;
                context.startNodeId = argv[4];
                if (argc > 5) {
                    context.goalNodeId = argv[5];
                }
                const auto comparison = handler.compareAlgorithms(argv[2], argv[3], context);
                const auto view = handler.renderer().renderComparison(comparison.left, comparison.right);
                std::cout << view.summary << "\n";
                std::cout << "--- Left ---\n" << view.left << "\n";
                std::cout << "--- Right ---\n" << view.right << "\n";
            } else {
                if (argc < 4) {
                    throw std::invalid_argument("compare requires <left> <right>");
                }
                const auto comparison = handler.compareAlgorithms(argv[2], argv[3], {});
                const auto view = handler.renderer().renderComparison(comparison.left, comparison.right);
                std::cout << view.summary << "\n";
                std::cout << "--- Left ---\n" << view.left << "\n";
                std::cout << "--- Right ---\n" << view.right << "\n";
            }
            return 0;
        }

        if (command == "quiz") {
            if (handler.mode() == av::controller::WorkspaceMode::Graph) {
                if (argc < 4) {
                    throw std::invalid_argument("quiz requires <algorithm> <start> [guess]");
                }
                av::algorithm::AlgorithmContext context;
                context.startNodeId = argv[3];
                const auto quiz = handler.createQuiz(argv[2], context);
                std::cout << quiz.prompt << "\n";
                std::cout << "Choices:";
                for (const auto& choice : quiz.choices) {
                    std::cout << " " << choice;
                }
                std::cout << "\n";
                if (!quiz.visualization.frames.empty()) {
                    std::cout << handler.renderer().renderFrame(quiz.visualization.frames.back()) << "\n";
                }
                if (argc > 4) {
                    std::cout << (handler.checkQuizAnswer(quiz, argv[4]) ? "Correct\n" : "Incorrect\n");
                }
            } else {
                if (argc < 3) {
                    throw std::invalid_argument("quiz requires <algorithm> [guess]");
                }
                const auto quiz = handler.createQuiz(argv[2], {});
                std::cout << quiz.prompt << "\n";
                std::cout << "Choices:";
                for (const auto& choice : quiz.choices) {
                    std::cout << " " << choice;
                }
                std::cout << "\n";
                if (!quiz.visualization.frames.empty()) {
                    std::cout << handler.renderer().renderFrame(quiz.visualization.frames.back()) << "\n";
                }
                if (argc > 3) {
                    std::cout << (handler.checkQuizAnswer(quiz, argv[3]) ? "Correct\n" : "Incorrect\n");
                }
            }
            return 0;
        }

        if (command == "save") {
            if (argc < 3) {
                throw std::invalid_argument("save requires a file path");
            }
            av::controller::Scenario scenario;
            scenario.name = "sample";
            scenario.mode = handler.mode();
            scenario.selectedAlgorithm = handler.selectedAlgorithm();
            scenario.context.startNodeId = "A";
            scenario.context.goalNodeId = "E";
            scenario.graph = handler.graph();
            scenario.values = handler.sortingValues();
            handler.saveScenario(argv[2], scenario);
            std::cout << "Saved scenario to " << std::filesystem::absolute(argv[2]) << "\n";
            return 0;
        }

        if (command == "load") {
            if (argc < 3) {
                throw std::invalid_argument("load requires a file path");
            }
            const auto scenario = handler.loadScenario(argv[2]);
            std::cout << "Loaded scenario: " << scenario.name << "\n";
            if (scenario.mode == av::controller::WorkspaceMode::Sort) {
                av::algorithm::PlaybackFrame frame;
                frame.visualizationType = av::algorithm::VisualizationType::Array;
                frame.arrayValues = scenario.values;
                frame.arrayStates = std::vector<av::algorithm::ArrayCellState>(scenario.values.size(), av::algorithm::ArrayCellState::Normal);
                frame.title = "Loaded array";
                std::cout << handler.renderer().renderFrame(frame);
            } else {
                std::cout << handler.renderer().renderGraph(scenario.graph);
            }
            return 0;
        }

        throw std::invalid_argument("unknown command: " + command);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}