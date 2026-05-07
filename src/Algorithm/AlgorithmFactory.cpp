#include "Algorithm/AlgorithmFactory.h"

#include "Algorithm/BreadthFirstSearch.h"
#include "Algorithm/DepthFirstSearch.h"
#include "Algorithm/DijkstraAlgorithm.h"
#include "Algorithm/KruskalAlgorithm.h"

#include <algorithm>
#include <stdexcept>

namespace av::algorithm {

/**
 * @brief Constructs the AlgorithmFactory and registers supported algorithms.
 * 
 * The constructor initializes the internal registry by mapping string identifiers
 * to shared instances of algorithm implementations such as BFS, DFS, Dijkstra, and Kruskal.
 */
AlgorithmFactory::AlgorithmFactory() {
    algorithms_.emplace("BFS", std::make_shared<BreadthFirstSearch>());
    algorithms_.emplace("DFS", std::make_shared<DepthFirstSearch>());
    algorithms_.emplace("Dijkstra", std::make_shared<DijkstraAlgorithm>());
    algorithms_.emplace("Kruskal", std::make_shared<KruskalAlgorithm>());
}

/**
 * @brief Retrieves a list of all algorithm names currently registered in the factory.
 * 
 * This method extracts the keys from the internal map and sorts them alphabetically
 * to provide a consistent ordering for UI components or logging.
 * 
 * @return A sorted vector of strings containing the names of available algorithms.
 */
std::vector<std::string> AlgorithmFactory::availableAlgorithms() const {
    std::vector<std::string> names;
    names.reserve(algorithms_.size());
    for (const auto& [name, algorithm] : algorithms_) {
        (void)algorithm;
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

/**
 * @brief Returns a pointer to the algorithm associated with the given name.
 * 
 * Accesses the registry to find a specific implementation. The returned pointer
 * is a shared_ptr to a constant IAlgorithm, ensuring the implementation remains immutable.
 * 
 * @param name The string identifier of the desired algorithm.
 * @return A shared pointer to the requested algorithm instance.
 * @throws std::invalid_argument if the provided name does not match any registered algorithm.
 */
std::shared_ptr<const IAlgorithm> AlgorithmFactory::get(const std::string& name) const {
    const auto iterator = algorithms_.find(name);
    if (iterator == algorithms_.end()) {
        throw std::invalid_argument("unknown algorithm: " + name);
    }
    return iterator->second;
}

}  // namespace av::algorithm