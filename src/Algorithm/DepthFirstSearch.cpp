#include "Algorithm/DepthFirstSearch.h"

#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace av::algorithm {

std::string DepthFirstSearch::name() const {
    return "DFS";
}

std::string DepthFirstSearch::description() const {
    return "Depth-first traversal for exploring a graph by following one branch as far as possible.";
}

AlgorithmRunResult DepthFirstSearch::run(const model::Graph& graph, const AlgorithmContext& context) const {
    if (context.startNodeId.empty()) {
        throw std::invalid_argument("DFS requires a start node");
    }
    if (!graph.hasNode(context.startNodeId)) {
        throw std::invalid_argument("DFS start node does not exist: " + context.startNodeId);
    }

    auto workingGraph = graph;
    workingGraph.clearVisualState();
    workingGraph.setNodeState(context.startNodeId, model::NodeState::Start, "source");

    AlgorithmRunResult result;
    result.algorithmName = name();
    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    appendFrame(
        result.frames,
        workingGraph,
        "Initialize DFS",
        {"Mark the source node.", "Use a LIFO stack to prioritize depth."},
        metrics);

    std::vector<std::string> stack {context.startNodeId};
    std::unordered_set<std::string> discovered;

    while (!stack.empty()) {
        const std::string current = stack.back();
        stack.pop_back();
        if (discovered.contains(current)) {
            continue;
        }

        discovered.insert(current);
        ++metrics.stepsTaken;
        ++metrics.visitedNodes;

        workingGraph.setNodeState(current, model::NodeState::Active, "pop");
        appendFrame(
            result.frames,
            workingGraph,
            "Visit " + current,
            {"The most recently discovered node is explored next."},
            metrics);

        result.traversalOrder.push_back(current);

        auto neighbors = workingGraph.neighbors(current);
        for (auto iterator = neighbors.rbegin(); iterator != neighbors.rend(); ++iterator) {
            ++metrics.edgesConsidered;
            const model::Neighbor& neighbor = *iterator;
            workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Frontier, "inspect");

            if (!discovered.contains(neighbor.nodeId)) {
                stack.push_back(neighbor.nodeId);
                workingGraph.setNodeState(neighbor.nodeId, model::NodeState::Frontier, "push");
                workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Traversed, "tree edge");
                appendFrame(
                    result.frames,
                    workingGraph,
                    "Push " + neighbor.nodeId,
                    {"The neighbor is added to the stack for deeper exploration."},
                    metrics);
            } else {
                appendFrame(
                    result.frames,
                    workingGraph,
                    "Skip " + neighbor.nodeId,
                    {"The neighbor was already visited, so this edge does not recurse further."},
                    metrics);
            }
        }

        workingGraph.setNodeState(current, model::NodeState::Visited, "processed");
    }

    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    appendFrame(
        result.frames,
        workingGraph,
        "DFS complete",
        {"Traversal finished.", "Nodes were processed depth-first."},
        metrics,
        true);
    return result;
}

}  // namespace av::algorithm
