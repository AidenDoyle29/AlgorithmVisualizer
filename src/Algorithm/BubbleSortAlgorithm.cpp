#include "Algorithm/BubbleSortAlgorithm.h"

#include "Algorithm/SortingHelpers.h"

#include <algorithm>
#include <chrono>
#include <sstream>

namespace av::algorithm {

namespace {

/**
 * @brief Helper to generate visual states marking elements as "Sorted" at the end of the array.
 * 
 * In Bubble Sort, the largest elements "bubble" to the end. After each pass, 
 * the rightmost elements are guaranteed to be in their final positions.
 * 
 * @param size The total number of elements in the array.
 * @param sortedCount The number of elements from the right that are now sorted.
 * @return A vector of ArrayCellState indicating which indices are sorted.
 */
std::vector<ArrayCellState> sortedStates(const std::size_t size, const std::size_t sortedCount) {
    auto states = makeArrayStates(size);
    if (sortedCount == 0U || size == 0U) {
        return states;
    }

    const std::size_t start = size - sortedCount;
    for (std::size_t index = start; index < size; ++index) {
        states[index] = ArrayCellState::Sorted;
    }
    return states;
}

}  // namespace

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing "Bubble".
 */
std::string BubbleSortAlgorithm::name() const {
    return "Bubble";
}

/**
 * @brief Provides a summary of the Bubble Sort behavior.
 * @return A string describing the adjacent comparison and "bubbling" mechanism.
 */
std::string BubbleSortAlgorithm::description() const {
    return "Adjacent comparison sort that repeatedly bubbles larger values toward the end.";
}

/**
 * @brief Executes the Bubble Sort algorithm and captures frames for visualization.
 * 
 * This implementation sorts the input vector in ascending order. It includes 
 * an optimization to stop early if a full pass occurs without any swaps, 
 * indicating the array is already sorted.
 * 
 * @param values The initial unsorted vector of integers.
 * @return An AlgorithmRunResult containing the sorted vector, metrics (swaps, 
 *         comparisons, passes), and visualization frames.
 */
AlgorithmRunResult BubbleSortAlgorithm::run(const std::vector<int>& values) const {
    std::vector<int> working = values;
    AlgorithmRunResult result;
    result.algorithmName = name();

    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    // Initial state frame
    appendArrayFrame(
        result.frames,
        working,
        makeArrayStates(working.size()),
        "Initialize Bubble Sort",
        {"Compare adjacent values and swap them when they are out of order."},
        metrics);

    if (working.empty()) {
        finalizeMetrics(metrics, startTime);
        result.metrics = metrics;
        result.sortedValues = working;
        appendArrayFrame(result.frames, working, {}, "Bubble Sort complete", {"Nothing to sort."}, metrics, true);
        return result;
    }

    // Outer loop for each pass through the array
    for (std::size_t pass = 0; pass < working.size(); ++pass) {
        ++metrics.passes;
        bool swappedAny = false;
        // Optimization: Subsequent passes don't need to check elements already bubbled to the end
        const std::size_t limit = working.size() - pass - 1;

        for (std::size_t index = 0; index < limit; ++index) {
            ++metrics.stepsTaken;
            ++metrics.comparisons;

            // Visual state for comparison
            auto compared = makeArrayStates(working.size());
            compared[index] = ArrayCellState::Compared;
            compared[index + 1] = ArrayCellState::Compared;
            appendArrayFrame(
                result.frames,
                working,
                compared,
                "Compare positions " + std::to_string(index) + " and " + std::to_string(index + 1),
                arraySnapshotDetails(working),
                metrics);

            if (working[index] > working[index + 1]) {
                // Perform the swap
                std::swap(working[index], working[index + 1]);
                ++metrics.swaps;
                metrics.writes += 3; // Swapping typically involves 3 writes
                swappedAny = true;

                // Visual state for the swap action
                auto swapped = makeArrayStates(working.size());
                swapped[index] = ArrayCellState::Swapped;
                swapped[index + 1] = ArrayCellState::Swapped;
                appendArrayFrame(
                    result.frames,
                    working,
                    swapped,
                    "Swap positions " + std::to_string(index) + " and " + std::to_string(index + 1),
                    {"The values were out of order, so they were exchanged."},
                    metrics);
            }
        }

        // Highlight the newly sorted section at the end of the pass
        auto states = sortedStates(working.size(), pass + 1);
        appendArrayFrame(
            result.frames,
            working,
            states,
            "Pass " + std::to_string(pass + 1) + " complete",
            {"The largest unsorted value has moved into place."},
            metrics);

        // Optimization: If no elements were swapped, the array is sorted
        if (!swappedAny) {
            break;
        }
    }

    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    result.sortedValues = working;
    
    // Final frame marking the entire array as sorted
    auto finalStates = makeArrayStates(working.size(), ArrayCellState::Sorted);
    appendArrayFrame(
        result.frames,
        working,
        finalStates,
        "Bubble Sort complete",
        {"The array is sorted from left to right."},
        metrics,
        true);
        
    return result;
}

}  // namespace av::algorithm