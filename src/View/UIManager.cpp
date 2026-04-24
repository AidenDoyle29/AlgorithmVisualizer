//UIManager.cpp
#include "View/UIManager.h"

UIManager::UIManager(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode(width, height), title)
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
    sf::Event event;
    while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(window, event);
        if (event.type == sf::Event::Closed) window.close();
    }
}

void UIManager::render() {
    ImGui::Begin("Settings");
    ImGui::Text("Ready to visualize!");
    ImGui::End();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
}

void UIManager::shutdown() {
    ImGui::SFML::Shutdown();
}