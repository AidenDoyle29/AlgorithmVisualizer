/**
 * @file BubbleSortAlgorithm.h
 * @brief Defines the Bubble Sort algorithm strategy.
 *
 * This file implements the concrete strategy for the Bubble Sort algorithm, 
 * conforming to the ISortingAlgorithm interface to visualize array manipulations.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_BUBBLE_SORT_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_BUBBLE_SORT_ALGORITHM_H

#include "Algorithm/SortingAlgorithm.h"

namespace av::algorithm {

/**
 * @class BubbleSortAlgorithm
 * @brief Executes an iterative Bubble Sort on a collection of integers.
 *
 * The BubbleSortAlgorithm class is a concrete implementation of the ISortingAlgorithm 
 * interface (Strategy Pattern). It repeatedly steps through the input list, compares 
 * adjacent elements, and swaps them if they are in the wrong order. 
 * * As it runs, it generates PlaybackFrames tailored for array visualization, 
 * updating the ArrayCellState of elements to reflect comparisons and swaps.
 * * This class is marked final to allow the compiler to devirtualize method calls 
 * for minor performance optimizations and to enforce architectural boundaries.
 */
class BubbleSortAlgorithm final : public ISortingAlgorithm {
  public:
    /**
     * @brief Retrieves the formal name of the algorithm.
     * @return std::string "Bubble Sort".
     */
    [[nodiscard]] std::string name() const override;

    /**
     * @brief Provides a short, human-readable explanation of Bubble Sort.
     * @return std::string A brief description of the adjacent-element swapping process.
     */
    [[nodiscard]] std::string description() const override;

    /**
     * @brief Executes the Bubble Sort algorithm on the provided dataset.
     *
     * This method processes the input array, executing the standard O(n^2) 
     * sorting logic. At each significant step (comparing two values, executing 
     * a swap, or finalizing an element's position), it yields a Memento frame 
     * so the rendering engine can accurately animate the array state.
     *
     * @param values The unsorted array of integers to be processed.
     * @return AlgorithmRunResult The complete chronological history of array playback frames and sorting metrics.
     */
    [[nodiscard]] AlgorithmRunResult run(const std::vector<int>& values) const override;
};

}  // namespace av::algorithm

#endif