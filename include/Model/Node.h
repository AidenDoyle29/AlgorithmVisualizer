#pragma once

struct Node {
    int id;
    float x;
    float y;

    Node(int id, float x = 0, float y = 0) : id(id), x(x), y(y) {}
};