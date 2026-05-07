#include "Model/Graph.h"

#include <algorithm>

namespace av::model {

/**
 * @brief Constructs a graph with specified directionality.
 * 
 * @param directed If true, creates a directed graph; if false, creates an undirected graph.
 */
Graph::Graph(const bool directed) : directed_(directed) {}

Graph::Graph(const Graph& other)
    : directed_(other.directed_),
      nodes_(other.nodes_),
      edges_(other.edges_),
      nodeIndexById_(other.nodeIndexById_),
      edgeIndexById_(other.edgeIndexById_) {}

/**
 * @brief Assignment operator for copying graph state.
 * 
 * Copies all nodes, edges, and indices from the other graph. Clears observers
 * from the destination graph. Uses copy-and-swap semantics for safety.
 * 
 * @param other The graph to copy from.
 * @return Reference to this graph after assignment.
 */
Graph& Graph::operator=(const Graph& other) {
    if (this == &other) {
        return *this;
    }

    directed_ = other.directed_;
    nodes_ = other.nodes_;
    edges_ = other.edges_;
    nodeIndexById_ = other.nodeIndexById_;
    edgeIndexById_ = other.edgeIndexById_;
    observers_.clear();
    return *this;
}

/**
 * @brief Determines if this graph is directed.
 * 
 * @return true if the graph is directed, false if undirected.
 */
bool Graph::isDirected() const noexcept {
    return directed_;
}

/**
 * @brief Sets the directionality of the graph.
 * 
 * Updates all edges in the graph to match the new directionality setting.
 * Notifies observers of the change.
 * 
 * @param directed If true, converts to directed; if false, converts to undirected.
 */
void Graph::setDirected(const bool directed) {
    directed_ = directed;

    for (auto& edge : edges_) {
        edge.directed = directed_;
    }

    notifyObservers("graph direction updated");
}

/**
 * @brief Registers an observer to be notified of graph changes.
 * 
 * Prevents duplicate observers from being registered. Does nothing if observer is null.
 * 
 * @param observer Pointer to a GraphObserver instance to register.
 */
void Graph::addObserver(GraphObserver* observer) {
    if (observer == nullptr) {
        return;
    }

    if (std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
        observers_.push_back(observer);
    }
}

/**
 * @brief Unregisters an observer from being notified of graph changes.
 * 
 * Safely removes the observer from the list. Does nothing if observer is not registered.
 * 
 * @param observer Pointer to the GraphObserver to remove.
 */
void Graph::removeObserver(GraphObserver* observer) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

/**
 * @brief Adds a node to the graph.
 * 
 * Validates that the node ID is unique and non-empty. Updates internal indices
 * and notifies observers of the addition.
 * 
 * @param node The Node object to add.
 * @throws GraphValidationError if node ID is empty or duplicate.
 */
void Graph::addNode(const Node& node) {
    if (node.id.empty()) {
        throw GraphValidationError("node id cannot be empty");
    }

    if (hasNode(node.id)) {
        throw GraphValidationError("duplicate node id: " + node.id);
    }

    nodes_.push_back(node);
    nodeIndexById_[node.id] = nodes_.size() - 1;
    notifyObservers("node added: " + node.id);
}

/**
 * @brief Adds a node with specified parameters.
 * 
 * Convenience overload that constructs a Node from individual parameters.
 * 
 * @param id Unique identifier for the node.
 * @param label Display label for the node (can be empty).
 * @param x X coordinate for visualization.
 * @param y Y coordinate for visualization.
 * @throws GraphValidationError if node ID is empty or duplicate.
 */
void Graph::addNode(std::string id, std::string label, const double x, const double y) {
    addNode(Node {std::move(id), std::move(label), x, y});
}

/**
 * @brief Removes a node and all its associated edges from the graph.
 * 
 * Also removes any edges connected to the node. Updates indices and notifies observers.
 * 
 * @param nodeId The ID of the node to remove.
 * @throws GraphValidationError if the node does not exist.
 */
void Graph::removeNode(const std::string& nodeId) {
    if (!hasNode(nodeId)) {
        throw GraphValidationError("cannot remove missing node: " + nodeId);
    }

    nodes_.erase(nodes_.begin() + static_cast<std::ptrdiff_t>(nodeIndexById_.at(nodeId)));
    edges_.erase(
        std::remove_if(
            edges_.begin(),
            edges_.end(),
            [&nodeId](const Edge& edge) { return edge.from == nodeId || edge.to == nodeId; }),
        edges_.end());
    rebuildIndices();
    notifyObservers("node removed: " + nodeId);
}

/**
 * @brief Adds an edge to the graph.
 * 
 * Validates that both endpoints exist and edge ID is unique. Enforces graph directionality.
 * Updates internal indices and notifies observers.
 * 
 * @param edge The Edge object to add.
 * @throws GraphValidationError if edge ID is empty, duplicate, or endpoints don't exist.
 */
void Graph::addEdge(const Edge& edge) {
    if (edge.id.empty()) {
        throw GraphValidationError("edge id cannot be empty");
    }

    if (hasEdge(edge.id)) {
        throw GraphValidationError("duplicate edge id: " + edge.id);
    }

    if (!hasNode(edge.from) || !hasNode(edge.to)) {
        throw GraphValidationError("edge endpoints must exist before adding edge " + edge.id);
    }

    Edge normalized = edge;
    normalized.directed = directed_;
    edges_.push_back(normalized);
    edgeIndexById_[normalized.id] = edges_.size() - 1;
    notifyObservers("edge added: " + normalized.id);
}

/**
 * @brief Adds an edge with specified parameters.
 * 
 * Convenience overload that constructs an Edge from individual parameters.
 * 
 * @param id Unique identifier for the edge.
 * @param from ID of the source node.
 * @param to ID of the destination node.
 * @param weight Edge weight for weighted graph algorithms (default 1.0).
 * @throws GraphValidationError if edge ID is empty, duplicate, or endpoints don't exist.
 */
void Graph::addEdge(std::string id, std::string from, std::string to, const double weight, const bool /* directedOverride */) {
    addEdge(Edge {std::move(id), std::move(from), std::move(to), weight, directed_});
}

/**
 * @brief Removes an edge from the graph.
 * 
 * Updates indices and notifies observers. Connected nodes remain in the graph.
 * 
 * @param edgeId The ID of the edge to remove.
 * @throws GraphValidationError if the edge does not exist.
 */
void Graph::removeEdge(const std::string& edgeId) {
    if (!hasEdge(edgeId)) {
        throw GraphValidationError("cannot remove missing edge: " + edgeId);
    }

    edges_.erase(edges_.begin() + static_cast<std::ptrdiff_t>(edgeIndexById_.at(edgeId)));
    rebuildIndices();
    notifyObservers("edge removed: " + edgeId);
}

/**
 * @brief Checks if a node exists in the graph.
 * 
 * @param nodeId The ID to check.
 * @return true if a node with the given ID exists, false otherwise.
 */
bool Graph::hasNode(const std::string& nodeId) const noexcept {
    return nodeIndexById_.contains(nodeId);
}

/**
 * @brief Checks if an edge exists in the graph.
 * 
 * @param edgeId The ID to check.
 * @return true if an edge with the given ID exists, false otherwise.
 */
bool Graph::hasEdge(const std::string& edgeId) const noexcept {
    return edgeIndexById_.contains(edgeId);
}

/**
 * @brief Determines if the graph is empty.
 * 
 * @return true if the graph has no nodes, false otherwise.
 */
bool Graph::empty() const noexcept {
    return nodes_.empty();
}

/**
 * @brief Gets the number of nodes in the graph.
 * 
 * @return The count of nodes.
 */
std::size_t Graph::nodeCount() const noexcept {
    return nodes_.size();
}

/**
 * @brief Gets the number of edges in the graph.
 * 
 * @return The count of edges.
 */
std::size_t Graph::edgeCount() const noexcept {
    return edges_.size();
}

/**
 * @brief Retrieves a node by ID (const access).
 * 
 * @param nodeId The ID of the node to retrieve.
 * @return Const reference to the node.
 * @throws std::out_of_range if the node does not exist.
 */
const Node& Graph::node(const std::string& nodeId) const {
    return nodes_.at(nodeIndexById_.at(nodeId));
}

/**
 * @brief Retrieves a node by ID (mutable access).
 * 
 * @param nodeId The ID of the node to retrieve.
 * @return Mutable reference to the node.
 * @throws std::out_of_range if the node does not exist.
 */
Node& Graph::node(const std::string& nodeId) {
    return nodes_.at(nodeIndexById_.at(nodeId));
}

/**
 * @brief Retrieves an edge by ID (const access).
 * 
 * @param edgeId The ID of the edge to retrieve.
 * @return Const reference to the edge.
 * @throws std::out_of_range if the edge does not exist.
 */
const Edge& Graph::edge(const std::string& edgeId) const {
    return edges_.at(edgeIndexById_.at(edgeId));
}

/**
 * @brief Retrieves an edge by ID (mutable access).
 * 
 * @param edgeId The ID of the edge to retrieve.
 * @return Mutable reference to the edge.
 * @throws std::out_of_range if the edge does not exist.
 */
Edge& Graph::edge(const std::string& edgeId) {
    return edges_.at(edgeIndexById_.at(edgeId));
}

/**
 * @brief Retrieves an edge by index (const access).
 * 
 * @param index The position of the edge in the edges vector.
 * @return Const reference to the edge.
 * @throws std::out_of_range if index is out of bounds.
 */
const Edge& Graph::edgeAt(const std::size_t index) const {
    return edges_.at(index);
}

/**
 * @brief Retrieves an edge by index (mutable access).
 * 
 * @param index The position of the edge in the edges vector.
 * @return Mutable reference to the edge.
 * @throws std::out_of_range if index is out of bounds.
 */
Edge& Graph::edgeAt(const std::size_t index) {
    return edges_.at(index);
}

/**
 * @brief Gets all nodes in the graph.
 * 
 * @return Const reference to the vector of nodes.
 */
const std::vector<Node>& Graph::nodes() const noexcept {
    return nodes_;
}

/**
 * @brief Gets all edges in the graph.
 * 
 * @return Const reference to the vector of edges.
 */
const std::vector<Edge>& Graph::edges() const noexcept {
    return edges_;
}

/**
 * @brief Finds all neighbors of a given node.
 * 
 * For directed graphs, returns nodes reachable by outgoing edges.
 * For undirected graphs, returns all nodes connected by any edge.
 * Returns Neighbor structs containing both node IDs and edge indices.
 * 
 * @param nodeId The ID of the node to find neighbors for.
 * @return Vector of Neighbor structs.
 * @throws GraphValidationError if the node does not exist.
 */
std::vector<Neighbor> Graph::neighbors(const std::string& nodeId) const {
    if (!hasNode(nodeId)) {
        throw GraphValidationError("missing node: " + nodeId);
    }

    std::vector<Neighbor> results;
    for (std::size_t index = 0; index < edges_.size(); ++index) {
        const Edge& edge = edges_[index];
        if (edge.from == nodeId) {
            results.push_back(Neighbor {edge.to, index});
        } else if (!edge.directed && edge.to == nodeId) {
            results.push_back(Neighbor {edge.from, index});
        }
    }
    return results;
}

/**
 * @brief Finds indices of all outgoing edges from a node.
 * 
 * For directed graphs, finds edges where from == nodeId.
 * For undirected graphs, finds all edges connected to the node.
 * 
 * @param nodeId The ID of the source node.
 * @return Vector of edge indices.
 * @throws GraphValidationError if the node does not exist.
 */
std::vector<std::size_t> Graph::outgoingEdgeIndices(const std::string& nodeId) const {
    if (!hasNode(nodeId)) {
        throw GraphValidationError("missing node: " + nodeId);
    }

    std::vector<std::size_t> indices;
    for (std::size_t index = 0; index < edges_.size(); ++index) {
        const Edge& edge = edges_[index];
        if (edge.from == nodeId || (!edge.directed && edge.to == nodeId)) {
            indices.push_back(index);
        }
    }
    return indices;
}

/**
 * @brief Finds the index of an edge by its ID.
 * 
 * @param edgeId The ID to search for.
 * @return The index of the edge if found, std::nullopt otherwise.
 */
std::optional<std::size_t> Graph::findEdgeIndex(const std::string& edgeId) const noexcept {
    const auto iterator = edgeIndexById_.find(edgeId);
    if (iterator == edgeIndexById_.end()) {
        return std::nullopt;
    }
    return iterator->second;
}

/**
 * @brief Finds an edge between two nodes.
 * 
 * For directed graphs, searches for edge (from -> to).
 * For undirected graphs, searches in both directions.
 * 
 * @param from ID of the source node.
 * @param to ID of the destination node.
 * @return The index of the edge if found, std::nullopt otherwise.
 */
std::optional<std::size_t> Graph::findEdgeIndexByEndpoints(const std::string& from, const std::string& to) const noexcept {
    for (std::size_t index = 0; index < edges_.size(); ++index) {
        const Edge& edge = edges_[index];
        if (edge.from == from && edge.to == to) {
            return index;
        }
        if (!edge.directed && edge.from == to && edge.to == from) {
            return index;
        }
    }
    return std::nullopt;
}

/**
 * @brief Gets the other endpoint of an edge.
 * 
 * Given an edge index and one endpoint, returns the other endpoint.
 * For directed edges, returns the destination if given the source.
 * For undirected edges, returns whichever endpoint is not the given nodeId.
 * 
 * @param edgeIndex Index of the edge.
 * @param nodeId One endpoint of the edge.
 * @return The other endpoint's ID.
 * @throws GraphValidationError if nodeId is not an endpoint of the edge.
 */
std::string Graph::oppositeNode(const std::size_t edgeIndex, const std::string& nodeId) const {
    const Edge& currentEdge = edgeAt(edgeIndex);
    if (currentEdge.from == nodeId) {
        return currentEdge.to;
    }
    if (!currentEdge.directed && currentEdge.to == nodeId) {
        return currentEdge.from;
    }
    throw GraphValidationError("node " + nodeId + " is not connected to edge " + currentEdge.id);
}

/**
 * @brief Determines if the graph has weighted edges.
 * 
 * Returns true if any edge has a weight other than the default (1.0).
 * 
 * @return true if the graph is weighted, false if all edges have weight 1.0.
 */
bool Graph::isWeighted() const noexcept {
    return std::any_of(edges_.begin(), edges_.end(), [](const Edge& edge) { return edge.weight != 1.0; });
}

/**
 * @brief Clears all visual state information.
 * 
 * Resets all nodes and edges to Normal state and clears annotations.
 * Used to prepare the graph for a new visualization.
 */
void Graph::clearVisualState() {
    for (auto& currentNode : nodes_) {
        currentNode.state = NodeState::Normal;
        currentNode.annotation.clear();
    }

    for (auto& currentEdge : edges_) {
        currentEdge.state = EdgeState::Normal;
        currentEdge.annotation.clear();
    }
}

/**
 * @brief Sets the visual state and annotation for a node.
 * 
 * Updates a node's state (for coloring/highlighting) and optional annotation text.
 * 
 * @param nodeId ID of the node to update.
 * @param state The new NodeState value.
 * @param annotation Optional descriptive text to display with the node.
 * @throws std::out_of_range if the node does not exist.
 */
void Graph::setNodeState(const std::string& nodeId, const NodeState state, std::string annotation) {
    Node& currentNode = node(nodeId);
    currentNode.state = state;
    currentNode.annotation = std::move(annotation);
}

/**
 * @brief Sets the visual state and annotation for an edge by ID.
 * 
 * Updates an edge's state (for coloring/highlighting) and optional annotation text.
 * 
 * @param edgeId ID of the edge to update.
 * @param state The new EdgeState value.
 * @param annotation Optional descriptive text to display with the edge.
 * @throws std::out_of_range if the edge does not exist.
 */
void Graph::setEdgeState(const std::string& edgeId, const EdgeState state, std::string annotation) {
    setEdgeStateByIndex(edgeIndexById_.at(edgeId), state, std::move(annotation));
}

/**
 * @brief Sets the visual state and annotation for an edge by index.
 * 
 * Updates an edge's state (for coloring/highlighting) and optional annotation text.
 * 
 * @param edgeIndex Position of the edge in the edges vector.
 * @param state The new EdgeState value.
 * @param annotation Optional descriptive text to display with the edge.
 * @throws std::out_of_range if the index is out of bounds.
 */
void Graph::setEdgeStateByIndex(const std::size_t edgeIndex, const EdgeState state, std::string annotation) {
    Edge& currentEdge = edgeAt(edgeIndex);
    currentEdge.state = state;
    currentEdge.annotation = std::move(annotation);
}

/**
 * @brief Removes all nodes and edges from the graph.
 * 
 * Clears internal data structures and notifies observers.
 */
void Graph::clear() {
    nodes_.clear();
    edges_.clear();
    nodeIndexById_.clear();
    edgeIndexById_.clear();
    notifyObservers("graph cleared");
}

/**
 * @brief Rebuilds internal ID-to-index mappings.
 * 
 * Called internally after structural modifications (removals) to ensure
 * nodeIndexById_ and edgeIndexById_ are synchronized with current node/edge positions.
 */
void Graph::rebuildIndices() {
    nodeIndexById_.clear();
    edgeIndexById_.clear();

    for (std::size_t index = 0; index < nodes_.size(); ++index) {
        nodeIndexById_[nodes_[index].id] = index;
    }

    for (std::size_t index = 0; index < edges_.size(); ++index) {
        edgeIndexById_[edges_[index].id] = index;
    }
}

/**
 * @brief Notifies all registered observers of a graph change.
 * 
 * Creates a GraphEvent with the reason and broadcasts it to all observers.
 * Safely handles null observer pointers.
 * 
 * @param reason A description of what changed in the graph.
 */
void Graph::notifyObservers(const std::string& reason) const {
    const GraphEvent event {*this, reason};
    for (GraphObserver* observer : observers_) {
        if (observer != nullptr) {
            observer->onGraphChanged(event);
        }
    }
}

}  // namespace av::model
