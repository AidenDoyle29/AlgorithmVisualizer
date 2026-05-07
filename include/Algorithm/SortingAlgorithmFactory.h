/**
 * @file SortingAlgorithmFactory.h
 * @brief Defines the factory class responsible for managing and retrieving sorting strategies.
 *
 * This file implements a combination of the Factory and Flyweight design patterns 
 * specifically tailored for array sorting algorithms. By cleanly separating this from 
 * the graph algorithm factory, the architecture ensures strict type safety.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_FACTORY_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_FACTORY_H

#include "Algorithm/SortingAlgorithm.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::algorithm {

/**
 * @class SortingAlgorithmFactory
 * @brief A centralized registry and dispenser for all array sorting algorithms.
 *
 * The SortingAlgorithmFactory pre-populates or lazily loads instances of ISortingAlgorithm. 
 * Because sorting algorithms are entirely stateless (all methods are const), the factory 
 * safely shares the same instance across multiple requests using shared, immutable pointers. 
 * This ensures high memory efficiency across the visualizer application.
 */
class SortingAlgorithmFactory {
  public:
    /**
     * @brief Constructs the SortingAlgorithmFactory and populates the internal registry.
     *
     * The constructor is responsible for registering the concrete sorting strategy 
     * classes (e.g., BubbleSortAlgorithm, QuickSortAlgorithm) into the internal map.
     */
    SortingAlgorithmFactory();

    /**
     * @brief Retrieves a list of the human-readable names of all registered sorting algorithms.
     *
     * This is primarily used by the View layer to dynamically populate the user interface 
     * (e.g., dropdown menus) without hardcoding the available sorting methods.
     *
     * @return std::vector<std::string> A list of available sorting algorithm names.
     */
    [[nodiscard]] std::vector<std::string> availableAlgorithms() const;

    /**
     * @brief Retrieves a shared, immutable instance of the requested sorting algorithm.
     *
     * @param name The exact string name of the sorting algorithm.
     * @return std::shared_ptr<const ISortingAlgorithm> A shared pointer to the stateless algorithm instance.
     * Returns nullptr if the requested algorithm name is not found in the registry.
     */
    [[nodiscard]] std::shared_ptr<const ISortingAlgorithm> get(const std::string& name) const;

  private:
    /**
     * @brief The internal registry mapping sorting algorithm names to their shared instances.
     */
    std::unordered_map<std::string, std::shared_ptr<const ISortingAlgorithm>> algorithms_ {};
};

}  // namespace av::algorithm

#endif