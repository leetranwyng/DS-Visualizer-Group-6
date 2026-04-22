#pragma once
#include "raylib.h"
#include <vector>
#include <bitset>
#include <string>
#include <cmath>

#include "../tree/AVL.h" 
#include "../../UI/UI.h" 

#define MAX_SIZE 100001ll

using namespace std;

class hashTableVis {
public:
    int TABLE_SIZE;
    int keysPresent;
    int PRIME;

    vector<int> hashTable;
    bitset<MAX_SIZE> isPrime;

    vector<TreeNode*> chainingTable;
    vector<NodeShape> scNodes;
    vector<EdgeShape> scEdges;

    bool hasActiveAnimation;
    int animType; // 1: Insert, 2: Search, 3: Erase
    int probingMode; // 0: Linear, 1: Quadaratic, 2: Double hashing, 3: Separate Chaining
    vector<int> probePath;
    int currentProbeIndex;
    int animatingValue;
    float animationTimer;
    float animationDelay;
    int prevBucketState;
    int highlightedBucket;

    int scHeadProbe;
    vector<TreeNode*> scProbePath;
    TreeNode* highlightedSCNode;
    TreeNode* newlyInsertedNode;
    TreeNode* scErasedNode;

    bool isRevealing;
    int currentRevealDepth;
    float revealTimer;

    vector<string> pseudoCode;
    string codeTitle;
    string codeStatus;
    int activeCodeLine;
    bool showCodeBox;

    bool isPaused;
    void startNewAction(int type, int value);
    void cleanupPreviousAction();
    void startReveal();
    void togglePause();
    void stepForward();
    void stepBackward();
    void skipToEnd();
    void skipToStart();
    void syncCodeBoxState();

    hashTableVis(int n, int probingMode);
    ~hashTableVis();

    void __setSieve();
    int inline hash1(int value);
    int inline hash2(int value);
    bool inline isFull();

    int inline getNextProbe(int value, int iteration, int& probe, int& offset);

    void insert(int value, bool animate = true);
    void erase(int value, bool animate = true);
    int search(int value, bool animate = true);

    void recomputeSCLayout(int screenWidth);
    NodeShape* findSCNode(TreeNode* node);
    void buildSCEdges(int screenWidth);

    void updateAnimation(float dt);
    void draw(int screenWidth, int screenHeight, Vector2 mousePos, bool mousePressed);
};