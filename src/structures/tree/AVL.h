#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "raylib.h"
using namespace std;

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    int height;
    TreeNode(int key) {
        value = key;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

struct EnterButton {
    Rectangle rec;
    string text;
    Color color;

    void draw();
    bool isPressed(Vector2 mousePos, bool mousePressed);
};

struct InputBox1 {
    Rectangle rec;
    char text[4];

    int letterCount;
    bool active;
    
    void update();
    void draw();
    void clear();
    int getVal();
    void checkPressed(Vector2 mousePos, bool mousePressed);
};

struct NodeShape {
    int value;
    Vector2 current; //current position
    Vector2 target; //final position
    float radius;
    Color color;
};

struct EdgeShape {
    int parent;
    int child;
    Vector2 start;
    Vector2 end;
    Color color;
};

struct AVLevent {
    string activity; //move node, rotate tree, etc
    int chosenVal; //affected node
};

struct AVL {
    TreeNode* root;
    AVL () {
        root = nullptr;
        isVisualizing = false;
        step = 0;
        stepTimer = 0.0f;
    }
    //Visualization event
    bool isVisualizing;
    int step;
    float stepTimer;
    vector<AVLevent> events;

    //Logic
    int getHeight (TreeNode* node);
    int balanceFactor(TreeNode* node);
    TreeNode* leftRotation (TreeNode* node);
    TreeNode* rightRotation (TreeNode* node);
    TreeNode* insert(TreeNode* node, int key);
    void insertAnimation(int key, bool visualizing = true);

    //Visualization
    void animation (float time);
    void draw();
};