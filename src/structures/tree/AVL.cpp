#include "AVL.h"

int AVL::getHeight (TreeNode* node) {
    if (!node) return 0;
    return node->height;
}

int AVL::balanceFactor(TreeNode* node) {
    if (!node) return 0;
    int heightDifference = getHeight(node->left) - getHeight(node->right);
    return heightDifference;
}

TreeNode* AVL::leftRotation (TreeNode* node) {
    TreeNode* leftr1 = node->right;
    TreeNode* leftr2 = leftr1->left;
    leftr1->left = node;
    node->right = leftr2;
    leftr1->height = 1 + max(getHeight(leftr1->left), getHeight(leftr1->right));
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    return leftr1;
}

TreeNode* AVL::rightRotation (TreeNode* node) {
    TreeNode* rightr1 = node->left;
    TreeNode* rightr2 = rightr1->right;
    rightr1->right = node;
    node->left = rightr2;
    rightr1->height = 1 + max(getHeight(rightr1->left), getHeight(rightr1->right));
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    return rightr1;
}

TreeNode* AVL::insert(TreeNode* node, int key, TreeNode*& insertedNode) {
    if (!node) {
        insertedNode = new TreeNode(key);
        return insertedNode;
    }

    if (key == node->value) {
        insertedNode = node;
        return node;
    } else if (key < node->value) node->left = insert(node->left, key, insertedNode);
    else node->right = insert(node->right, key, insertedNode);

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = balanceFactor(node);
    //LEFT-LEFT
    if (balance > 1 && key < node->left->value) {
        return rightRotation(node);
    }
    //RIGHT-RIGHT
    if (balance < -1 && key > node->right->value) {
        return leftRotation(node);
    }
    //RIGHT-LEFT
    if (balance < -1 && key < node->right->value) {
        node->right = rightRotation(node->right);
        return leftRotation(node);
    }
    //LEFT-RIGHT
    if (balance > 1 && key > node->left->value) {
        node->left = leftRotation(node->left);
        return rightRotation(node);
    }

    return node;
}
