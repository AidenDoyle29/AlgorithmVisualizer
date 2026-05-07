/**
 * @file Sidebar.h
 * @brief Manages the layout, state, and rendering of the application's control panel.
 */

#ifndef ALGORITHM_VISUALIZER_VIEW_SIDEBAR_H
#define ALGORITHM_VISUALIZER_VIEW_SIDEBAR_H

#include "Controller/InputHandler.h"
#include "View/SFMLRenderer.h"
#include "Playback/PlaybackSession.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace av::view {

/**
 * @struct ButtonSpec
 * @brief Defines the metadata, label, and physical screen bounds for a UI button.
 */
struct ButtonSpec {
    std::string id;       /**< Unique identifier for the button. */
    std::string label;    /**< Display text rendered on the button. */
    sf::FloatRect rect;   /**< The physical bounds of the button on screen. */
    bool active {false};  /**< Whether the button is currently selected or active. */
};

/**
 * @class Sidebar
 * @brief Manages the layout, state, and rendering of the application's right-hand control panel.
 */
class Sidebar {
  public:
    Sidebar() = default;

    /**
     * @brief Renders the entire sidebar, including buttons, inspector, and metrics.
     * * @param renderer The SFMLRenderer instance used for drawing operations.
     * @param handler The input handler containing the current workspace state.
     * @param primarySession The playback session for the primary algorithm.
     * @param secondarySession The playback session for the secondary (comparison) algorithm.
     * @param primaryAlgo The name of the currently selected primary algorithm.
     * @param secondaryAlgo The name of the currently selected secondary algorithm.
     * @param statusMsg The current status message to display in the inspector.
     * @param windowSize The current dimensions of the application window.
     */
    void draw(
        SFMLRenderer& renderer, 
        const controller::InputHandler& handler,
        const playback::PlaybackSession& primarySession,
        const playback::PlaybackSession& secondarySession,
        const std::string& primaryAlgo,
        const std::string& secondaryAlgo,
        const std::string& statusMsg,
        sf::Vector2f windowSize);

    /**
     * @brief Checks if a mouse click landed on any sidebar button.
     * * @param position The screen coordinates of the mouse click.
     * @param handler The input handler containing the current workspace state.
     * @param primaryAlgo The name of the currently selected primary algorithm.
     * @param secondaryAlgo The name of the currently selected secondary algorithm.
     * @param windowSize The current dimensions of the application window.
     * @return std::string The ID of the clicked button, or an empty string if no button was clicked.
     */
    [[nodiscard]] std::string handleMouseClick(
        sf::Vector2f position, 
        const controller::InputHandler& handler, 
        const std::string& primaryAlgo, 
        const std::string& secondaryAlgo,
        sf::Vector2f windowSize);
    
    /**
     * @brief Processes scroll wheel input to move the sidebar content up or down.
     * * @param delta The scroll wheel delta amount.
     * @param handler The input handler containing the current workspace state.
     * @param primaryAlgo The name of the currently selected primary algorithm.
     * @param secondaryAlgo The name of the currently selected secondary algorithm.
     * @param windowSize The current dimensions of the application window.
     */
    void handleScroll(
        float delta, 
        const controller::InputHandler& handler, 
        const std::string& primaryAlgo, 
        const std::string& secondaryAlgo,
        sf::Vector2f windowSize);

  private:
    float scrollOffset_ {0.0F}; /**< The current vertical scroll offset of the sidebar content. */

    /**
     * @brief Generates the layout and specifications for all currently visible buttons.
     * * @param handler The input handler containing the current workspace state.
     * @param primaryAlgo The currently selected primary algorithm.
     * @param secondaryAlgo The currently selected secondary algorithm.
     * @param windowSize The current dimensions of the application window.
     * @return std::vector<ButtonSpec> A list of configured buttons with screen coordinates.
     */
    [[nodiscard]] std::vector<ButtonSpec> buildButtons(const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) const;
    
    /**
     * @brief Calculates the total vertical height of all sidebar content.
     * * @param handler The input handler containing the current workspace state.
     * @param primaryAlgo The currently selected primary algorithm.
     * @param secondaryAlgo The currently selected secondary algorithm.
     * @param windowSize The current dimensions of the application window.
     * @return float The total height in pixels.
     */
    [[nodiscard]] float getContentHeight(const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) const;
    
    /**
     * @brief Gets the Y-coordinate of the bottom of the last button in the list.
     * * @param buttons The list of currently generated buttons.
     * @return float The Y-coordinate in pixels.
     */
    [[nodiscard]] float getButtonsBottom(const std::vector<ButtonSpec>& buttons) const;
    
    /**
     * @brief Restricts the scroll offset to valid bounds based on content height and window size.
     * * @param handler The input handler containing the current workspace state.
     * @param primaryAlgo The currently selected primary algorithm.
     * @param secondaryAlgo The currently selected secondary algorithm.
     * @param windowSize The current dimensions of the application window.
     */
    void clampScroll(const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize);

    /**
     * @brief Renders the list of generated buttons.
     * * @param renderer The SFMLRenderer instance.
     * @param buttons The list of buttons to draw.
     */
    void drawButtons(SFMLRenderer& renderer, const std::vector<ButtonSpec>& buttons);
    
    /**
     * @brief Renders the metrics and statistics panel for algorithm runs.
     * * @param renderer The SFMLRenderer instance.
     * @param primary The playback session for the primary algorithm.
     * @param secondary The playback session for the secondary algorithm.
     * @param topY The starting Y-coordinate for the panel.
     * @param comparisonEnabled Whether comparison mode is active.
     * @param windowSize The current dimensions of the application window.
     */
    void drawMetricsPanel(SFMLRenderer& renderer, const playback::PlaybackSession& primary, const playback::PlaybackSession& secondary, float topY, bool comparisonEnabled, sf::Vector2f windowSize);
    
    /**
     * @brief Renders the interactive scrollbar track and thumb.
     * * @param renderer The SFMLRenderer instance.
     * @param handler The input handler containing the current workspace state.
     * @param primaryAlgo The currently selected primary algorithm.
     * @param secondaryAlgo The currently selected secondary algorithm.
     * @param windowSize The current dimensions of the application window.
     */
    void drawScrollbar(SFMLRenderer& renderer, const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize);
};

} // namespace av::view

#endif