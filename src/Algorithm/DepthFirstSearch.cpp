#include "Algorithm/DepthFirstSearch.h"

#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace av::algorithm {

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing the shorthand name "DFS".
 */
std::string DepthFirstSearch::name() const {
    return "DFS";
}

/**
 * @brief Provides a summary of the Depth-First Search strategy.
 * @return A string describing the exhaustive branch-based exploration.
 */
std::string DepthFirstSearch::description() const {
    return "Depth-first traversal for exploring a graph by following one branch as far as possible.";
}

/**
 * @brief Executes the Depth-First Search algorithm on the provided graph.
 * 
 * This implementation uses an iterative approach with a stack to simulate recursion.
 * It explores as deep as possible along each branch before backtracking, capturing 
 * visualization frames for stack operations (push/pop) and discovery events.
 * 
 * @param graph The graph structure to explore.
 * @param context Configuration for the run, containing the required startNodeId.
 * @return An AlgorithmRunResult containing the node visit order, metrics, and animation frames.
 * @throws std::invalid_argument if the start node is missing or invalid.
 */
AlgorithmRunResult DepthFirstSearch::run(const model::Graph& graph, const AlgorithmContext& context) const {
    if (context.startNodeId.empty()) {
        throw std::invalid_argument("DFS requires a start node");
    }
    if (!graph.hasNode(context.startNodeId)) {
        throw std::invalid_argument("DFS start node does not exist: " + context.startNodeId);
    }

    // Initialize the working copy of the graph for visual state management
    auto workingGraph = graph;
    workingGraph.clearVisualState();
    workingGraph.setNodeState(context.startNodeId, model::NodeState::Start, "source");

    AlgorithmRunResult result;
    result.algorithmName = name();
    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    // Initial frame showing the algorithm entry point
    appendFrame(
        result.frames,
        workingGraph,
        "Initialize DFS",
        {"Mark the source node.", "Use a LIFO stack to prioritize depth."},
        metrics);

    // Iterative stack-based DFS implementation
    std::vector<std::string> stack {context.startNodeId};
    std::unordered_set<std::string> discovered;

    while (!stack.empty()) {
        const std::string current = stack.back();
        stack.pop_back();

        // Skip nodes that were already finalized to handle multiple paths to the same node in the stack
        if (discovered.contains(current)) {
            continue;
        }

        discovered.insert(current);
        ++metrics.stepsTaken;
        ++metrics.visitedNodes;

        // Visual update: Mark the node currently being popped and visited
        workingGraph.setNodeState(current, model::NodeState::Active, "pop");
        appendFrame(
            result.frames,
            workingGraph,
            "Visit " + current,
            {"The most recently discovered node is explored next."},
            metrics);

        result.traversalOrder.push_back(current);

        // Retrieve neighbors and iterate in reverse to maintain expected discovery order 
        // when using a stack (LIFO)
        auto neighbors = workingGraph.neighbors(current);
        for (auto iterator = neighbors.rbegin(); iterator != neighbors.rend(); ++iterator) {
            ++metrics.edgesConsidered;
            const model::Neighbor& neighbor = *iterator;
            workingGraph.setEdgeStateByIndex(neighbor.edgeIndex, model::EdgeState::Frontier, "inspect");

            if (!discovered.contains(neighbor.nodeId)) {
                // New branch found: push to stack and update visual frontier
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
                // Node already visited: show inspection but skip adding to stack
                appendFrame(
                    result.frames,
                    workingGraph,
                    "Skip " + neighbor.nodeId,
                    {"The neighbor was already visited, so this edge does not recurse further."},
                    metrics);
            }
        }

        // Finalize node state after exploring its local neighborhood
        workingGraph.setNodeState(current, model::NodeState::Visited, "processed");
    }

    // Finalize performance metrics and capture the completion frame
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