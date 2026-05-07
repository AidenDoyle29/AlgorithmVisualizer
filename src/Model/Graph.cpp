#include "Model/Graph.h"

#include <algorithm>

namespace av::model {

Graph::Graph(const bool directed) : directed_(directed) {}

Graph::Graph(const Graph& other)
    : directed_(other.directed_),
      nodes_(other.nodes_),
      edges_(other.edges_),
      nodeIndexById_(other.nodeIndexById_),
      edgeIndexById_(other.edgeIndexById_) {}

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

bool Graph::isDirected() const noexcept {
    return directed_;
}

void Graph::setDirected(const bool directed) {
    directed_ = directed;

    for (auto& edge : edges_) {
        edge.directed = directed_;
    }

    notifyObservers("graph direction updated");
}

void Graph::addObserver(GraphObserver* observer) {
    if (observer == nullptr) {
        return;
    }

    if (std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
        observers_.push_back(observer);
    }
}

void Graph::removeObserver(GraphObserver* observer) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

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

void Graph::addNode(std::string id, std::string label, const double x, const double y) {
    addNode(Node {std::move(id), std::move(label), x, y});
}

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

void Graph::addEdge(std::string id, std::string from, std::string to, const double weight, const bool /* directedOverride */) {
    addEdge(Edge {std::move(id), std::move(from), std::move(to), weight, directed_});
}

void Graph::removeEdge(const std::string& edgeId) {
    if (!hasEdge(edgeId)) {
        throw GraphValidationError("cannot remove missing edge: " + edgeId);
    }

    edges_.erase(edges_.begin() + static_cast<std::ptrdiff_t>(edgeIndexById_.at(edgeId)));
    rebuildIndices();
    notifyObservers("edge removed: " + edgeId);
}

bool Graph::hasNode(const std::string& nodeId) const noexcept {
    return nodeIndexById_.contains(nodeId);
}

bool Graph::hasEdge(const std::string& edgeId) const noexcept {
    return edgeIndexById_.contains(edgeId);
}

bool Graph::empty() const noexcept {
    return nodes_.empty();
}

std::size_t Graph::nodeCount() const noexcept {
    return nodes_.size();
}

std::size_t Graph::edgeCount() const noexcept {
    return edges_.size();
}

const Node& Graph::node(const std::string& nodeId) const {
    return nodes_.at(nodeIndexById_.at(nodeId));
}

Node& Graph::node(const std::string& nodeId) {
    return nodes_.at(nodeIndexById_.at(nodeId));
}

const Edge& Graph::edge(const std::string& edgeId) const {
    return edges_.at(edgeIndexById_.at(edgeId));
}

Edge& Graph::edge(const std::string& edgeId) {
    return edges_.at(edgeIndexById_.at(edgeId));
}

const Edge& Graph::edgeAt(const std::size_t index) const {
    return edges_.at(index);
}

Edge& Graph::edgeAt(const std::size_t index) {
    return edges_.at(index);
}

const std::vector<Node>& Graph::nodes() const noexcept {
    return nodes_;
}

const std::vector<Edge>& Graph::edges() const noexcept {
    return edges_;
}

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

std::optional<std::size_t> Graph::findEdgeIndex(const std::string& edgeId) const noexcept {
    const auto iterator = edgeIndexById_.find(edgeId);
    if (iterator == edgeIndexById_.end()) {
        return std::nullopt;
    }
    return iterator->second;
}

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

bool Graph::isWeighted() const noexcept {
    return std::any_of(edges_.begin(), edges_.end(), [](const Edge& edge) { return edge.weight != 1.0; });
}

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

void Graph::setNodeState(const std::string& nodeId, const NodeState state, std::string annotation) {
    Node& currentNode = node(nodeId);
    currentNode.state = state;
    currentNode.annotation = std::move(annotation);
}

void Graph::setEdgeState(const std::string& edgeId, const EdgeState state, std::string annotation) {
    setEdgeStateByIndex(edgeIndexById_.at(edgeId), state, std::move(annotation));
}

void Graph::setEdgeStateByIndex(const std::size_t edgeIndex, const EdgeState state, std::string annotation) {
    Edge& currentEdge = edgeAt(edgeIndex);
    currentEdge.state = state;
    currentEdge.annotation = std::move(annotation);
}

void Graph::clear() {
    nodes_.clear();
    edges_.clear();
    nodeIndexById_.clear();
    edgeIndexById_.clear();
    notifyObservers("graph cleared");
}

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

void Graph::notifyObservers(const std::string& reason) const {
    const GraphEvent event {*this, reason};
    for (GraphObserver* observer : observers_) {
        if (observer != nullptr) {
            observer->onGraphChanged(event);
        }
    }
}

}  // namespace av::model
