#include "Algorithm/SortingHelpers.h"

#include <sstream>

namespace av::algorithm {

/**
 * @brief Converts an ArrayCellState enum value into a human-readable string for logging or UI.
 * 
 * This helper facilitates the bridge between the internal algorithm state and the 
 * visual representation in the UI, mapping states like 'Pivot' or 'Compared' to 
 * CSS-friendly or user-facing labels.
 * 
 * @param state The internal state of a specific array cell.
 * @return A string representing the visual category of the cell (e.g., "pivot", "swapped").
 */
std::string renderArrayState(const ArrayCellState state) {
    switch (state) {
        case ArrayCellState::Normal:
            return "normal";
        case ArrayCellState::Active:
            return "current";
        case ArrayCellState::Compared:
            return "compared";
        case ArrayCellState::Swapped:
            return "swapped";
        case ArrayCellState::Sorted:
            return "sorted";
        case ArrayCellState::Pivot:
            return "pivot";
    }

    return "normal";
}

/**
 * @brief Generates a detailed string representation of the current array values.
 * 
 * Typically used for debugging or providing textual "details" in visualization frames. 
 * Formats the array in a standard bracketed list format, e.g., "Array: [1, 2, 3]".
 * 
 * @param values The vector of integers to be snapshotted.
 * @return A vector of strings containing the formatted array representation.
 */
std::vector<std::string> arraySnapshotDetails(const std::vector<int>& values) {
    std::ostringstream stream;
    stream << "Array: [";
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0U) {
            stream << ", ";
        }
        stream << values[index];
    }
    stream << "]";
    return {stream.str()};
}

}  // namespace av::algorithm