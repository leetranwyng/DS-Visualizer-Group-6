#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <functional>
#include <fstream>

using namespace std;

struct TreeNode;
struct AVL;
struct NodeShape;
struct EdgeShape;

const int AVL_ACTION_NONE = 0;
const int AVL_ACTION_INSERT = 1;
const int AVL_ACTION_INIT = 2;
const int AVL_ACTION_VISIT = 3;
const int AVL_ACTION_UPDATE = 4;
const int AVL_ACTION_DELETE = 5;

enum EventType {
    VISIT,
    INSERT,
    ROTATE_LEFT,
    ROTATE_RIGHT,
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