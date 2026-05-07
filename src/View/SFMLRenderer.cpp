#include "View/SFMLRenderer.h"
#include "View/UITheme.h"

#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace av::view {

namespace {
std::vector<std::filesystem::path> fontCandidates() {
    return {
        "/System/Library/Fonts/Supplemental/Futura.ttc",
        "/System/Library/Fonts/Supplemental/GillSans.ttc",
        "/System/Library/Fonts/Supplemental/AmericanTypewriter.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/SFNS.ttf"};
}
}

SFMLRenderer::SFMLRenderer(sf::RenderWindow& window) : window_(window) {
    loadFont();
}

void SFMLRenderer::loadFont() {
    for (const auto& candidate : fontCandidates()) {
        if (std::filesystem::exists(candidate) && font_.openFromFile(candidate)) {
            return;
        }
    }
    throw std::runtime_error("Unable to load a system font for the SFML UI");
}

void SFMLRenderer::drawGradientRect(
    const sf::FloatRect& rect, const sf::Color topLeft, const sf::Color topRight,
    const sf::Color bottomRight, const sf::Color bottomLeft) {
    sf::VertexArray quad(sf::PrimitiveType::TriangleStrip, 4);
    quad[0].position = rect.position;
    quad[1].position = {rect.position.x + rect.size.x, rect.position.y};
    quad[2].position = {rect.position.x, rect.position.y + rect.size.y};
    quad[3].position = {rect.position.x + rect.size.x, rect.position.y + rect.size.y};
    quad[0].color = topLeft;
    quad[1].color = topRight;
    quad[2].color = bottomLeft;
    quad[3].color = bottomRight;
    window_.draw(quad);
}

void SFMLRenderer::drawPanel(
    const sf::FloatRect& rect, const sf::Color fill, const sf::Color outline,
    const float outlineThickness, const sf::Vector2f shadowOffset, const sf::Color shadowColor) {
    sf::RectangleShape shadow(rect.size);
    shadow.setPosition(rect.position + shadowOffset);
    shadow.setFillColor(shadowColor);
    window_.draw(shadow);

    sf::RectangleShape box(rect.size);
    box.setPosition(rect.position);
    box.setFillColor(fill);
    box.setOutlineColor(outline);
    box.setOutlineThickness(outlineThickness);
    window_.draw(box);
}

void SFMLRenderer::drawCanvasGrid(const sf::FloatRect& rect) {
    const float left = rect.position.x + 14.0F;
    const float right = rect.position.x + rect.size.x - 14.0F;
    const float top = rect.position.y + 70.0F;
    const float bottom = rect.position.y + rect.size.y - 14.0F;

    for (int index = 0; index < 10; ++index) {
        const float x = left + (right - left) * static_cast<float>(index) / 9.0F;
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = {x, top};
        line[1].position = {x, bottom};
        line[0].color = sf::Color(226, 232, 239, 70);
        line[1].color = sf::Color(226, 232, 239, 70);
        window_.draw(line);
    }
    for (int index = 0; index < 7; ++index) {
        const float y = top + (bottom - top) * static_cast<float>(index) / 6.0F;
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = {left, y};
        line[1].position = {right, y};
        line[0].color = sf::Color(226, 232, 239, 60);
        line[1].color = sf::Color(226, 232, 239, 60);
        window_.draw(line);
    }
}

sf::Vector2f SFMLRenderer::worldToScreen(const sf::Vector2f point, const sf::FloatRect& canvas) const {
    return {
        canvas.position.x + theme::CanvasPadding + point.x / theme::WorkspaceWidth * (canvas.size.x - 2.0F * theme::CanvasPadding),
        canvas.position.y + theme::CanvasPadding + point.y / theme::WorkspaceHeight * (canvas.size.y - 2.0F * theme::CanvasPadding)};
}

void SFMLRenderer::drawBackdrop() {
    const sf::Vector2f size(window_.getSize());
    drawGradientRect(
        {{0.0F, 0.0F}, size},
        sf::Color(247, 241, 232), sf::Color(239, 247, 248),
        sf::Color(232, 239, 246), sf::Color(244, 236, 228));

    sf::CircleShape orbOne(180.0F, 80);
    orbOne.setPosition({-60.0F, 80.0F});
    orbOne.setFillColor(sf::Color(255, 196, 61, 32));
    window_.draw(orbOne);

    sf::CircleShape orbTwo(220.0F, 80);
    orbTwo.setPosition({780.0F, -40.0F});
    orbTwo.setFillColor(sf::Color(74, 144, 226, 28));
    window_.draw(orbTwo);

    drawGradientRect(
        {{0.0F, 0.0F}, {size.x, 92.0F}},
        sf::Color(17, 31, 48), sf::Color(18, 55, 79),
        sf::Color(16, 37, 58), sf::Color(16, 27, 43));

    drawGradientRect(
        {{size.x - theme::SidebarWidth, 0.0F}, {theme::SidebarWidth, size.y}},
        sf::Color(15, 25, 39), sf::Color(16, 35, 52),
        sf::Color(11, 21, 34), sf::Color(12, 24, 38));

    sf::RectangleShape divider({1.0F, size.y});
    divider.setPosition({size.x - theme::SidebarWidth, 0.0F});
    divider.setFillColor(sf::Color(255, 255, 255, 22));
    window_.draw(divider);
}

void SFMLRenderer::drawHeader(const std::string& primaryTag, const std::string& secondaryTag) {
    drawText("Algorithm Visualizer", {28.0F, 18.0F}, 36U, sf::Color(250, 252, 255), true);
    drawText("Graph and sorting studio for algorithms, playback, metrics, comparisons, and quizzes", {30.0F, 56.0F}, 14U, sf::Color(199, 216, 231));
    drawTag({690.0F, 20.0F}, primaryTag, sf::Color(255, 196, 61));
    drawTag({760.0F, 20.0F}, secondaryTag, sf::Color(74, 144, 226));
}

void SFMLRenderer::drawGraphCanvas(
    const sf::FloatRect& rect, const model::Graph& graph, const std::string& title, 
    const std::string& selectedNode, const std::string& selectedEdge, 
    const std::string& startNodeId, const std::string& goalNodeId) {
    
    drawPanel(rect, sf::Color(252, 251, 248), sf::Color(210, 218, 228), 1.0F, {0.0F, 12.0F}, sf::Color(23, 36, 52, 24));
    sf::FloatRect accentRect {{rect.position.x, rect.position.y}, {rect.size.x, 8.0F}};
    drawGradientRect(accentRect, sf::Color(74, 144, 226), sf::Color(255, 196, 61), sf::Color(255, 196, 61), sf::Color(74, 144, 226));
    drawCanvasGrid(rect);
    drawText(title, rect.position + sf::Vector2f(18.0F, 18.0F), 24U, sf::Color(17, 31, 48), true);
    drawText("Click to edit. Set start/goal from the sidebar. Space = play/pause, arrows = step.", rect.position + sf::Vector2f(18.0F, 48.0F), 13U, sf::Color(110, 123, 137));

    drawEdges(graph, rect, selectedEdge);
    drawNodes(graph, rect, selectedNode, startNodeId, goalNodeId);
}

void SFMLRenderer::drawArrayCanvas(
    const sf::FloatRect& rect, const std::vector<int>& values, 
    const std::vector<algorithm::ArrayCellState>& states, const std::string& title) {
    
    drawPanel(rect, sf::Color(251, 250, 245), sf::Color(210, 218, 228), 1.0F, {0.0F, 12.0F}, sf::Color(23, 36, 52, 24));
    sf::FloatRect accentRect {{rect.position.x, rect.position.y}, {rect.size.x, 8.0F}};
    drawGradientRect(accentRect, sf::Color(84, 160, 255), sf::Color(255, 196, 61), sf::Color(35, 196, 131), sf::Color(156, 102, 255));
    drawText(title, rect.position + sf::Vector2f(18.0F, 18.0F), 24U, sf::Color(17, 31, 48), true);
    drawText("Type an array, load a preset, then run. Space = play/pause, arrows = step.", rect.position + sf::Vector2f(18.0F, 48.0F), 13U, sf::Color(110, 123, 137));

    const float innerLeft = rect.position.x + 28.0F;
    const float innerTop = rect.position.y + 82.0F;
    const float innerWidth = rect.size.x - 56.0F;
    const float innerHeight = rect.size.y - 116.0F;
    const float baseline = innerTop + innerHeight;

    if (values.empty()) {
        drawText("No values loaded.", {innerLeft, innerTop + 20.0F}, 18U, sf::Color(110, 123, 137), true);
        return;
    }

    const int maxValue = std::max(1, *std::max_element(values.begin(), values.end()));
    const float gap = 8.0F;
    const float barWidth = std::max(18.0F, (innerWidth - gap * static_cast<float>(values.size() - 1U)) / static_cast<float>(values.size()));

    for (std::size_t index = 0; index < values.size(); ++index) {
        const float heightRatio = static_cast<float>(values[index]) / static_cast<float>(maxValue);
        const float barHeight = std::max(16.0F, heightRatio * (innerHeight - 32.0F));
        const float x = innerLeft + static_cast<float>(index) * (barWidth + gap);
        const float y = baseline - barHeight;
        const algorithm::ArrayCellState state = index < states.size() ? states[index] : algorithm::ArrayCellState::Normal;

        sf::RectangleShape bar({barWidth, barHeight});
        bar.setPosition({x, y});
        bar.setFillColor(theme::arrayColor(state));
        bar.setOutlineColor(sf::Color(255, 255, 255, 90));
        bar.setOutlineThickness(1.0F);
        window_.draw(bar);

        sf::Text valueText(font_, std::to_string(values[index]), 14U);
        valueText.setFillColor(sf::Color(17, 31, 48));
        valueText.setStyle(sf::Text::Bold);
        const sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setPosition({x + (barWidth - bounds.size.x) / 2.0F, y - 20.0F});
        window_.draw(valueText);

        sf::Text indexText(font_, std::to_string(index), 11U);
        indexText.setFillColor(sf::Color(110, 123, 137));
        indexText.setPosition({x + (barWidth - 10.0F) / 2.0F, baseline + 4.0F});
        window_.draw(indexText);
    }
}

void SFMLRenderer::drawEdges(const model::Graph& graph, const sf::FloatRect& rect, const std::string& selectedEdge) {
    for (const auto& edge : graph.edges()) {
        const model::Node& from = graph.node(edge.from);
        const model::Node& to = graph.node(edge.to);
        const sf::Vector2f start = worldToScreen({static_cast<float>(from.x), static_cast<float>(from.y)}, rect);
        const sf::Vector2f end = worldToScreen({static_cast<float>(to.x), static_cast<float>(to.y)}, rect);

        sf::VertexArray shadow(sf::PrimitiveType::Lines, 2);
        shadow[0].position = start + sf::Vector2f(0.0F, 2.0F);
        shadow[1].position = end + sf::Vector2f(0.0F, 2.0F);
        shadow[0].color = sf::Color(14, 24, 38, 28);
        shadow[1].color = sf::Color(14, 24, 38, 28);
        window_.draw(shadow);

        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = start;
        line[1].position = end;
        const sf::Color color = edge.id == selectedEdge ? sf::Color(255, 120, 86) : theme::edgeColor(edge.state);
        line[0].color = color;
        line[1].color = color;
        window_.draw(line);

        if (edge.directed) {
            drawArrowHead(start, end, color);
        }

        std::ostringstream weight;
        weight << edge.weight;
        const sf::Vector2f midpoint {(start.x + end.x) / 2.0F, (start.y + end.y) / 2.0F};
        sf::CircleShape badge(14.0F, 24);
        badge.setOrigin({14.0F, 14.0F});
        badge.setPosition(midpoint);
        badge.setFillColor(sf::Color(255, 255, 255));
        badge.setOutlineColor(theme::withAlpha(color, 190));
        badge.setOutlineThickness(1.0F);
        window_.draw(badge);
        drawText(weight.str(), midpoint + sf::Vector2f(-8.0F, -8.0F), 12U, sf::Color(17, 31, 48), true);
    }
}

void SFMLRenderer::drawArrowHead(const sf::Vector2f start, const sf::Vector2f end, const sf::Color color) {
    const sf::Vector2f direction = end - start;
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length <= 0.001F) return;

    const sf::Vector2f unit = direction / length;
    const sf::Vector2f base = end - unit * (theme::NodeRadius + 8.0F);
    const sf::Vector2f left {-unit.y, unit.x};

    sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);
    triangle[0].position = base;
    triangle[1].position = base - unit * 12.0F + left * 6.0F;
    triangle[2].position = base - unit * 12.0F - left * 6.0F;
    triangle[0].color = color;
    triangle[1].color = color;
    triangle[2].color = color;
    window_.draw(triangle);
}

void SFMLRenderer::drawNodes(
    const model::Graph& graph, const sf::FloatRect& rect, 
    const std::string& selectedNode, const std::string& startNodeId, const std::string& goalNodeId) {
    for (const auto& node : graph.nodes()) {
        const sf::Vector2f position = worldToScreen({static_cast<float>(node.x), static_cast<float>(node.y)}, rect);

        sf::CircleShape glow(theme::NodeRadius + 8.0F, 64);
        glow.setOrigin({theme::NodeRadius + 8.0F, theme::NodeRadius + 8.0F});
        glow.setPosition(position);
        glow.setFillColor(theme::withAlpha(theme::nodeColor(node.state), 34));
        window_.draw(glow);

        sf::CircleShape circle(theme::NodeRadius, 48);
        circle.setOrigin({theme::NodeRadius, theme::NodeRadius});
        circle.setPosition(position);
        circle.setFillColor(theme::nodeColor(node.state));
        circle.setOutlineColor(node.id == selectedNode ? sf::Color(17, 31, 48) : sf::Color(219, 226, 235));
        circle.setOutlineThickness(node.id == selectedNode ? 4.0F : 2.0F);
        window_.draw(circle);

        drawText(node.displayLabel(), position + sf::Vector2f(-8.0F, -12.0F), 18U, sf::Color(17, 31, 48), true);

        if (node.id == startNodeId) {
            drawTag(position + sf::Vector2f(-28.0F, -44.0F), "START", sf::Color(84, 160, 255));
        }
        if (node.id == goalNodeId) {
            drawTag(position + sf::Vector2f(8.0F, -44.0F), "GOAL", sf::Color(255, 159, 67));
        }
        if (!node.annotation.empty()) {
            drawText(node.annotation, position + sf::Vector2f(-30.0F, 32.0F), 12U, sf::Color(110, 123, 137));
        }
    }
}

void SFMLRenderer::drawTag(const sf::Vector2f position, const std::string& label, const sf::Color color) {
    const sf::FloatRect rect {position, {58.0F, 20.0F}};
    drawPanel(rect, color, theme::withAlpha(sf::Color::White, 90), 1.0F, {0.0F, 4.0F}, sf::Color(8, 16, 26, 32));
    drawText(label, position + sf::Vector2f(8.0F, 2.0F), 11U, sf::Color::White, true);
}

void SFMLRenderer::drawPendingEdgePreview(const sf::Vector2f& startWorldPos, const sf::Vector2f& mouseScreenPos, const sf::FloatRect& canvasRect) {
    const sf::Vector2f start = worldToScreen(startWorldPos, canvasRect);
    sf::VertexArray preview(sf::PrimitiveType::Lines, 2);
    preview[0].position = start;
    preview[1].position = mouseScreenPos;
    preview[0].color = sf::Color(255, 159, 67, 220);
    preview[1].color = sf::Color(255, 159, 67, 120);
    window_.draw(preview);
}

void SFMLRenderer::drawText(const std::string& value, const sf::Vector2f position, const unsigned int size, const sf::Color color, const bool bold) {
    sf::Text text(font_, value, size);
    text.setPosition(position);
    text.setFillColor(color);
    if (bold) text.setStyle(sf::Text::Bold);
    window_.draw(text);
}

void SFMLRenderer::drawMultilineText(const std::string& value, const sf::Vector2f position, const unsigned int size, const sf::Color color) {
    sf::Text text(font_, value, size);
    text.setPosition(position);
    text.setFillColor(color);
    window_.draw(text);
}

}  // namespace av::view