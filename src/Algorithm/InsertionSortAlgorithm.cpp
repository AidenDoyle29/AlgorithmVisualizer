#include "Algorithm/InsertionSortAlgorithm.h"

#include "Algorithm/SortingHelpers.h"

#include <chrono>

namespace av::algorithm {

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing "Insertion".
 */
std::string InsertionSortAlgorithm::name() const {
    return "Insertion";
}

/**
 * @brief Provides a summary of the Insertion Sort strategy.
 * @return A string describing the incremental building of a sorted prefix.
 */
std::string InsertionSortAlgorithm::description() const {
    return "Builds a sorted prefix by inserting each new element into the correct location.";
}

/**
 * @brief Executes the Insertion Sort algorithm and captures frames for visualization.
 * 
 * Insertion sort works by virtually splitting the array into a sorted and an unsorted part. 
 * Values from the unsorted part are picked and placed at the correct position in the sorted part.
 * This implementation captures the "key" selection, the shifting of elements, and the final 
 * insertion for each pass.
 * 
 * @param values The initial unsorted vector of integers.
 * @return An AlgorithmRunResult containing the sorted vector, metrics (writes, 
 *         comparisons, passes), and visualization frames.
 */
AlgorithmRunResult InsertionSortAlgorithm::run(const std::vector<int>& values) const {
    std::vector<int> working = values;
    AlgorithmRunResult result;
    result.algorithmName = name();

    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    // Initial state frame: starting the sorting process
    appendArrayFrame(
        result.frames,
        working,
        makeArrayStates(working.size()),
        "Initialize Insertion Sort",
        {"Take each element and insert it into the sorted prefix on the left."},
        metrics);

    if (working.empty()) {
        finalizeMetrics(metrics, startTime);
        result.metrics = metrics;
        result.sortedValues = working;
        appendArrayFrame(result.frames, working, {}, "Insertion Sort complete", {"Nothing to sort."}, metrics, true);
        return result;
    }

    // Outer loop: iterate through the array starting from the second element
    for (std::size_t i = 1; i < working.size(); ++i) {
        ++metrics.passes;
        const int key = working[i]; // The element currently being positioned
        std::size_t j = i;

        // Inner loop: shift elements of the sorted prefix that are greater than the key
        while (j > 0U) {
            ++metrics.stepsTaken;
            ++metrics.comparisons;

            // Visual state: comparing the key against the element at j-1
            auto compared = makeArrayStates(working.size());
            compared[j] = ArrayCellState::Active; // Current "hole" or key position
            compared[j - 1] = ArrayCellState::Compared;
            appendArrayFrame(
                result.frames,
                working,
                compared,
                "Compare key with position " + std::to_string(j - 1),
                {"Shift larger values right until the insertion point is found."},
                metrics);

            // If the element is smaller or equal to the key, we've found our spot
            if (working[j - 1] <= key) {
                break;
            }

            // Shift the larger element to the right
            working[j] = working[j - 1];
            ++metrics.writes;

            // Visual state: element shifting right
            auto shifted = makeArrayStates(working.size());
            shifted[j] = ArrayCellState::Swapped;
            shifted[j - 1] = ArrayCellState::Active;
            appendArrayFrame(
                result.frames,
                working,
                shifted,
                "Shift value at position " + std::to_string(j - 1),
                {"The current value moves right to make room for the key."},
                metrics);
            --j;
        }

        // Place the key into its correct sorted position
        working[j] = key;
        ++metrics.writes;

        // Visual state: key successfully inserted
        auto inserted = makeArrayStates(working.size());
        inserted[j] = ArrayCellState::Swapped;
        appendArrayFrame(
            result.frames,
            working,
            inserted,
            "Insert key at position " + std::to_string(j),
            {"The key is placed into the open slot."},
            metrics);

        // Visual state: highlight the current sorted prefix
        auto sorted = makeArrayStates(working.size());
        for (std::size_t index = 0; index <= i; ++index) {
            sorted[index] = ArrayCellState::Sorted;
        }
        appendArrayFrame(
            result.frames,
            working,
            sorted,
            "Prefix up to position " + std::to_string(i) + " sorted",
            {"The left side grows as a sorted prefix."},
            metrics);
    }

    // Wrap up execution and finalize metrics
    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    result.sortedValues = working;
    
    // Final frame marking the entire array as sorted
    auto finalStates = makeArrayStates(working.size(), ArrayCellState::Sorted);
    appendArrayFrame(
        result.frames,
        working,
        finalStates,
        "Insertion Sort complete",
        {"The array is sorted from left to right."},
        metrics,
        true);
        
    return result;
}

}  // namespace av::algorithm