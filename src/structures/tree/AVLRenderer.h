#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <functional>
using namespace std;

struct TreeNode;
struct AVL;

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
    float stepDelay;
    bool isVisualizing;
    vector<NodeShape> nodes;
    vector<EdgeShape> edges;

    AVLRender() {
        tree = nullptr;
        step = 0;
        stepDelay = 0.5f;
        stepTimer = 0.0f;
        isVisualizing = false;
    };

    void handleEvent(EventType type, TreeNode* node);
    void processEvent(const AVLevent& e);
    void animation(float dt);
    void updatePositions(float dt);
    void syncWithTree();
    void assignPosition(TreeNode* node, float x, float y, float offset);
    void recomputeLayout();
    void buildEdges(TreeNode* node);
    NodeShape* findNode(TreeNode* node);
    void draw();
};

void RenderAVL();