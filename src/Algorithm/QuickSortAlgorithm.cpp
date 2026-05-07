#include "Algorithm/QuickSortAlgorithm.h"

#include "Algorithm/SortingHelpers.h"

#include <chrono>
#include <functional>

namespace av::algorithm {

namespace {

/**
 * @brief Helper to generate visual states for the partitioning phase of Quick Sort.
 * 
 * Specifically highlights the current range (left to right), the selected pivot, 
 * and the elements currently being compared or swapped.
 * 
 * @param values The current array values.
 * @param left The start index of the current partition.
 * @param right The end index (pivot index) of the current partition.
 * @param active A list of indices currently being processed.
 * @param activeState The visual state (color) to apply to the active indices.
 * @return A vector of ArrayCellState for the entire array.
 */
std::vector<ArrayCellState> quickStates(
    const std::vector<int>& values,
    const std::size_t left,
    const std::size_t right,
    const std::vector<std::size_t>& active = {},
    const ArrayCellState activeState = ArrayCellState::Compared) {
    auto states = makeArrayStates(values.size());
    for (std::size_t index = left; index <= right && index < values.size(); ++index) {
        states[index] = ArrayCellState::Normal;
    }
    if (right < values.size()) {
        states[right] = ArrayCellState::Pivot;
    }
    for (const auto index : active) {
        if (index < values.size()) {
            states[index] = activeState;
        }
    }
    return states;
}

}  // namespace

/**
 * @brief Returns the display name of the algorithm.
 * @return A string representing "Quick".
 */
std::string QuickSortAlgorithm::name() const {
    return "Quick";
}

/**
 * @brief Provides a summary of the Quick Sort strategy.
 * @return A string describing the partition-based divide-and-conquer approach.
 */
std::string QuickSortAlgorithm::description() const {
    return "Partition-based divide-and-conquer sort that places a pivot in its final position.";
}

/**
 * @brief Executes the Quick Sort algorithm and captures frames for visualization.
 * 
 * This implementation uses the Lomuto partition scheme, where the rightmost 
 * element is chosen as the pivot. The array is rearranged so that all elements 
 * smaller than the pivot are to its left, and all larger elements are to its right. 
 * This process is then applied recursively to the resulting sub-arrays.
 * 
 * @param values The initial unsorted vector of integers.
 * @return An AlgorithmRunResult containing the sorted vector, metrics (swaps, 
 *         comparisons, writes), and visualization frames.
 */
AlgorithmRunResult QuickSortAlgorithm::run(const std::vector<int>& values) const {
    std::vector<int> working = values;
    AlgorithmRunResult result;
    result.algorithmName = name();

    AlgorithmMetrics metrics;
    const auto startTime = std::chrono::steady_clock::now();

    // Initial frame showing the setup
    appendArrayFrame(
        result.frames,
        working,
        makeArrayStates(working.size()),
        "Initialize Quick Sort",
        {"Choose a pivot, partition the array, then recursively sort the partitions."},
        metrics);

    /**
     * @brief Recursive lambda to perform partitioning and sorting.
     * @param left Start index of the partition.
     * @param right End index of the partition (used as pivot).
     */
    std::function<void(std::size_t, std::size_t)> quickSort = [&](const std::size_t left, const std::size_t right) {
        // Base case: range is empty or single element
        if (right <= left) {
            return;
        }

        const int pivot = working[right];
        std::size_t storeIndex = left;

        // Visualize pivot selection
        appendArrayFrame(
            result.frames,
            working,
            quickStates(working, left, right, {right}, ArrayCellState::Pivot),
            "Select pivot",
            {"The rightmost value is used as the pivot for this partition."},
            metrics);

        // Partitioning loop
        for (std::size_t index = left; index < right; ++index) {
            ++metrics.stepsTaken;
            ++metrics.comparisons;
            
            appendArrayFrame(
                result.frames,
                working,
                quickStates(working, left, right, {index}, ArrayCellState::Compared),
                "Compare with pivot",
                {"Values smaller than the pivot stay on the left side."},
                metrics);

            if (working[index] < pivot) {
                if (storeIndex != index) {
                    std::swap(working[storeIndex], working[index]);
                    ++metrics.swaps;
                    metrics.writes += 3;
                    
                    // Visualize the move into the smaller-than-pivot section
                    appendArrayFrame(
                        result.frames,
                        working,
                        quickStates(working, left, right, {storeIndex, index}, ArrayCellState::Swapped),
                        "Swap into left partition",
                        {"The value belongs before the pivot, so it moves left."},
                        metrics);
                }
                ++storeIndex;
            }
        }

        // Place the pivot in its final sorted position
        std::swap(working[storeIndex], working[right]);
        ++metrics.swaps;
        metrics.writes += 3;
        
        appendArrayFrame(
            result.frames,
            working,
            quickStates(working, left, right, {storeIndex}, ArrayCellState::Swapped),
            "Place pivot at final position",
            {"The pivot lands between the two partitions."},
            metrics);

        // Recursive calls on the two resulting sub-partitions
        if (storeIndex > left) {
            quickSort(left, storeIndex - 1U);
        }
        if (storeIndex + 1U <= right) {
            quickSort(storeIndex + 1U, right);
        }
    };

    if (!working.empty()) {
        quickSort(0U, working.size() - 1U);
    }

    // Final frame marking the entire array as sorted
    auto finalStates = makeArrayStates(working.size(), ArrayCellState::Sorted);
    finalizeMetrics(metrics, startTime);
    result.metrics = metrics;
    result.sortedValues = working;
    appendArrayFrame(
        result.frames,
        working,
        finalStates,
        "Quick Sort complete",
        {"The array is sorted from left to right."},
        metrics,
        true);
        
    return result;
}

}  // namespace av::algorithm