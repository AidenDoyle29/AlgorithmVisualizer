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

struct GraphEvent {
    const Graph& graph;
    std::string reason;
};

class GraphObserver {
  public:
    virtual ~GraphObserver() = default;
    virtual void onGraphChanged(const GraphEvent& event) = 0;
};

struct Neighbor {
    std::string nodeId;
    std::size_t edgeIndex {0};
};

class Graph {
  public:
    explicit Graph(bool directed = false);
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);

    [[nodiscard]] bool isDirected() const noexcept;
    void setDirected(bool directed);

    void addObserver(GraphObserver* observer);
    void removeObserver(GraphObserver* observer);

    void addNode(const Node& node);
    void addNode(std::string id, std::string label = {}, double x = 0.0, double y = 0.0);
    void removeNode(const std::string& nodeId);
    void addEdge(const Edge& edge);
    void addEdge(std::string id, std::string from, std::string to, double weight = 1.0, bool directedOverride = false);
    void removeEdge(const std::string& edgeId);

    [[nodiscard]] bool hasNode(const std::string& nodeId) const noexcept;
    [[nodiscard]] bool hasEdge(const std::string& edgeId) const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t nodeCount() const noexcept;
    [[nodiscard]] std::size_t edgeCount() const noexcept;

    [[nodiscard]] const Node& node(const std::string& nodeId) const;
    [[nodiscard]] Node& node(const std::string& nodeId);
    [[nodiscard]] const Edge& edge(const std::string& edgeId) const;
    [[nodiscard]] Edge& edge(const std::string& edgeId);
    [[nodiscard]] const Edge& edgeAt(std::size_t index) const;
    [[nodiscard]] Edge& edgeAt(std::size_t index);
    [[nodiscard]] const std::vector<Node>& nodes() const noexcept;
    [[nodiscard]] const std::vector<Edge>& edges() const noexcept;

    [[nodiscard]] std::vector<Neighbor> neighbors(const std::string& nodeId) const;
    [[nodiscard]] std::vector<std::size_t> outgoingEdgeIndices(const std::string& nodeId) const;
    [[nodiscard]] std::optional<std::size_t> findEdgeIndex(const std::string& edgeId) const noexcept;
    [[nodiscard]] std::optional<std::size_t> findEdgeIndexByEndpoints(const std::string& from, const std::string& to) const noexcept;
    [[nodiscard]] std::string oppositeNode(std::size_t edgeIndex, const std::string& nodeId) const;
    [[nodiscard]] bool isWeighted() const noexcept;

    void clearVisualState();
    void setNodeState(const std::string& nodeId, NodeState state, std::string annotation = {});
    void setEdgeState(const std::string& edgeId, EdgeState state, std::string annotation = {});
    void setEdgeStateByIndex(std::size_t edgeIndex, EdgeState state, std::string annotation = {});
    void clear();

  private:
    bool directed_ {false};
    std::vector<Node> nodes_ {};
    std::vector<Edge> edges_ {};
    std::unordered_map<std::string, std::size_t> nodeIndexById_ {};
    std::unordered_map<std::string, std::size_t> edgeIndexById_ {};
    std::vector<GraphObserver*> observers_ {};

    void rebuildIndices();
    void notifyObservers(const std::string& reason) const;
};

class GraphValidationError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

}  // namespace av::model

#endif
