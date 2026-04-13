#include <gtest/gtest.h>
#include "Graph.h"
#include "BFSStrategy.h"

class BFSTest : public ::testing::Test {
protected:
    Graph g;
    BFSStrategy bfs;

    void SetUp() override {
        // Create a simple path: 0 -> 1 -> 2
        g.addNode(0);
        g.addNode(1);
        g.addNode(2);
        g.addEdge(0, 1);
        g.addEdge(1, 2);
    }
};

TEST_F(BFSTest, BFSRunsCorrectly) {
    AlgorithmConfig config;
    config.startNodeID = 0;

    auto states = bfs.run(g, config);

    // Path 0->1->2 should have multiple states:
    // 1. Init (0 FRONTIER)
    // 2. Visit 0 (0 CURRENT)
    // 3. Neighbors of 0 (1 FRONTIER)
    // 4. Finished 0 (0 VISIT)
    // ...
    ASSERT_FALSE(states.empty());
    
    // Check if start node is handled
    bool foundStartVisited = false;
    for (const auto& state : states) {
        if (state.nodeStatuses.at(0) == NodeStatus::VISIT) {
            foundStartVisited = true;
            break;
        }
    }
    EXPECT_TRUE(foundStartVisited);
}

TEST_F(BFSTest, HandlesInvalidStartNode) {
    AlgorithmConfig config;
    config.startNodeID = 999;

    auto states = bfs.run(g, config);
    EXPECT_TRUE(states.empty());
}

TEST_F(BFSTest, VerifiesStateTransitions) {
    AlgorithmConfig config;
    config.startNodeID = 0;

    auto states = bfs.run(g, config);

    // Track the statuses of the start node (0) throughout the simulation
    std::vector<NodeStatus> lifecycle;
    for (const auto& state : states) {
        NodeStatus status = state.nodeStatuses.at(0);
        if (lifecycle.empty() || lifecycle.back() != status) {
            lifecycle.push_back(status);
        }
    }

    // Expected lifecycle: FRONTIER -> CURRENT -> VISIT
    ASSERT_EQ(lifecycle.size(), 3);
    EXPECT_EQ(lifecycle[0], NodeStatus::FRONTIER);
    EXPECT_EQ(lifecycle[1], NodeStatus::CURRENT);
    EXPECT_EQ(lifecycle[2], NodeStatus::VISIT);
    
    // Also verify step numbers are incrementing
    for (size_t i = 1; i < states.size(); ++i) {
        EXPECT_EQ(states[i].stepNumber, states[i-1].stepNumber + 1);
    }
}
