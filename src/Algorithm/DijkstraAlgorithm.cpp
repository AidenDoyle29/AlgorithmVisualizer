#include "Algorithm/DijkstraAlgorithm.h"

#include <functional>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace av::algorithm {

std::string DijkstraAlgorithm::name() const {
    return "Dijkstra";
}

std::string DijkstraAlgorithm::description() const {
    return "Single-source shortest paths for graphs with non-negative weights.";
}

bool DijkstraAlgorithm::requiresWeightedGraph() const noexcept {
    return true;
}

AlgorithmRunResult DijkstraAlgorithm::run(const model::Graph& graph, const AlgorithmContext& context) const {
    if (context.startNodeId.empty()) {
        throw std::invalid_argument("Dijkstra requires a start node");
    }
    if (!graph.hasNode(context.startNodeId)) {
        throw std::invalid_argument("Dijkstra start node does not exist: " + context.startNodeId);
    }

    auto workingGraph = graph;
    workingGraph.clearVisualState();
    workingGraph.setNodeState(context.startNodeId, model::NodeState::Start, "distance = 0");

    AlgorithmRunResult result;
    result.algorithmName = name();
    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    std::unordered_map<std::string, double> distance;
    std::unordered_map<std::string, std::string> predecessor;
    for (const auto& node : workingGraph.nodes()) {
        distance[node.id] = infinity();
    }
    distance[context.startNodeId] = 0.0;

    using QueueEntry = std::pair<double, std::string>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> frontier;
    frontier.push({0.0, context.startNodeId});
    std::unordered_set<std::string> settled;

    appendFrame(
        result.frames,
        workingGraph,
        "Initialize Dijkstra",
        {"Set all distances to infinity except the source.", "Use a priority queue ordered by tentative distance."},
        metrics);

    while (!frontier.empty()) {
        const auto [currentDistance, current] = frontier.top();
        frontier.pop();
        if (settled.contains(current)) {
            continue;
        }

        settled.insert(current);
        ++metrics.stepsTaken;
        ++metrics.visitedNodes;
        workingGraph.setNodeState(current, model::NodeState::Active, "settled");

        {
            std::ostringstream detail;
            detail << "The next minimum-distance node is " << current << " with distance " << currentDistance << ".";
            appendFrame(result.frames, workingGraph, "Settle " + current, {detail.str()}, metrics);
        }

        result.traversalOrder.push_back(current);

        for (const model::Neighbor& neighbor : workingGraph.neighbors(current)) {
            ++metrics.edgesConsidered;
            ++metrics.comparisons;
            const model::Edge& edge = workingGraph.edgeAt(neighbor.edgeIndex);
            if (edge.weight < 0.0) {
                throw std::invalid_argument("Dijkstra does not support negative edge weights");
            }

            const double candidateDistance = currentDistance + edge.weight;
            workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Frontier, "relax?");

            if (candidateDistance < distance[neighbor.nodeId]) {
                distance[neighbor.nodeId] = candidateDistance;
                predecessor[neighbor.nodeId] = current;
                frontier.push({candidateDistance, neighbor.nodeId});
                ++metrics.relaxations;
                workingGraph.setNodeState(neighbor.nodeId, model::NodeState::Frontier, "distance improved");
                workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Relaxed, "relaxed");

                std::ostringstream detail;
                detail << "Updated shortest distance to " << neighbor.nodeId << " = " << candidateDistance << ".";
                appendFrame(result.frames, workingGraph, "Relax " + neighbor.nodeId, {detail.str()}, metrics);
            } else {
                appendFrame(
                    result.frames,
                    workingGraph,
                    "Keep distance for " + neighbor.nodeId,
                    {"The candidate path is not shorter than the best known distance."},
                    metrics);
            }
        }

        workingGraph.setNodeState(current, model::NodeState::Visited, "finalized");
    }

    if (!context.goalNodeId.empty() && predecessor.contains(context.goalNodeId)) {
        std::string current = context.goalNodeId;
        while (predecessor.contains(current)) {
            workingGraph.setNodeState(current, model::NodeState::Path, "shortest path");
            const auto edgeIndex = workingGraph.findEdgeIndexByEndpoints(predecessor[current], current);
            if (edgeIndex.has_value()) {
                workingGraph.setEdgeStateByIndex(*edgeIndex, model::EdgeState::Traversed, "shortest path");
            }
            current = predecessor[current];
        }
        workingGraph.setNodeState(context.startNodeId, model::NodeState::Start, "source");
    }

    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    result.distances = std::move(distance);
    appendFrame(
        result.frames,
        workingGraph,
        "Dijkstra complete",
        {"All reachable nodes have their shortest known distance."},
        metrics,
        true);
    return result;
}

}  // namespace av::algorithm
