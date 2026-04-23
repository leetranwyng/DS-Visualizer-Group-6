#pragma once
#include "AVL.h"
#include "../../UI/UI.h"
#include <vector>
#include <string>
#include <functional>
#include <fstream>

using namespace std;

struct NodeShape {
    TreeNode* node;
    Vector2 current;
    Vector2 target;
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

const int AVL_ACTION_NONE = 0;
const int AVL_ACTION_INSERT = 1;
const int AVL_ACTION_INIT = 2;
const int AVL_ACTION_VISIT = 3;
const int AVL_ACTION_UPDATE = 4;
const int AVL_ACTION_DELETE = 5;

struct AVLevent {
    EventType type;
    TreeNode* nodeVal;
};

struct AVLRender {
    AVL* tree;
    vector<AVLevent> events;
    int step;
    float stepTimer;
    float stepDelay;
    bool isVisualizing;
    bool isPlaying;
    bool stepMode;
    vector<NodeShape> nodes;
    vector<EdgeShape> edges;

    AVLRender() {
        tree = nullptr;
        step = 0;
        stepDelay = 0.5f;
        stepTimer = 0.0f;
        isVisualizing = false;
        isPlaying = false;
        stepMode = false;
    }

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