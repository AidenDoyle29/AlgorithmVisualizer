#include "Algorithm/MergeSortAlgorithm.h"

#include "Algorithm/SortingHelpers.h"

#include <chrono>
#include <functional>

namespace av::algorithm {

namespace {

/**
 * @brief Helper to generate visual states for a specific segment of the array during recursion.
 * 
 * Highlights the active sub-array being merged and provides specific color states for 
 * indices currently being compared or moved.
 * 
 * @param size The total size of the array.
 * @param left The starting index of the current sub-array.
 * @param right The ending index (exclusive) of the current sub-array.
 * @param active A list of indices to highlight with the activeState.
 * @param activeState The state (color) to apply to active indices.
 * @return A vector of ArrayCellState for the entire array.
 */
std::vector<ArrayCellState> segmentStates(
    const std::size_t size,
    const std::size_t left,
    const std::size_t right,
    const std::vector<std::size_t>& active = {},
    const ArrayCellState activeState = ArrayCellState::Compared) {
    auto states = makeArrayStates(size);
    for (std::size_t index = left; index < right && index < size; ++index) {
        states[index] = ArrayCellState::Sorted;
    }
    for (const auto index : active) {
        if (index < size) {
            states[index] = activeState;
        }
    }
    return states;
}

}  // namespace

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing "Merge".
 */
std::string MergeSortAlgorithm::name() const {
    return "Merge";
}

/**
 * @brief Provides a summary of the Merge Sort strategy.
 * @return A string describing the divide-and-conquer approach.
 */
std::string MergeSortAlgorithm::description() const {
    return "Divide-and-conquer sort that merges two sorted halves into one ordered segment.";
}

/**
 * @brief Executes the Merge Sort algorithm and captures frames for visualization.
 * 
 * This implementation uses a recursive helper function to split the array down to 
 * single-element base cases and then merges the results back together using a 
 * temporary buffer. Visualization frames are captured for comparisons, buffer writes, 
 * and final segment completions.
 * 
 * @param values The initial unsorted vector of integers.
 * @return An AlgorithmRunResult containing the sorted vector, metrics (comparisons, 
 *         writes, steps), and visualization frames.
 */
AlgorithmRunResult MergeSortAlgorithm::run(const std::vector<int>& values) const {
    std::vector<int> working = values;
    std::vector<int> buffer(working.size(), 0);
    AlgorithmRunResult result;
    result.algorithmName = name();

    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    // Initial frame showing the full unsorted array
    appendArrayFrame(
        result.frames,
        working,
        makeArrayStates(working.size()),
        "Initialize Merge Sort",
        {"Split the array into halves, sort each half, then merge the results."},
        metrics);

    /**
     * @brief Recursive lambda to perform the divide-and-conquer sort.
     * @param left Start index of the segment.
     * @param right End index (exclusive) of the segment.
     */
    std::function<void(std::size_t, std::size_t)> mergeSort = [&](const std::size_t left, const std::size_t right) {
        // Base case: segments of size 0 or 1 are already sorted
        if (right - left <= 1U) {
            return;
        }

        const std::size_t middle = left + (right - left) / 2U;
        
        // Recursively sort the two halves
        mergeSort(left, middle);
        mergeSort(middle, right);

        // Merge logic: compare heads of each half and move the smaller into the buffer
        std::size_t i = left;
        std::size_t j = middle;
        std::size_t k = left;

        while (i < middle && j < right) {
            ++metrics.stepsTaken;
            ++metrics.comparisons;

            // Visual state: highlighting the two elements currently being compared
            appendArrayFrame(
                result.frames,
                working,
                segmentStates(working.size(), left, right, {i, j}),
                "Compare merge heads",
                {"Choose the smaller value from the left or right half."},
                metrics);

            if (working[i] <= working[j]) {
                buffer[k++] = working[i++];
            } else {
                buffer[k++] = working[j++];
            }
            ++metrics.writes;

            appendArrayFrame(
                result.frames,
                working,
                segmentStates(working.size(), left, right),
                "Write merged value",
                {"The next smallest value is written into the merge buffer."},
                metrics);
        }

        // Copy any remaining elements from the left half
        while (i < middle) {
            ++metrics.stepsTaken;
            buffer[k++] = working[i++];
            ++metrics.writes;
            appendArrayFrame(
                result.frames,
                working,
                segmentStates(working.size(), left, right, {i - 1}, ArrayCellState::Active),
                "Copy remaining left value",
                {"Values left in the left half are already in order."},
                metrics);
        }

        // Copy any remaining elements from the right half
        while (j < right) {
            ++metrics.stepsTaken;
            buffer[k++] = working[j++];
            ++metrics.writes;
            appendArrayFrame(
                result.frames,
                working,
                segmentStates(working.size(), left, right, {j - 1}, ArrayCellState::Active),
                "Copy remaining right value",
                {"Values left in the right half are already in order."},
                metrics);
        }

        // Copy the sorted elements from the buffer back into the working array
        for (std::size_t index = left; index < right; ++index) {
            working[index] = buffer[index];
            ++metrics.writes;
        }

        // Visualize the completion of this segment's merge
        appendArrayFrame(
            result.frames,
            working,
            segmentStates(working.size(), left, right),
            "Merge segment complete",
            {"The segment is now sorted."},
            metrics);
    };

    if (!working.empty()) {
        mergeSort(0U, working.size());
    }

    // Final state marking the entire array as sorted
    auto finalStates = makeArrayStates(working.size(), ArrayCellState::Sorted);
    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    result.sortedValues = working;
    appendArrayFrame(
        result.frames,
        working,
        finalStates,
        "Merge Sort complete",
        {"The array is sorted from left to right."},
        metrics,
        true);
        
    return result;
}

}  // namespace av::algorithm