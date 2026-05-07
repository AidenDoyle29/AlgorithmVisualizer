#include "Algorithm/SelectionSortAlgorithm.h"

#include "Algorithm/SortingHelpers.h"

#include <algorithm>
#include <chrono>

namespace av::algorithm {

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing "Selection".
 */
std::string SelectionSortAlgorithm::name() const {
    return "Selection";
}

/**
 * @brief Provides a summary of the Selection Sort strategy.
 * @return A string describing the repeated selection of the minimum element.
 */
std::string SelectionSortAlgorithm::description() const {
    return "Sorts by repeatedly selecting the smallest remaining value and placing it at the front.";
}

/**
 * @brief Executes the Selection Sort algorithm and captures frames for visualization.
 * 
 * Selection sort divides the input list into two parts: a sorted sublist of items which is 
 * built up from left to right at the front (left) of the list, and a sublist of the 
 * remaining unsorted items that occupy the rest of the list. It repeatedly finds the 
 * smallest element in the unsorted sublist and swaps it with the leftmost unsorted element.
 * 
 * @param values The initial unsorted vector of integers.
 * @return An AlgorithmRunResult containing the sorted vector, metrics (swaps, 
 *         comparisons, passes), and visualization frames.
 */
AlgorithmRunResult SelectionSortAlgorithm::run(const std::vector<int>& values) const {
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
        "Initialize Selection Sort",
        {"Find the smallest element in the unsorted suffix and move it to the front."},
        metrics);

    // Outer loop: iterate through each position in the array
    for (std::size_t i = 0; i < working.size(); ++i) {
        ++metrics.passes;
        std::size_t minIndex = i; // Assume the first unsorted element is the minimum
        
        auto states = makeArrayStates(working.size());
        states[minIndex] = ArrayCellState::Active;
        appendArrayFrame(
            result.frames,
            working,
            states,
            "Start pass " + std::to_string(i + 1),
            {"The current front position is the tentative minimum."},
            metrics);

        // Inner loop: scan the remaining unsorted part to find the actual minimum
        for (std::size_t j = i + 1; j < working.size(); ++j) {
            ++metrics.stepsTaken;
            ++metrics.comparisons;
            
            auto compared = makeArrayStates(working.size());
            compared[minIndex] = ArrayCellState::Active; // Highlight current minimum
            compared[j] = ArrayCellState::Compared;      // Highlight candidate
            appendArrayFrame(
                result.frames,
                working,
                compared,
                "Compare candidate " + std::to_string(j),
                {"Check whether the new value is smaller than the current minimum."},
                metrics);

            if (working[j] < working[minIndex]) {
                minIndex = j; // Update the index of the smallest element found so far
                auto updated = makeArrayStates(working.size());
                updated[minIndex] = ArrayCellState::Active;
                appendArrayFrame(
                    result.frames,
                    working,
                    updated,
                    "New minimum at position " + std::to_string(minIndex),
                    {"A smaller value has been found."},
                    metrics);
            }
        }

        // Swap the found minimum with the first unsorted element
        if (minIndex != i) {
            std::swap(working[i], working[minIndex]);
            ++metrics.swaps;
            metrics.writes += 3;
            
            auto swapped = makeArrayStates(working.size());
            swapped[i] = ArrayCellState::Swapped;
            swapped[minIndex] = ArrayCellState::Swapped;
            appendArrayFrame(
                result.frames,
                working,
                swapped,
                "Place minimum at position " + std::to_string(i),
                {"The selected minimum is swapped into its final place for this pass."},
                metrics);
        }

        // Visual state: mark elements from the start up to i as sorted
        auto sorted = makeArrayStates(working.size());
        for (std::size_t index = 0; index <= i; ++index) {
            sorted[index] = ArrayCellState::Sorted;
        }
        appendArrayFrame(
            result.frames,
            working,
            sorted,
            "Pass " + std::to_string(i + 1) + " complete",
            {"The prefix up to this position is now in final order."},
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
        "Selection Sort complete",
        {"The array is sorted from left to right."},
        metrics,
        true);
        
    return result;
}

}  // namespace av::algorithm