#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_H

#include "Algorithm/Algorithm.h"

#include <string>
#include <vector>

namespace av::algorithm {

class ISortingAlgorithm {
  public:
    virtual ~ISortingAlgorithm() = default;

    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;
    [[nodiscard]] virtual AlgorithmRunResult run(const std::vector<int>& values) const = 0;
};

}  // namespace av::algorithm

#endif
