#include "Graph.h"

void Graph::addNode(int id, float x, float y) {
    if (nodes.find(id) == nodes.end()) {
        nodes.emplace(id, Node(id, x, y));
        adjacencyList[id] = std::vector<int>();
    }
}

void Graph::addEdge(int from, int to, float weight) {
    // Ensure nodes exist
    addNode(from);
    addNode(to);
    
    edges.emplace_back(from, to, weight);
    adjacencyList[from].push_back(to);
    // For undirected graph, add: adjacencyList[to].push_back(from);
}

const std::vector<int>& Graph::getNeighbors(int nodeID) const {
    static const std::vector<int> empty;
    auto it = adjacencyList.find(nodeID);
    if (it != adjacencyList.end()) {
        return it->second;
    }
    return empty;
}