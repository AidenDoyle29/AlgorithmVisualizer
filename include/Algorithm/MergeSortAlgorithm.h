/**
 * @file MergeSortAlgorithm.h
 * @brief Defines the Merge Sort algorithm strategy.
 *
 * This file implements the concrete strategy for the Merge Sort algorithm, 
 * conforming to the ISortingAlgorithm interface to visualize array manipulations 
 * using a divide-and-conquer approach.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_MERGE_SORT_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_MERGE_SORT_ALGORITHM_H

#include "Algorithm/SortingAlgorithm.h"

namespace av::algorithm {

/**
 * @class MergeSortAlgorithm
 * @brief Executes a recursive Merge Sort on a collection of integers.
 *
 * The MergeSortAlgorithm class is a concrete implementation of the 
 * ISortingAlgorithm interface (Strategy Pattern). It utilizes a highly efficient, 
 * O(n log n) divide-and-conquer strategy, repeatedly splitting the array in half 
 * until sub-arrays are trivially sorted, and then merging them back together.
 * * As it executes, it generates PlaybackFrames tailored for array visualization. 
 * The View team can expect the ArrayCellState to visually distinguish between 
 * the active sub-arrays being merged, element comparisons, and finalized placements.
 * This class is marked final to allow the compiler to devirtualize method calls 
 * and enforce strict architectural boundaries.
 */
class MergeSortAlgorithm final : public ISortingAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Merge Sort".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of Merge Sort.
     * @return std::string A brief description of the divide-and-conquer and merging process.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Executes the Merge Sort algorithm on the provided dataset.
     *
     * This method processes the input array using recursive splitting and merging. 
     * At each significant visual step (partitioning the array, comparing elements 
     * from the left and right halves, and overwriting the original array with sorted 
     * values), it yields a Memento frame so the rendering engine can accurately 
     * animate the state transitions.
     *
     * @param values The unsorted array of integers to be processed.
     * @return AlgorithmRunResult The complete chronological history of array playback frames and sorting metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const std::vector<int>& values) const override;
};

}  // namespace av::algorithm

#endif