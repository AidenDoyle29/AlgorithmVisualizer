#pragma once

#include <SFML/System/Vector2.hpp>

struct ImVec2 {
    float x {0.0F};
    float y {0.0F};

    ImVec2() = default;
    ImVec2(float xValue, float yValue) : x(xValue), y(yValue) {}
};

struct ImGuiIO {
    ImVec2 DisplaySize {};
};

namespace ImGui {

inline ImGuiIO& GetIO() {
    static ImGuiIO io;
    return io;
}

inline void SetNextWindowPos(const ImVec2&) {}
inline void SetNextWindowSize(const ImVec2&) {}
inline void Begin(const char*, void* = nullptr, int = 0) {}
inline void End() {}
inline void SetWindowFontScale(float) {}
inline void Text(const char*, ...) {}
inline void Separator() {}
inline bool Button(const char*) {
    return false;
}
inline void SameLine() {}

constexpr int ImGuiWindowFlags_NoResize = 1 << 0;
constexpr int ImGuiWindowFlags_NoMove = 1 << 1;
constexpr int ImGuiWindowFlags_NoTitleBar = 1 << 2;

}  // namespace ImGui
