#pragma once
#include <iostream>
#include <functional>
using namespace std;

enum EventType {
    VISIT,
    INSERT,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    REMOVE,
};

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

struct AVL {
    TreeNode* root;
    AVL () {
        root = nullptr;
    }
    int getHeight (TreeNode* node);
    int balanceFactor(TreeNode* node);
    TreeNode* leftRotation (TreeNode* node, std::function<void(EventType, TreeNode*)> onEvent = nullptr);
    TreeNode* rightRotation (TreeNode* node, std::function<void(EventType, TreeNode*)> onEvent = nullptr);
    TreeNode* insert(TreeNode* node, int key, TreeNode*& insertedNode, std::function<void(EventType, TreeNode*)> onEvent = nullptr);
    TreeNode* getSuccessor(TreeNode* node);
    TreeNode* remove(TreeNode* node, int key, std::function<void(EventType, TreeNode*)> onEvent = nullptr);
    TreeNode* find(TreeNode* node, int key, std::function<void(EventType, TreeNode*)> onEvent = nullptr);
    TreeNode* deleteTree(TreeNode* node);
};