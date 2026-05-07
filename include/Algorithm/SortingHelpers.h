/**
 * @file SortingHelpers.h
 * @brief Defines stateless utility functions for array visualization and Memento generation.
 *
 * This file contains namespace-level free functions that assist in formatting 
 * and translating array data for the rendering engine. By decoupling these 
 * helpers from the core sorting strategies, the main algorithm classes remain 
 * strictly focused on mathematical logic rather than UI string formatting.
 */

#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SORTING_HELPERS_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SORTING_HELPERS_H

#include "Algorithm/Algorithm.h"

#include <string>
#include <vector>

namespace av::algorithm {

/**
 * @brief Translates an ArrayCellState enum into a human-readable string identifier.
 *
 * This function bridges the gap between the algorithm's internal state tracking 
 * and the View layer. It converts the strict enum values (e.g., Active, Compared, 
 * Swapped) into strings that the UI can easily map to rendering colors or logs.
 *
 * @param state The current visual state of an array element.
 * @return std::string A textual representation of the state.
 */
[[nodiscard]] std::string renderArrayState(ArrayCellState state);

/**
 * @brief Serializes a raw array of integers into a formatted list of detail strings.
 *
 * This utility takes a snapshot of the array's numeric values and formats it 
 * into a structured string vector. This is primarily used to populate the 
 * "details" field of a PlaybackFrame, allowing the UI's side-panel to display 
 * exactly what the array looked like at that specific moment in time.
 *
 * @param values The current numeric values of the array.
 * @return std::vector<std::string> A formatted string representation of the array state.
 */
[[nodiscard]] std::vector<std::string> arraySnapshotDetails(const std::vector<int>& values);

}  // namespace av::algorithm

#endif