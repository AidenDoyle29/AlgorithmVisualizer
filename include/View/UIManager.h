//UIManager.h
#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>

class UIManager {
public:
    UIManager(unsigned int width, unsigned int height, const std::string& title);
    void run();
    void shutdown();

private:
    void handleEvents();
    void render();

    sf::RenderWindow window;
    sf::Clock deltaClock;
};