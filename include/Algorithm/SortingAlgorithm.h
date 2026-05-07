/**
 * @file SortingAlgorithm.h
 * @brief Defines the core interface for all array sorting algorithms.
 *
 * This file establishes a distinct architectural boundary for sorting strategies, 
 * separating them from graph traversal algorithms to ensure type safety and 
 * appropriate data handling (arrays vs. mathematical graphs).
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_H

#include "Algorithm/Algorithm.h"

#include <string>
#include <vector>

namespace av::algorithm {

/**
 * @class ISortingAlgorithm
 * @brief The abstract base class for all visualizable sorting algorithms (Strategy Pattern).
 *
 * The ISortingAlgorithm interface guarantees that all concrete sorting strategies 
 * (e.g., Bubble Sort, Quick Sort) adhere to a strict contract. It is purposefully 
 * decoupled from the graph-based IAlgorithm to ensure the run method strictly accepts 
 * a vector of integers. Concrete implementations will process this vector and generate 
 * an array-specific history of PlaybackFrames for the rendering engine.
 */
class ISortingAlgorithm {
  public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived strategy classes.
     */
    virtual ~ISortingAlgorithm() = default;

    /**
     * @brief Retrieves the human-readable name of the sorting algorithm.
     * @return std::string The algorithm's name (e.g., "Merge Sort").
     */
    [[nodiscard]] virtual std::string name() const = 0;

    /**
     * @brief Retrieves a short explanation of how the sorting algorithm operates.
     * @return std::string A brief description of the algorithm's mechanics.
     */
    [[nodiscard]] virtual std::string description() const = 0;

    /**
     * @brief The core execution function for array sorting.
     *
     * This method processes the provided dataset to completion. It records step-by-step 
     * history via Memento frames tailored for array visualization (updating ArrayCellStates) 
     * and compiles the final statistical metrics.
     *
     * @param values The unsorted array of integers to be processed.
     * @return AlgorithmRunResult A complete package containing the array playback frames and final metrics.
     */
    [[nodiscard]] virtual AlgorithmRunResult run(const std::vector<int>& values) const = 0;
};

}  // namespace av::algorithm

#endif