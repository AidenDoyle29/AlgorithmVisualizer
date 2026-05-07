/**
 * @file Graph.h
 * @brief Defines the core mathematical Graph model and its Observer interfaces.
 *
 * This file implements the central data structure of the application. It acts as 
 * both a mathematical adjacency mapping for pathfinding algorithms and a state 
 * container for the visual rendering engine. It utilizes the Observer pattern 
 * to broadcast state changes dynamically.
 */

#ifndef ALGORITHM_VISUALIZER_MODEL_GRAPH_H
#define ALGORITHM_VISUALIZER_MODEL_GRAPH_H

#include "Model/Edge.h"
#include "Model/Node.h"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::model {

class Graph;

/**
 * @struct GraphEvent
 * @brief A broadcast package containing context about a graph mutation.
 */
struct GraphEvent {
    const Graph& graph;     /**< A read-only reference to the updated graph. */
    std::string reason;     /**< A description of the change (e.g., "NodeAdded", "VisualStateCleared"). */
};

/**
 * @class GraphObserver
 * @brief Interface for objects that need to react to Graph state changes (Observer Pattern).
 *
 * Typically implemented by the View layer (or mediators) to automatically trigger 
 * a screen redraw whenever the underlying mathematical model is modified.
 */
class GraphObserver {
  public:
    virtual ~GraphObserver() = default;

    /**
     * @brief Called by the observed Graph whenever its internal state mutates.
     * @param event The payload containing the new graph state and mutation reason.
     */
    virtual void onGraphChanged(const GraphEvent& event) = 0;
};

/**
 * @struct Neighbor
 * @brief Represents an adjacency relationship from a specific node.
 */
struct Neighbor {
    std::string nodeId;         /**< The ID of the adjacent destination node. */
    std::size_t edgeIndex {0};  /**< The internal memory index of the edge connecting them. */
};

/**
 * @class Graph
 * @brief The primary data structure encapsulating nodes, edges, and their relationships.
 *
 * The Graph maintains contiguous memory storage (std::vector) for fast iteration 
 * by algorithms, paired with hash maps (std::unordered_map) for O(1) ID lookups. 
 * It strictly encapsulates its data, forcing all mutations to pass through its API 
 * so it can notify attached observers safely.
 */
class Graph {
  public:
    /**
     * @brief Constructs an empty graph.
     * @param directed If true, all edges are treated as one-way by default.
     */
    explicit Graph(bool directed = false);
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);

    [[nodiscard]] bool isDirected() const noexcept;
    void setDirected(bool directed);

    /**
     * @brief Subscribes an observer to receive mutation events.
     * @param observer Pointer to the implementing observer.
     */
    void addObserver(GraphObserver* observer);

    /**
     * @brief Unsubscribes an observer from mutation events.
     * @param observer Pointer to the observer to remove.
     */
    void removeObserver(GraphObserver* observer);

    /**
     * @brief Injects a fully constructed Node entity into the graph.
     * @param node The Node object to copy into storage.
     */
    void addNode(const Node& node);

    /**
     * @brief Constructs and injects a Node directly into the graph.
     */
    void addNode(std::string id, std::string label = {}, double x = 0.0, double y = 0.0);
    
    /**
     * @brief Removes a node and automatically deletes all connected edges.
     * @param nodeId The unique ID of the node to destroy.
     */
    void removeNode(const std::string& nodeId);

    /**
     * @brief Injects a fully constructed Edge entity into the graph.
     * @param edge The Edge object to copy into storage.
     */
    void addEdge(const Edge& edge);

    /**
     * @brief Constructs and injects an Edge directly into the graph.
     */
    void addEdge(std::string id, std::string from, std::string to, double weight = 1.0, bool directedOverride = false);
    void removeEdge(const std::string& edgeId);

    [[nodiscard]] bool hasNode(const std::string& nodeId) const noexcept;
    [[nodiscard]] bool hasEdge(const std::string& edgeId) const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t nodeCount() const noexcept;
    [[nodiscard]] std::size_t edgeCount() const noexcept;

    // --- Fast Accessors ---
    [[nodiscard]] const Node& node(const std::string& nodeId) const;
    [[nodiscard]] Node& node(const std::string& nodeId);
    [[nodiscard]] const Edge& edge(const std::string& edgeId) const;
    [[nodiscard]] Edge& edge(const std::string& edgeId);
    [[nodiscard]] const Edge& edgeAt(std::size_t index) const;
    [[nodiscard]] Edge& edgeAt(std::size_t index);
    [[nodiscard]] const std::vector<Node>& nodes() const noexcept;
    [[nodiscard]] const std::vector<Edge>& edges() const noexcept;

    // --- Traversal & Adjacency Helpers ---
    
    /**
     * @brief Retrieves all reachable adjacent nodes from a starting point.
     * @param nodeId The source node ID.
     * @return std::vector<Neighbor> A list of connected destinations and their connecting edges.
     */
    [[nodiscard]] std::vector<Neighbor> neighbors(const std::string& nodeId) const;
    [[nodiscard]] std::vector<std::size_t> outgoingEdgeIndices(const std::string& nodeId) const;
    [[nodiscard]] std::optional<std::size_t> findEdgeIndex(const std::string& edgeId) const noexcept;
    [[nodiscard]] std::optional<std::size_t> findEdgeIndexByEndpoints(const std::string& from, const std::string& to) const noexcept;
    
    /**
     * @brief Finds the ID of the node on the other side of an edge.
     */
    [[nodiscard]] std::string oppositeNode(std::size_t edgeIndex, const std::string& nodeId) const;
    
    /**
     * @brief Checks if any edge in the graph has a weight other than 1.0.
     */
    [[nodiscard]] bool isWeighted() const noexcept;

    // --- Visual State Mutators ---

    /**
     * @brief Resets all Node and Edge states to Normal and clears annotations.
     * * Typically called by the Controller before starting a new algorithm run 
     * to guarantee a clean visual slate.
     */
    void clearVisualState();
    void setNodeState(const std::string& nodeId, NodeState state, std::string annotation = {});
    void setEdgeState(const std::string& edgeId, EdgeState state, std::string annotation = {});
    void setEdgeStateByIndex(std::size_t edgeIndex, EdgeState state, std::string annotation = {});
    
    /**
     * @brief Completely empties the graph of all nodes and edges.
     */
    void clear();

  private:
    bool directed_ {false};
    std::vector<Node> nodes_ {};
    std::vector<Edge> edges_ {};
    std::unordered_map<std::string, std::size_t> nodeIndexById_ {};
    std::unordered_map<std::string, std::size_t> edgeIndexById_ {};
    std::vector<GraphObserver*> observers_ {};

    /**
     * @brief Synchronizes the O(1) hash maps with the contiguous vectors after a deletion.
     */
    void rebuildIndices();
    void notifyObservers(const std::string& reason) const;
};

/**
 * @class GraphValidationError
 * @brief Custom exception thrown when illegal operations are attempted (e.g., adding an edge between non-existent nodes).
 */
class GraphValidationError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

}  // namespace av::model

#endif
