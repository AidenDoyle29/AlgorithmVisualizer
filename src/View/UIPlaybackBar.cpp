/**
 * @file UIPlaybackBar.cpp
 * @brief Implements the playback control bar for the UI.
 */
#include "View/UIPlaybackBar.h"

using namespace ImGui;

void UIPlaybackBar::draw() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - 50));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 50));
    ImGui::Begin("##playbar", nullptr, 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoTitleBar
    );

    ImGui::Button("Play");
    ImGui::SameLine();
    ImGui::Button("Step");
    ImGui::SameLine();
    ImGui::Button("Reset");

    ImGui::End();
}
