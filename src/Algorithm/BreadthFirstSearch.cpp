#include "Algorithm/BreadthFirstSearch.h"

#include <queue>
#include <stdexcept>
#include <unordered_set>

namespace av::algorithm {

std::string BreadthFirstSearch::name() const {
    return "BFS";
}

std::string BreadthFirstSearch::description() const {
    return "Breadth-first traversal for exploring a graph level by level.";
}

AlgorithmRunResult BreadthFirstSearch::run(const model::Graph& graph, const AlgorithmContext& context) const {
    if (context.startNodeId.empty()) {
        throw std::invalid_argument("BFS requires a start node");
    }
    if (!graph.hasNode(context.startNodeId)) {
        throw std::invalid_argument("BFS start node does not exist: " + context.startNodeId);
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
        "Initialize BFS",
        {"Mark the source node.", "Prepare a FIFO queue for level-order exploration."},
        metrics);

    std::queue<std::string> frontier;
    std::unordered_set<std::string> discovered;
    frontier.push(context.startNodeId);
    discovered.insert(context.startNodeId);

    while (!frontier.empty()) {
        const std::string current = frontier.front();
        frontier.pop();
        ++metrics.stepsTaken;
        ++metrics.visitedNodes;

        workingGraph.setNodeState(current, model::NodeState::Active, "dequeue");
        appendFrame(
            result.frames,
            workingGraph,
            "Visit " + current,
            {"The next node in the queue becomes active."},
            metrics);

        result.traversalOrder.push_back(current);
        for (const model::Neighbor& neighbor : workingGraph.neighbors(current)) {
            ++metrics.edgesConsidered;
            workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Frontier, "inspect");

            if (!discovered.contains(neighbor.nodeId)) {
                discovered.insert(neighbor.nodeId);
                frontier.push(neighbor.nodeId);
                workingGraph.setNodeState(neighbor.nodeId, model::NodeState::Frontier, "enqueue");
                workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Traversed, "tree edge");
                appendFrame(
                    result.frames,
                    workingGraph,
                    "Discover " + neighbor.nodeId,
                    {"An undiscovered neighbor is enqueued.", "BFS expands one layer at a time."},
                    metrics);
            } else {
                appendFrame(
                    result.frames,
                    workingGraph,
                    "Skip " + neighbor.nodeId,
                    {"The neighbor was already discovered, so the edge does not expand the search."},
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
        "BFS complete",
        {"Traversal finished.", "Nodes were processed in level-order."},
        metrics,
        true);
    return result;
}

}  // namespace av::algorithm