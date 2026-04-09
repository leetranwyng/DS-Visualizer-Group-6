#pragma once
#include "raylib.h"
#include "AVL.h"
#include <vector>
#include <string>
using namespace std;

struct NodeShape {
    TreeNode* node;
    Vector2 current; //current position
    Vector2 target; //final position
    float radius;
    Color color;
};

struct EdgeShape {
    TreeNode* parent;
    TreeNode* child;
    Vector2 start;
    Vector2 end;
    Color color;
};

enum EventType {
    VISIT,
    INSERT,
    ROTATE_LEFT,
    ROTATE_RIGHT
};
struct AVLevent {
    EventType type;
    TreeNode* nodeVal; //affected node
};

struct AVLRender {
    AVL* tree;
    vector<AVLevent> events;
    int step;
    float stepTimer;
    bool isVisualizing;

    vector<NodeShape> nodes;
    vector<EdgeShape> edges;

    AVLRender() {
        tree = nullptr;
        isVisualizing = false;
        step = 0;
        stepTimer = 0.0f;
    }

    void handleEvent(EventType type, TreeNode* key);
    void animation(float time);
    void draw();
};

void RenderAVL();