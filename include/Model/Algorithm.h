#pragma once
#include <vector>
#include "Graph.h"
#include "SimulationState.h"

struct AlgorithmConfig {
    int startNodeID = -1; //-1 means "Not Provided"
    int targetNodeID = -1; //-1 means "Not Provided"
};

class Algorithm {
    public:
        virtual ~Algorithm() = default;
        virtual std::vector<SimulationState> run(Graph &graph, const AlgorithmConfig &config) = 0;
};