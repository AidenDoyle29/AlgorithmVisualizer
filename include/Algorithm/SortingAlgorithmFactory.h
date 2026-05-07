#ifndef ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_FACTORY_H
#define ALGORITHM_VISUALIZER_ALGORITHM_SORTING_ALGORITHM_FACTORY_H

#include "Algorithm/SortingAlgorithm.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::algorithm {

class SortingAlgorithmFactory {
  public:
    SortingAlgorithmFactory();

    [[nodiscard]] std::vector<std::string> availableAlgorithms() const;
    [[nodiscard]] std::shared_ptr<const ISortingAlgorithm> get(const std::string& name) const;

  private:
    std::unordered_map<std::string, std::shared_ptr<const ISortingAlgorithm>> algorithms_ {};
};

}  // namespace av::algorithm

#endif
