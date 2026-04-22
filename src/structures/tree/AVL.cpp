#include "AVL.h"

int AVL::getHeight(TreeNode* node) {
    if (!node) return 0;
    return node->height;
}

int AVL::balanceFactor(TreeNode* node) {
    if (!node) return 0;
    int heightDifference = getHeight(node->left) - getHeight(node->right);
    return heightDifference;
}

TreeNode* AVL::leftRotation (TreeNode* node, std::function<void(EventType, TreeNode*)> onEvent) {
    if (onEvent) onEvent(ROTATE_LEFT, node);
    TreeNode* leftr1 = node->right;
    TreeNode* leftr2 = leftr1->left;
    leftr1->left = node;
    node->right = leftr2;
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    leftr1->height = 1 + max(getHeight(leftr1->left), getHeight(leftr1->right));
    return leftr1;
}

TreeNode* AVL::rightRotation (TreeNode* node, std::function<void(EventType, TreeNode*)> onEvent) {
    if (onEvent) onEvent(ROTATE_RIGHT, node);
    TreeNode* rightr1 = node->left;
    TreeNode* rightr2 = rightr1->right;
    rightr1->right = node;
    node->left = rightr2;
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    rightr1->height = 1 + max(getHeight(rightr1->left), getHeight(rightr1->right));
    return rightr1;
}

TreeNode* AVL::insert(TreeNode* node, int key, TreeNode*& insertedNode, std::function<void(EventType, TreeNode*)> onEvent) {
    if (node && onEvent) onEvent(VISIT, node);
    
    if (!node) {
        insertedNode = new TreeNode(key);
        if (onEvent) onEvent(INSERT, insertedNode);
        return insertedNode;
    }

    if (key == node->value) {
        insertedNode = node;
        return node;
    } else if (key < node->value) node->left = insert(node->left, key, insertedNode, onEvent);
    else node->right = insert(node->right, key, insertedNode, onEvent);

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = balanceFactor(node);

    //LEFT-LEFT
    if (balance > 1 && key < node->left->value) {
        return rightRotation(node, onEvent);
    }
    //RIGHT-RIGHT
    if (balance < -1 && key > node->right->value) {
        return leftRotation(node, onEvent);
    }
    //RIGHT-LEFT
    if (balance < -1 && key < node->right->value) {
        node->right = rightRotation(node->right, onEvent);
        return leftRotation(node, onEvent);
    }
    //LEFT-RIGHT
    if (balance > 1 && key > node->left->value) {
        node->left = leftRotation(node->left, onEvent);
        return rightRotation(node, onEvent);
    }

    return node;
}

TreeNode* AVL::getSuccessor(TreeNode* node) {
    TreeNode* cur = node->right;
    while (cur->left) cur = cur->left;
    return cur;
}

TreeNode* AVL::remove(TreeNode* node, int key, std::function<void(EventType, TreeNode*)> onEvent) {
    if (node && onEvent) onEvent(VISIT, node);

    if (!node) return nullptr;
    if (key < node->value) node->left = remove(node->left, key, onEvent);
    else if (key > node->value) node->right = remove(node->right, key, onEvent);
    else {
        if (!node->left && !node->right) { //No kids
            if (onEvent) onEvent(REMOVE, node);
            delete node;
            return nullptr;
        } else if (!node->left) {          //One right kid
            TreeNode* temp = node->right;
            if (onEvent) onEvent(REMOVE, node);
            delete node;
            return temp;
        } else if (!node->right) {         //One not-right kid
            TreeNode* temp = node->left;
            if (onEvent) onEvent(REMOVE, node);
            delete node;
            return temp;
        }
        TreeNode* temp = getSuccessor(node);
        node->value = temp->value;
        node->right = remove(node->right, temp->value, onEvent);
    }

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = balanceFactor(node);
    if (balance > 1 && key < node->left->value) {
        return rightRotation(node, onEvent);
    }
    if (balance < -1 && key > node->right->value) {
        return leftRotation(node, onEvent);
    }
    if (balance < -1 && key < node->right->value) {
        node->right = rightRotation(node->right, onEvent);
        return leftRotation(node, onEvent);
    }
    if (balance > 1 && key > node->left->value) {
        node->left = leftRotation(node->left, onEvent);
        return rightRotation(node, onEvent);
    }

    return node;
}

TreeNode* AVL::find(TreeNode* node, int key, std::function<void(EventType, TreeNode*)> onEvent) {
    if (node && onEvent) onEvent(VISIT, node);
    if (!node) return nullptr;
    if (key == node->value) return node;
    else if (key < node->value) return find(node->left, key, onEvent);
    else return find(node->right, key, onEvent);
}

TreeNode* AVL::deleteTree(TreeNode* node) {
    if (!node) return nullptr;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
    return nullptr;
}