#pragma once

#include "imgui.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace ImGui {
namespace SFML {

inline void Init(sf::RenderWindow&) {}
inline void Update(sf::RenderWindow&, sf::Time const&) {}
inline void ProcessEvent(sf::RenderWindow&, sf::Event const&) {}
inline void Render(sf::RenderWindow&) {}
inline void Shutdown() {}

}  // namespace SFML
}  // namespace ImGui
