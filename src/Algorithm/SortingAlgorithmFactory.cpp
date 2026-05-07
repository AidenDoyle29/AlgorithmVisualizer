#include "Algorithm/SortingAlgorithmFactory.h"

#include "Algorithm/BubbleSortAlgorithm.h"
#include "Algorithm/InsertionSortAlgorithm.h"
#include "Algorithm/MergeSortAlgorithm.h"
#include "Algorithm/QuickSortAlgorithm.h"
#include "Algorithm/SelectionSortAlgorithm.h"

#include <algorithm>
#include <stdexcept>

namespace av::algorithm {

/**
 * @brief Constructs the factory and registers all supported sorting algorithms.
 * 
 * The constructor populates the internal registry with shared instances of 
 * available algorithms. This centralizes algorithm management and allows the 
 * rest of the application to request algorithms by their string names.
 */
SortingAlgorithmFactory::SortingAlgorithmFactory() {
    algorithms_.emplace("Bubble", std::make_shared<BubbleSortAlgorithm>());
    algorithms_.emplace("Selection", std::make_shared<SelectionSortAlgorithm>());
    algorithms_.emplace("Insertion", std::make_shared<InsertionSortAlgorithm>());
    algorithms_.emplace("Merge", std::make_shared<MergeSortAlgorithm>());
    algorithms_.emplace("Quick", std::make_shared<QuickSortAlgorithm>());
}

/**
 * @brief Retrieves a list of all registered algorithm names.
 * 
 * This is useful for populating UI components (like dropdown menus) with 
 * the names of algorithms that this factory can provide.
 * 
 * @return A sorted vector of strings containing the keys for all available algorithms.
 */
std::vector<std::string> SortingAlgorithmFactory::availableAlgorithms() const {
    std::vector<std::string> names;
    names.reserve(algorithms_.size());
    for (const auto& [name, algorithm] : algorithms_) {
        (void)algorithm; // Explicitly ignore the value to avoid unused variable warnings
        names.push_back(name);
    }
    // Ensure the list is alphabetical for consistent UI presentation
    std::sort(names.begin(), names.end());
    return names;
}

/**
 * @brief Provides a shared pointer to the requested sorting algorithm.
 * 
 * Accesses the internal registry to return a pointer to the constant algorithm interface.
 * 
 * @param name The string identifier of the algorithm (e.g., "Merge", "Quick").
 * @return A shared_ptr to the requested ISortingAlgorithm implementation.
 * @throws std::invalid_argument if the provided name does not exist in the registry.
 */
std::shared_ptr<const ISortingAlgorithm> SortingAlgorithmFactory::get(const std::string& name) const {
    const auto iterator = algorithms_.find(name);
    if (iterator == algorithms_.end()) {
        throw std::invalid_argument("unknown sorting algorithm: " + name);
    }
    return iterator->second;
}

}  // namespace av::algorithm