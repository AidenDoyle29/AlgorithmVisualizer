/**
 * @file AlgorithmFactory.h
 * @brief Defines the factory class responsible for managing and retrieving algorithm strategies.
 * * This file implements a combination of the Factory and Flyweight design patterns. 
 * It registers all available concrete algorithms (e.g., BFS, DFS) and dispenses 
 * shared, immutable pointers to them upon request, ensuring memory efficiency.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_FACTORY_H
#define ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_FACTORY_H

#include "Algorithm/Algorithm.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::algorithm {

/**
 * @class AlgorithmFactory
 * @brief A centralized registry and dispenser for all pathfinding and traversal algorithms.
 * * The factory pre-populates or lazily loads instances of `IAlgorithm`. Because algorithms 
 * are entirely stateless (all methods are const), the factory safely shares the same 
 * instance across multiple requests using `std::shared_ptr<const IAlgorithm>`.
 */
class AlgorithmFactory {
  public:
    /**
     * @brief Constructs the AlgorithmFactory and populates the internal registry.
     * * The constructor is typically responsible for registering the concrete strategy 
     * classes (like BreadthFirstSearch and DijkstraStrategy) into the internal map.
     */
    AlgorithmFactory();

    /**
     * @brief Retrieves a list of the human-readable names of all registered algorithms.
     * * This is primarily used by the View/Controller layers to dynamically populate 
     * the user interface (e.g., dropdown menus) without hardcoding algorithm names.
     * * @return std::vector<std::string> A list of available algorithm names.
     */
    [[nodiscard]] std::vector<std::string> availableAlgorithms() const;

    /**
     * @brief Retrieves a shared, immutable instance of the requested algorithm.
     * * @param name The exact string name of the algorithm (must match a string returned by availableAlgorithms()).
     * @return std::shared_ptr<const IAlgorithm> A shared pointer to the stateless algorithm instance.
     * Returns nullptr if the requested algorithm name is not found in the registry.
     */
    [[nodiscard]] std::shared_ptr<const IAlgorithm> get(const std::string& name) const;

  private:
    /**
     * @brief The internal registry mapping algorithm names to their shared instances.
     */
    std::unordered_map<std::string, std::shared_ptr<const IAlgorithm>> algorithms_ {};
};

}  // namespace av::algorithm

#endif