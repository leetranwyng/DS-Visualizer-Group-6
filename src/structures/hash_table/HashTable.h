#pragma once
#include "raylib.h"
#include <vector>
#include <bitset>
#include <string>

#define MAX_SIZE 100001ll

using namespace std;

class doubleHash {
public:
    int TABLE_SIZE;
    int keysPresent;
    int PRIME;

    vector<int> hashTable;
    bitset<MAX_SIZE> isPrime;

    bool isAnimating;
    int animType; // 1: Insert, 2: Search, 3: Erase
    vector<int> probePath;
    int currentProbeIndex;
    int animatingValue;
    float animationTimer;
    float animationDelay;
    int highlightedBucket;

    doubleHash(int n);

    void __setSieve();
    int inline hash1(int value);
    int inline hash2(int value);
    bool inline isFull();

    void insert(int value, bool animate = true);
    void erase(int value, bool animate = true);
    int search(int value, bool animate = true);

    void updateAnimation(float dt);
    void draw(int screenWidth, int screenHeight);
};