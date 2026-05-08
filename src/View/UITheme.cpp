#include "View/UITheme.h"

namespace av::view::theme {

sf::Color withAlpha(const sf::Color color, const std::uint8_t alpha) {
    return sf::Color(color.r, color.g, color.b, alpha);
}

sf::Color nodeColor(const model::NodeState state) {
    switch (state) {
        case model::NodeState::Normal:
            return sf::Color(244, 246, 250);
        case model::NodeState::Start:
            return sf::Color(74, 144, 226);
        case model::NodeState::Frontier:
            return sf::Color(247, 181, 56);
        case model::NodeState::Active:
            return sf::Color(255, 107, 107);
        case model::NodeState::Visited:
            return sf::Color(74, 170, 165);
        case model::NodeState::Path:
            return sf::Color(35, 196, 131);
        case model::NodeState::MinimumSpanningTree:
            return sf::Color(98, 111, 255);
    }
    return sf::Color::White;
}

sf::Color edgeColor(const model::EdgeState state) {
    switch (state) {
        case model::EdgeState::Normal:
            return sf::Color(145, 155, 175);
        case model::EdgeState::Frontier:
            return sf::Color(255, 184, 77);
        case model::EdgeState::Active:
            return sf::Color(255, 107, 107);
        case model::EdgeState::Traversed:
            return sf::Color(35, 196, 131);
        case model::EdgeState::Relaxed:
            return sf::Color(77, 145, 255);
        case model::EdgeState::MinimumSpanningTree:
            return sf::Color(98, 111, 255);
    }
    return sf::Color::White;
}

sf::Color arrayColor(const algorithm::ArrayCellState state) {
    switch (state) {
        case algorithm::ArrayCellState::Normal:
            return sf::Color(99, 121, 145);
        case algorithm::ArrayCellState::Active:
            return sf::Color(84, 160, 255);
        case algorithm::ArrayCellState::Compared:
            return sf::Color(255, 196, 61);
        case algorithm::ArrayCellState::Swapped:
            return sf::Color(255, 107, 107);
        case algorithm::ArrayCellState::Sorted:
            return sf::Color(35, 196, 131);
        case algorithm::ArrayCellState::Pivot:
            return sf::Color(156, 102, 255);
    }
    return sf::Color(99, 121, 145);
}

}  // namespace av::view::theme