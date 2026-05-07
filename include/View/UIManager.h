/**
 * @file UIManager.h
 * @brief Declares the main ImGui-driven application window.
 */
#pragma once

#include "UIPlaybackBar.h"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <string>

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
    UIPlaybackBar playbackBar;
};
