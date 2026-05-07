#ifndef ALGORITHM_VISUALIZER_ALGORITHM_QUICK_SORT_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_QUICK_SORT_ALGORITHM_H

#include "Algorithm/SortingAlgorithm.h"

namespace av::algorithm {

class QuickSortAlgorithm final : public ISortingAlgorithm {
  public:
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] std::string description() const override;
    [[nodiscard]] AlgorithmRunResult run(const std::vector<int>& values) const override;
};

}  // namespace av::algorithm

#endif
