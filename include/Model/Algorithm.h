/**
 * @file Algorithm.h
 * @brief Defines the interface Algorithm that will be implemented by various algorithms (e.g. DFSStrategy, BFSStrategy, etc).
 */

#pragma once
#include <vector>
#include "Graph.h"
#include "SimulationState.h"

/**
 * @struct AlgorithmConfig
 * @brief A struct used to store configurable parameters for algorithms (e.g. startNodeID).
 */
struct AlgorithmConfig {
    int startNodeID = -1; //-1 means "Not Provided"
    int targetNodeID = -1; //-1 means "Not Provided"
};

/**
 * @class Algorithm
 * @brief This class holds the core functions needed by algorithms.
 */
class Algorithm {
    public:
        /**
         * @brief A basic constructor/destructor for the Algorithm class
         */
        virtual ~Algorithm() = default;

        /**
         * @brief The core function: run(). All algorithms will implement this function in order to run the algorithm on its respective graph.
         * @param graph The graph that contains its respective nodes and edges that the algorithm will run on
         * @param AlgorithmConfig The configurable struct that each algorithm will implement depending on the given algorithm's specifications. This contains relevant information such as start and target node IDs.
         * @return std::vector<> The vector containing all meaningful states of the graph during the algorithms runtime. This will be used by the view to render the graph, and for the user to be able to move forwards 
         * and backwards in the algorithm's history.
         */
        virtual std::vector<SimulationState> run(Graph &graph, const AlgorithmConfig &config) = 0;
};