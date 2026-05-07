#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SORTING_HELPERS_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SORTING_HELPERS_H

#include "Algorithm/Algorithm.h"

#include <string>
#include <vector>

namespace av::algorithm {

[[nodiscard]] std::string renderArrayState(ArrayCellState state);
[[nodiscard]] std::vector<std::string> arraySnapshotDetails(const std::vector<int>& values);

}  // namespace av::algorithm

#endif
