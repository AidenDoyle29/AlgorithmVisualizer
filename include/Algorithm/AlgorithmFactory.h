#ifndef ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_FACTORY_H
#define ALGORITHM_VISUALIZER_ALGORITHM_ALGORITHM_FACTORY_H

#include "Algorithm/Algorithm.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace av::algorithm {

class AlgorithmFactory {
  public:
    AlgorithmFactory();

    [[nodiscard]] std::vector<std::string> availableAlgorithms() const;
    [[nodiscard]] std::shared_ptr<const IAlgorithm> get(const std::string& name) const;

  private:
    std::unordered_map<std::string, std::shared_ptr<const IAlgorithm>> algorithms_ {};
};

}  // namespace av::algorithm

#endif
