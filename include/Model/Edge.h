#pragma once

struct Edge {
    int from;
    int to;
    float weight;

    Edge(int u, int v, float w = 1.0f) : from(u), to(v), weight(w) {}
};