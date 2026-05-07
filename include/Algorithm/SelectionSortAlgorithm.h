/**
 * @file SelectionSortAlgorithm.h
 * @brief Defines the Selection Sort algorithm strategy.
 *
 * This file implements the concrete strategy for the Selection Sort algorithm, 
 * conforming to the ISortingAlgorithm interface to visualize array manipulations 
 * through iterative minimum-value selection.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SELECTION_SORT_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SELECTION_SORT_ALGORITHM_H

#include "Algorithm/SortingAlgorithm.h"

namespace av::algorithm {

/**
 * @class SelectionSortAlgorithm
 * @brief Executes an iterative Selection Sort on a collection of integers.
 *
 * The SelectionSortAlgorithm class is a concrete implementation of the 
 * ISortingAlgorithm interface (Strategy Pattern). It sorts an array by repeatedly 
 * finding the minimum element from the unsorted part and putting it at the beginning.
 * * As it executes, it generates PlaybackFrames tailored for array visualization. 
 * The View team can expect the ArrayCellState to visually highlight the linear 
 * scan for the minimum value, the current minimum found so far, and the final 
 * swap into the sorted portion of the array. This class is marked final to allow 
 * the compiler to devirtualize method calls and enforce strict architectural boundaries.
 */
class SelectionSortAlgorithm final : public ISortingAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Selection Sort".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of Selection Sort.
     * @return std::string A brief description of the minimum-finding and swapping process.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Executes the Selection Sort algorithm on the provided dataset.
     *
     * This method processes the input array using an O(n^2) selection approach. 
     * At each significant visual step (scanning the remaining elements, updating 
     * the lowest known value, and swapping it to the front), it yields a Memento 
     * frame so the rendering engine can accurately animate the state transitions.
     *
     * @param values The unsorted array of integers to be processed.
     * @return AlgorithmRunResult The complete chronological history of array playback frames and sorting metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const std::vector<int>& values) const override;
};

}  // namespace av::algorithm

#endif