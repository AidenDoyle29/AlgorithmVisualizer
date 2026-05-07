#include "Algorithm/KruskalAlgorithm.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <unordered_map>

namespace av::algorithm {

namespace {

/**
 * @brief A Union-Find (Disjoint Set) data structure to manage connected components.
 * 
 * This structure uses path compression and union-by-rank optimizations to ensure 
 * near-constant time operations, which is essential for the efficiency of Kruskal's algorithm.
 */
class DisjointSet {
  public:
    /**
     * @brief Initializes the set with each node in its own component.
     * @param graph The graph whose nodes will be managed as disjoint sets.
     */
    explicit DisjointSet(const model::Graph& graph) {
        for (const auto& node : graph.nodes()) {
            parent_[node.id] = node.id;
            rank_[node.id] = 0;
        }
    }

    /**
     * @brief Finds the representative (root) of the set containing the given value.
     * 
     * Uses path compression to flatten the structure, making future searches faster.
     * 
     * @param value The identifier of the node to look up.
     * @return The representative identifier of the component.
     */
    std::string find(const std::string& value) {
        if (parent_[value] == value) {
            return value;
        }
        parent_[value] = find(parent_[value]); // Path compression
        return parent_[value];
    }

    /**
     * @brief Merges two disjoint sets if they are not already connected.
     * 
     * Uses union-by-rank to keep the resulting tree shallow.
     * 
     * @param left The identifier of the first node.
     * @param right The identifier of the second node.
     * @return true if the sets were merged, false if they were already in the same set (cycle detected).
     */
    bool unite(const std::string& left, const std::string& right) {
        std::string leftRoot = find(left);
        std::string rightRoot = find(right);
        if (leftRoot == rightRoot) {
            return false;
        }

        // Union by rank
        if (rank_[leftRoot] < rank_[rightRoot]) {
            std::swap(leftRoot, rightRoot);
        }

        parent_[rightRoot] = leftRoot;
        if (rank_[leftRoot] == rank_[rightRoot]) {
            ++rank_[leftRoot];
        }
        return true;
    }

  private:
    std::unordered_map<std::string, std::string> parent_ {};
    std::unordered_map<std::string, int> rank_ {};
};

}  // namespace

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing "Kruskal".
 */
std::string KruskalAlgorithm::name() const {
    return "Kruskal";
}

/**
 * @brief Provides a summary of the algorithm's goal.
 * @return A string describing the greedy construction of a Minimum Spanning Tree.
 */
std::string KruskalAlgorithm::description() const {
    return "Minimum spanning tree construction by repeatedly choosing the lightest safe edge.";
}

/**
 * @brief Indicates that Kruskal's does not require a specific starting node.
 * @return Always false.
 */
bool KruskalAlgorithm::requiresStartNode() const noexcept {
    return false;
}

/**
 * @brief Checks if the algorithm can run on the provided graph.
 * 
 * Kruskal's algorithm is designed specifically for undirected graphs.
 * 
 * @param graph The graph structure to check.
 * @return true if the graph is undirected, false otherwise.
 */
bool KruskalAlgorithm::supports(const model::Graph& graph) const {
    return !graph.isDirected();
}

/**
 * @brief Specifies that the algorithm requires edge weights to determine the MST.
 * @return Always true.
 */
bool KruskalAlgorithm::requiresWeightedGraph() const noexcept {
    return true;
}

/**
 * @brief Executes Kruskal's algorithm to find the Minimum Spanning Tree (MST).
 * 
 * The algorithm sorts all edges by weight and iterates through them. If an edge 
 * connects two nodes in different components, it is added to the MST. Disjoint-set 
 * management is used to track components and detect cycles in $O(E \log E)$ time.
 * 
 * @param graph The graph to process.
 * @param context Run configuration (unused, as no start node is required).
 * @return An AlgorithmRunResult containing the MST edges, total weight, and visual frames.
 * @throws std::invalid_argument if the graph is directed.
 */
AlgorithmRunResult KruskalAlgorithm::run(const model::Graph& graph, const AlgorithmContext& /* context */) const {
    if (graph.isDirected()) {
        throw std::invalid_argument("Kruskal requires an undirected graph");
    }

    auto workingGraph = graph;
    workingGraph.clearVisualState();

    AlgorithmRunResult result;
    result.algorithmName = name();
    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    // Sort edges by weight
    std::vector<std::size_t> edgeIndices(workingGraph.edgeCount());
    std::iota(edgeIndices.begin(), edgeIndices.end(), 0);
    std::sort(edgeIndices.begin(), edgeIndices.end(), [&workingGraph](const std::size_t left, const std::size_t right) {
        return workingGraph.edgeAt(left).weight < workingGraph.edgeAt(right).weight;
    });

    appendFrame(
        result.frames,
        workingGraph,
        "Initialize Kruskal",
        {"Sort all edges by increasing weight.", "Use a disjoint-set structure to avoid cycles."},
        metrics);

    DisjointSet forest(workingGraph);

    // Process edges in greedy order
    for (const std::size_t edgeIndex : edgeIndices) {
        ++metrics.stepsTaken;
        ++metrics.edgesConsidered;
        ++metrics.comparisons;
        const model::Edge& edge = workingGraph.edgeAt(edgeIndex);
        workingGraph.setEdgeStateByIndex(edgeIndex, model::EdgeState::Active, "candidate");

        // Attempt to merge components
        if (forest.unite(edge.from, edge.to)) {
            // Edge is safe: add to MST
            ++metrics.mstEdgesChosen;
            metrics.totalWeight += edge.weight;
            workingGraph.setEdgeStateByIndex(edgeIndex, model::EdgeState::MinimumSpanningTree, "chosen");
            workingGraph.setNodeState(edge.from, model::NodeState::MinimumSpanningTree, "connected");
            workingGraph.setNodeState(edge.to, model::NodeState::MinimumSpanningTree, "connected");
            result.traversalOrder.push_back(edge.id);
            
            appendFrame(
                result.frames,
                workingGraph,
                "Choose " + edge.id,
                {"The edge connects two different components, so it is safe to include in the MST."},
                metrics);
        } else {
            // Edge would form a cycle: reject
            workingGraph.setEdgeStateByIndex(edgeIndex, model::EdgeState::Traversed, "cycle");
            appendFrame(
                result.frames,
                workingGraph,
                "Reject " + edge.id,
                {"The edge would create a cycle, so it is skipped."},
                metrics);
        }
    }

    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    
    appendFrame(
        result.frames,
        workingGraph,
        "Kruskal complete",
        {"The minimum spanning forest has been constructed."},
        metrics,
        true);
        
    return result;
}

}  // namespace av::algorithm