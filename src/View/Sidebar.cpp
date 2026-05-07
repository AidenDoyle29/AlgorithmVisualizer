#include "View/Sidebar.h"
#include "View/UITheme.h"

#include <algorithm>
#include <sstream>

namespace av::view {

namespace {
std::string metricsSummary(const algorithm::AlgorithmMetrics& metrics) {
    std::ostringstream stream;
    stream << "Steps: " << metrics.stepsTaken << "\nVisited nodes: " << metrics.visitedNodes 
           << "\nEdges considered: " << metrics.edgesConsidered << "\nRelaxations: " << metrics.relaxations 
           << "\nComparisons: " << metrics.comparisons << "\nSwaps: " << metrics.swaps 
           << "\nWrites: " << metrics.writes << "\nPasses: " << metrics.passes 
           << "\nMST edges: " << metrics.mstEdgesChosen << "\nTotal weight: " << metrics.totalWeight 
           << "\nElapsed us: " << metrics.elapsedMicroseconds;
    return stream.str();
}

bool contains(const sf::FloatRect& rect, const sf::Vector2f point) {
    return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x && 
           point.y >= rect.position.y && point.y <= rect.position.y + rect.size.y;
}
} // namespace

void Sidebar::draw(
    SFMLRenderer& renderer, 
    const controller::InputHandler& handler,
    const playback::PlaybackSession& primarySession,
    const playback::PlaybackSession& secondarySession,
    const std::string& primaryAlgo,
    const std::string& secondaryAlgo,
    const std::string& statusMsg,
    sf::Vector2f windowSize) {

    clampScroll(handler, primaryAlgo, secondaryAlgo, windowSize);
    
    auto buttons = buildButtons(handler, primaryAlgo, secondaryAlgo, windowSize);
    drawButtons(renderer, buttons);

    float textY = getButtonsBottom(buttons) + 20.0F;
    const sf::Vector2f sidebarOrigin = {windowSize.x - theme::SidebarWidth, 0.0F};

    renderer.drawText("Inspector", sidebarOrigin + sf::Vector2f(16.0F, textY), 20U, sf::Color(247, 249, 252), true);
    textY += 28.0F;

    if (handler.mode() == controller::WorkspaceMode::Sort) {
        renderer.drawText("Array length: " + std::to_string(handler.sortingValues().size()), sidebarOrigin + sf::Vector2f(16.0F, textY), 15U, sf::Color(223, 233, 240));
        textY += 20.0F;
        renderer.drawText("Selected algorithm: " + primaryAlgo, sidebarOrigin + sf::Vector2f(16.0F, textY), 15U, sf::Color(223, 233, 240));
        textY += 20.0F;
    } else {
        renderer.drawText("Mode: Graph", sidebarOrigin + sf::Vector2f(16.0F, textY), 15U, sf::Color(223, 233, 240));
        textY += 20.0F;
        renderer.drawText("Selected algorithm: " + primaryAlgo, sidebarOrigin + sf::Vector2f(16.0F, textY), 15U, sf::Color(223, 233, 240));
        textY += 20.0F;
    }

    textY += 20.0F;
    renderer.drawText("Status", sidebarOrigin + sf::Vector2f(16.0F, textY), 20U, sf::Color(247, 249, 252), true);
    textY += 24.0F;
    renderer.drawMultilineText(statusMsg, sidebarOrigin + sf::Vector2f(16.0F, textY), 14U, sf::Color(223, 233, 240));

    drawMetricsPanel(renderer, primarySession, secondarySession, textY + 80.0F, !secondaryAlgo.empty(), windowSize);
    drawScrollbar(renderer, handler, primaryAlgo, secondaryAlgo, windowSize);
}

std::string Sidebar::handleMouseClick(sf::Vector2f position, const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) {
    const auto buttons = buildButtons(handler, primaryAlgo, secondaryAlgo, windowSize);
    for (const auto& button : buttons) {
        if (contains(button.rect, position)) return button.id;
    }
    return "";
}

void Sidebar::handleScroll(float delta, const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) {
    scrollOffset_ -= delta * theme::SidebarScrollStep;
    clampScroll(handler, primaryAlgo, secondaryAlgo, windowSize);
}

void Sidebar::clampScroll(const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) {
    const float contentHeight = getContentHeight(handler, primaryAlgo, secondaryAlgo, windowSize);
    const float maxScroll = std::max(0.0F, contentHeight - windowSize.y);
    scrollOffset_ = std::clamp(scrollOffset_, 0.0F, maxScroll);
}

float Sidebar::getContentHeight(const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) const {
    Sidebar tempSidebar;
    const auto buttons = tempSidebar.buildButtons(handler, primaryAlgo, secondaryAlgo, windowSize);
    float contentY = tempSidebar.getButtonsBottom(buttons) + 20.0F;
    contentY += 28.0F + 40.0F + 20.0F; 
    contentY += 20.0F + 24.0F + 60.0F; 
    contentY += 80.0F + 230.0F + 20.0F; 
    return contentY;
}

void Sidebar::drawButtons(SFMLRenderer& renderer, const std::vector<ButtonSpec>& buttons) {
    for (const auto& button : buttons) {
        sf::Color fill = button.active ? sf::Color(255, 196, 61) : sf::Color(255, 255, 255, 10);
        sf::Color textColor = button.active ? sf::Color(16, 27, 43) : sf::Color(232, 239, 244);
        renderer.drawPanel(button.rect, fill, sf::Color(255, 255, 255, 16), 1.0F, {0.0F, 5.0F}, sf::Color(4, 10, 18, 28));
        renderer.drawText(button.label, button.rect.position + sf::Vector2f(12.0F, 6.0F), 14U, textColor, button.active);
    }
}

std::vector<ButtonSpec> Sidebar::buildButtons(const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) const {
    std::vector<ButtonSpec> buttons;
    const float x = windowSize.x - theme::SidebarWidth + 16.0F; 
    float y = 18.0F - scrollOffset_;
    const float width = theme::SidebarWidth - 32.0F;
    const float halfWidth = (width - theme::ButtonGap) / 2.0F;

    auto pushButton = [&](std::string id, std::string label, bool active = false) {
        buttons.push_back({id, label, {{x, y}, {width, theme::ButtonHeight}}, active});
        y += theme::ButtonHeight + theme::ButtonGap;
    };

    auto pushRow = [&](std::string idLeft, std::string labelLeft, bool activeLeft, std::string idRight, std::string labelRight, bool activeRight) {
        buttons.push_back({idLeft, labelLeft, {{x, y}, {halfWidth, theme::ButtonHeight}}, activeLeft});
        buttons.push_back({idRight, labelRight, {{x + halfWidth + theme::ButtonGap, y}, {halfWidth, theme::ButtonHeight}}, activeRight});
        y += theme::ButtonHeight + theme::ButtonGap;
    };

    pushRow("mode_graph", "Mode: Graph", handler.mode() == controller::WorkspaceMode::Graph, 
            "mode_sort", "Mode: Sort", handler.mode() == controller::WorkspaceMode::Sort);
    y += theme::SectionGap;

    if (handler.mode() == controller::WorkspaceMode::Sort) {
        pushRow("array_shuffle", "Shuffle", false, "array_randomize", "Randomize", false);
        pushRow("array_sorted", "Sorted Preset", false, "array_reverse", "Reverse Preset", false);
        pushRow("array_size_minus", "Size -", false, "array_size_plus", "Size +", false);
        y += theme::SectionGap;
        pushButton("algorithm_bubble", "Primary: Bubble", primaryAlgo == "Bubble");
        pushButton("algorithm_selection", "Primary: Selection", primaryAlgo == "Selection");
        pushButton("algorithm_insertion", "Primary: Insertion", primaryAlgo == "Insertion");
        pushButton("algorithm_merge", "Primary: Merge", primaryAlgo == "Merge");
        pushButton("algorithm_quick", "Primary: Quick", primaryAlgo == "Quick");
    } else {
        pushButton("mode_select", "Mode: Select / Move", false);
        pushButton("mode_add_node", "Mode: Add Node", false);
        pushButton("mode_add_edge", "Mode: Add Edge", false);
        pushButton("mode_delete", "Mode: Delete", false);
        y += theme::SectionGap;
        pushButton("load_sample", "Load Sample Graph");
        pushButton("clear_graph", "Clear Graph");
        y += theme::SectionGap;
        pushButton("algorithm_bfs", "Primary: BFS", primaryAlgo == "BFS");
        pushButton("algorithm_dfs", "Primary: DFS", primaryAlgo == "DFS");
        pushButton("algorithm_dijkstra", "Primary: Dijkstra", primaryAlgo == "Dijkstra");
        pushButton("algorithm_kruskal", "Primary: Kruskal", primaryAlgo == "Kruskal");
    }

    y += theme::SectionGap;
    pushButton("run", "Run");
    pushRow("play_pause", "Play / Pause", false, "reset", "Reset Playback", false);
    pushRow("step_back", "Step Back", false, "step_forward", "Step Forward", false);
    
    return buttons;
}

float Sidebar::getButtonsBottom(const std::vector<ButtonSpec>& buttons) const {
    if (buttons.empty()) return 18.0F;
    return buttons.back().rect.position.y + buttons.back().rect.size.y;
}

void Sidebar::drawMetricsPanel(
    SFMLRenderer& renderer, 
    const playback::PlaybackSession& primary, 
    const playback::PlaybackSession& secondary, 
    float topY, 
    bool comparisonEnabled,
    sf::Vector2f windowSize) {
    
    const float x = windowSize.x - theme::SidebarWidth + 16.0F;
    const float panelHeight = 230.0F;
    
    renderer.drawPanel({{x, topY}, {theme::SidebarWidth - 32.0F, panelHeight}}, sf::Color(20, 33, 48, 204), sf::Color(255, 255, 255, 22));
    renderer.drawText("Metrics", {x + 14.0F, topY + 12.0F}, 20U, sf::Color(248, 250, 252), true);

    if (!primary.empty()) {
        renderer.drawText("Primary Run", {x + 14.0F, topY + 44.0F}, 14U, sf::Color(255, 214, 92), true);
        renderer.drawMultilineText(metricsSummary(primary.currentFrame().metrics), {x + 14.0F, topY + 68.0F}, 13U, sf::Color(225, 234, 242));
    } else {
        renderer.drawMultilineText("No playback loaded.\nSetup your workspace and press Run.", {x + 14.0F, topY + 44.0F}, 13U, sf::Color(225, 234, 242));
    }
}

void Sidebar::drawScrollbar(SFMLRenderer& renderer, const controller::InputHandler& handler, const std::string& primaryAlgo, const std::string& secondaryAlgo, sf::Vector2f windowSize) {
    const float maxScroll = std::max(0.0F, getContentHeight(handler, primaryAlgo, secondaryAlgo, windowSize) - windowSize.y);
    if (maxScroll <= 0.0F) return;

    const sf::FloatRect trackRect {{windowSize.x - 10.0F, 14.0F}, {4.0F, windowSize.y - 28.0F}};
    renderer.drawPanel(trackRect, sf::Color(255, 255, 255, 20), sf::Color::Transparent, 0.0F, {0.0F, 0.0F}, sf::Color::Transparent);

    const float viewportRatio = std::clamp(windowSize.y / getContentHeight(handler, primaryAlgo, secondaryAlgo, windowSize), 0.12F, 1.0F);
    const float thumbHeight = trackRect.size.y * viewportRatio;
    const float scrollProgress = scrollOffset_ / maxScroll;
    const float thumbY = trackRect.position.y + (trackRect.size.y - thumbHeight) * scrollProgress;

    renderer.drawPanel({{trackRect.position.x, thumbY}, {trackRect.size.x, thumbHeight}}, sf::Color(255, 196, 61, 170), sf::Color::Transparent, 0.0F, {0.0F, 0.0F}, sf::Color::Transparent);
}

} // namespace av::view