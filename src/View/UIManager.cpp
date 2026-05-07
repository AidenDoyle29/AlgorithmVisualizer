/**
 * @file UIManager.cpp
 * @brief Implements the main ImGui-driven application window.
 */
#include "View/UIManager.h"
#include "View/UIPlaybackBar.h"

using namespace ImGui;

UIManager::UIManager(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title)
{
    ImGui::SFML::Init(window);
}

void UIManager::run() {
    while (window.isOpen()) {
        handleEvents();
        ImGui::SFML::Update(window, deltaClock.restart());
        render();
    }
    shutdown();
}

void UIManager::handleEvents() {
    while (auto event = window.pollEvent()) {
        ImGui::SFML::ProcessEvent(window, *event);
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void UIManager::render() {
    ImGuiIO& io = ImGui::GetIO();

    // Title bar
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 40));
    ImGui::Begin("##titlebar", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar
    );
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("Algorithm Visualizer");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::End();

    // Sidebar
    ImGui::SetNextWindowPos(ImVec2(0, 40));
    ImGui::SetNextWindowSize(ImVec2(250, io.DisplaySize.y - 90));
    ImGui::Begin("##sidebar", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar
    );
    ImGui::Text("Algorithm");
    ImGui::Separator();
    ImGui::Button("BFS");
    ImGui::Text("Graph Controls");
    ImGui::Separator();
    ImGui::Button("Add Node");
    ImGui::Button("Add Edge");
    ImGui::End();

    playbackBar.draw();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
}

void UIManager::shutdown() {
    ImGui::SFML::Shutdown();
}
