#pragma once
#include <vector>
#include <unordered_map>
#include "Node.h"
#include "Edge.h"

class Graph {
public:
    void addNode(int id, float x = 0, float y = 0);
    void addEdge(int from, int to, float weight = 1.0f);

    const std::unordered_map<int, Node>& getNodes() const { return nodes; }
    const std::vector<Edge>& getEdges() const { return edges; }
    const std::vector<int>& getNeighbors(int nodeID) const;

private:
    std::unordered_map<int, Node> nodes;
    std::vector<Edge> edges;
    std::unordered_map<int, std::vector<int> > adjacencyList;
};
