/**
 * @file QuickSortAlgorithm.h
 * @brief Defines the Quick Sort algorithm strategy.
 *
 * This file implements the concrete strategy for the Quick Sort algorithm, 
 * conforming to the ISortingAlgorithm interface to visualize array 
 * partitioning and recursive sorting.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_QUICK_SORT_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_QUICK_SORT_ALGORITHM_H

#include "Algorithm/SortingAlgorithm.h"

namespace av::algorithm {

/**
 * @class QuickSortAlgorithm
 * @brief Executes a recursive Quick Sort on a collection of integers.
 *
 * The QuickSortAlgorithm class is a concrete implementation of the 
 * ISortingAlgorithm interface (Strategy Pattern). It utilizes a highly 
 * efficient, average O(n log n) divide-and-conquer strategy by selecting a 
 * "pivot" element and partitioning the array so that smaller elements are 
 * moved to the left of the pivot and larger elements to the right.
 * * As it executes, it generates PlaybackFrames tailored for array visualization. 
 * The View team can expect the ArrayCellState to visually distinguish the 
 * active pivot, the elements being compared against it, and the resulting swaps.
 * This class is marked final to allow the compiler to devirtualize method calls 
 * and enforce strict architectural boundaries.
 */
class QuickSortAlgorithm final : public ISortingAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Quick Sort".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of Quick Sort.
     * @return std::string A brief description of the pivot selection and partitioning process.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Executes the Quick Sort algorithm on the provided dataset.
     *
     * This method processes the input array using recursive partitioning. 
     * At each significant visual step (selecting a new pivot, scanning elements 
     * from the left and right, and swapping values into their correct partitions), 
     * it yields a Memento frame so the rendering engine can accurately animate 
     * the state transitions.
     *
     * @param values The unsorted array of integers to be processed.
     * @return AlgorithmRunResult The complete chronological history of array playback frames and sorting metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const std::vector<int>& values) const override;
};

}  // namespace av::algorithm

#endif