/**
 * @file SFMLRenderer.h
 * @brief Defines the visual rendering engine for the application.
 *
 * This file encapsulates all low-level SFML draw commands. It is completely 
 * decoupled from the application's core logic and state management, acting 
 * strictly as a painter that translates data models into pixels on the screen.
 */

#ifndef ALGORITHM_VISUALIZER_VIEW_SFML_RENDERER_H
#define ALGORITHM_VISUALIZER_VIEW_SFML_RENDERER_H

#include "Algorithm/Algorithm.h"
#include "Model/Graph.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace av::view {

class SFMLRenderer {
  public:
    /**
     * @brief Constructs the renderer and loads required system fonts.
     * @param window The active SFML window to draw to.
     */
    explicit SFMLRenderer(sf::RenderWindow& window);

    // --- High-Level Component Rendering ---

    void drawBackdrop();
    void drawHeader(const std::string& primaryTag, const std::string& secondaryTag);
    void drawGraphCanvas(
        const sf::FloatRect& rect, 
        const model::Graph& graph, 
        const std::string& title, 
        const std::string& selectedNode, 
        const std::string& selectedEdge,
        const std::string& startNodeId,
        const std::string& goalNodeId);
    void drawArrayCanvas(
        const sf::FloatRect& rect, 
        const std::vector<int>& values, 
        const std::vector<algorithm::ArrayCellState>& states, 
        const std::string& title);
    
    // --- UI Primitives (Temporarily public for the Sidebar transition) ---

    void drawPanel(const sf::FloatRect& rect, sf::Color fill, sf::Color outline, float outlineThickness = 1.0F, sf::Vector2f shadowOffset = {0.0F, 8.0F}, sf::Color shadowColor = sf::Color(7, 16, 28, 32));
    void drawGradientRect(const sf::FloatRect& rect, sf::Color topLeft, sf::Color topRight, sf::Color bottomRight, sf::Color bottomLeft);
    void drawText(const std::string& value, sf::Vector2f position, unsigned int size, sf::Color color, bool bold = false);
    void drawMultilineText(const std::string& value, sf::Vector2f position, unsigned int size, sf::Color color);
    void drawTag(sf::Vector2f position, const std::string& label, sf::Color color);
    void drawPendingEdgePreview(const sf::Vector2f& startWorldPos, const sf::Vector2f& mouseScreenPos, const sf::FloatRect& canvasRect);

    // --- Spatial Utilities ---

    [[nodiscard]] sf::Vector2f worldToScreen(const sf::Vector2f point, const sf::FloatRect& canvas) const;

  private:
    sf::RenderWindow& window_;
    sf::Font font_;

    void loadFont();
    void drawCanvasGrid(const sf::FloatRect& rect);
    void drawEdges(const model::Graph& graph, const sf::FloatRect& rect, const std::string& selectedEdge);
    void drawNodes(const model::Graph& graph, const sf::FloatRect& rect, const std::string& selectedNode, const std::string& startNodeId, const std::string& goalNodeId);
    void drawArrowHead(sf::Vector2f start, sf::Vector2f end, sf::Color color);
};

}  // namespace av::view

#endif