/**
 * @file InsertionSortAlgorithm.h
 * @brief Defines the Insertion Sort algorithm strategy.
 *
 * This file implements the concrete strategy for the Insertion Sort algorithm,
 * conforming to the ISortingAlgorithm interface to visualize array manipulations.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_INSERTION_SORT_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_INSERTION_SORT_ALGORITHM_H

#include "Algorithm/SortingAlgorithm.h"

namespace av::algorithm
{

    /**
     * @class InsertionSortAlgorithm
     * @brief Executes an iterative Insertion Sort on a collection of integers.
     *
     * The InsertionSortAlgorithm class is a concrete implementation of the
     * ISortingAlgorithm interface (Strategy Pattern). It builds the final sorted
     * array one item at a time by taking elements from the unsorted portion and
     * inserting them into their correct position within the sorted portion.
     * * As it executes, it generates PlaybackFrames tailored for array visualization,
     * updating the ArrayCellState to reflect active comparisons, element shifts,
     * and sorted placements. This class is marked final to allow the compiler to
     * devirtualize method calls for minor performance optimizations and enforce
     * architectural boundaries.
     */
    class InsertionSortAlgorithm final : public ISortingAlgorithm
    {
    public:
        /**
         * @brief Retrieves the formal name of the algorithm.
         * @return std::string "Insertion Sort".
         */
        [[nodiscard]] std::string name() const override;

        /**
         * @brief Provides a short, human-readable explanation of Insertion Sort.
         * @return std::string A brief description of the shift-and-insert sorting process.
         */
        [[nodiscard]] std::string description() const override;

        /**
         * @brief Executes the Insertion Sort algorithm on the provided dataset.
         *
         * This method processes the input array, executing the standard O(n^2)
         * insertion sorting logic. At each significant step (selecting a key,
         * shifting larger elements to the right, and placing the key), it yields
         * a Memento frame so the rendering engine can accurately animate the state.
         *
         * @param values The unsorted array of integers to be processed.
         * @return AlgorithmRunResult The complete chronological history of array playback frames and sorting metrics.
         */
        [[nodiscard]] AlgorithmRunResult run(const std::vector<int> &values) const override;
    };

} // namespace av::algorithm

#endif