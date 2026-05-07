/**
 * @file UITheme.h
 * @brief Defines the global visual design system and layout constants.
 *
 * This file centralizes all magic numbers, layout dimensions, and color palettes.
 * By decoupling these from the application logic, the View layer remains highly 
 * maintainable and paves the way for features like dynamic scaling or a "Dark Mode".
 */

#ifndef ALGORITHM_VISUALIZER_VIEW_UI_THEME_H
#define ALGORITHM_VISUALIZER_VIEW_UI_THEME_H

#include "Algorithm/Algorithm.h"
#include "Model/Edge.h"
#include "Model/Node.h"

#include <SFML/Graphics.hpp>
#include <cstdint>

namespace av::view::theme {

// --- Layout & Spatial Constants ---

constexpr float SidebarWidth = 372.0F;
constexpr float WorkspaceWidth = 1000.0F;
constexpr float WorkspaceHeight = 700.0F;
constexpr float CanvasPadding = 24.0F;
constexpr float NodeRadius = 22.0F;
constexpr float ButtonHeight = 30.0F;
constexpr float SectionGap = 8.0F;
constexpr float ButtonGap = 6.0F;
constexpr float SidebarScrollStep = 48.0F;

// --- Global Colors ---

const sf::Color BackgroundColor(246, 245, 240);

// --- State Color Resolvers ---

/**
 * @brief Safely applies an alpha (transparency) channel to an existing SFML color.
 * @param color The base RGB color.
 * @param alpha The opacity level (0 is invisible, 255 is solid).
 * @return sf::Color A new color containing the applied alpha channel.
 */
[[nodiscard]] sf::Color withAlpha(sf::Color color, std::uint8_t alpha);

/**
 * @brief Resolves the appropriate rendering color for a specific NodeState.
 */
[[nodiscard]] sf::Color nodeColor(model::NodeState state);

/**
 * @brief Resolves the appropriate rendering color for a specific EdgeState.
 */
[[nodiscard]] sf::Color edgeColor(model::EdgeState state);

/**
 * @brief Resolves the appropriate rendering color for a specific ArrayCellState.
 */
[[nodiscard]] sf::Color arrayColor(algorithm::ArrayCellState state);

}  // namespace av::view::theme

#endif