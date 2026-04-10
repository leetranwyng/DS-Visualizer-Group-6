#include "HashTable.h"

void hashTableVis::__setSieve() {
    isPrime[0] = isPrime[1] = 1;
    for (long long i = 2; i * i < MAX_SIZE; i++)
        if (isPrime[i] == 0)
            for (long long j = i * i; j < MAX_SIZE; j += i)
                isPrime[j] = 1;
}

int hashTableVis::hash1(int value) {
    return value % TABLE_SIZE;
}

int hashTableVis::hash2(int value) {
    return PRIME - (value % PRIME);
}

bool hashTableVis::isFull() {
    return (TABLE_SIZE == keysPresent);
}

int hashTableVis::getNextProbe(int value, int iteration, int& probe, int& offset) {
    if (probingMode == 0) { // Linear probing
        probe = (probe + 1) % TABLE_SIZE;
    }
    else if (probingMode == 1) { // Quadratic probing
        int initialHash = hash1(value);
        probe = (initialHash + iteration * iteration) % TABLE_SIZE;
    }
    else if (probingMode == 2) { // Double hashing
        probe = (hash1(value) + iteration * offset) % TABLE_SIZE;
    }
    return probe;
}

hashTableVis::hashTableVis(int n, int probingMode) {
    __setSieve();
    TABLE_SIZE = n;
    if (probingMode == 2) {
        PRIME = TABLE_SIZE - 1;
        while (isPrime[PRIME] == 1) PRIME--;
    } else {
        PRIME = 0;
    }

    keysPresent = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable.push_back(-1);
    }

    isAnimating = false;
    animType = 0;
    currentProbeIndex = 0;
    animatingValue = -1;
    animationTimer = 0.0f;
    animationDelay = 0.5f;
    highlightedBucket = -1;
}

void hashTableVis::insert(int value, bool animate) {
    if (value == -1 || value == -2) return;
    if (isFull() || isAnimating) return;

    highlightedBucket = -1;

    int initialProbe = hash1(value);
    int offset = (probingMode == 2) ? hash2(value) : 1;

    if (!animate) {
        int probe = initialProbe;
        int iteration = 0;
        while (hashTable[probe] != -1) {
            if (hashTable[probe] == -2) break; 
            iteration++;
            probe = getNextProbe(value, iteration, probe, offset);
            if (probe == initialProbe) break; 
        }
        hashTable[probe] = value;
        keysPresent++;
        return;
    }

    probePath.clear();
    int probe = initialProbe;
    int iteration = 0;
    bool looped = false;

    while (hashTable[probe] != -1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -2) break; 
        if (looped) break;
        iteration++;
        probe = getNextProbe(value, iteration, probe, offset);
        if (probe == initialProbe) looped = true;
    }

    if (probePath.empty() || probePath.back() != probe) {
        probePath.push_back(probe);
    }

    isAnimating = true;
    animType = 1; // 1 = Insert
    currentProbeIndex = 0;
    animatingValue = value;
    animationTimer = 0.0f;
}


void hashTableVis::erase(int value, bool animate) {
    if (isAnimating) return;

    highlightedBucket = -1;

    int initialProbe = hash1(value);
    int offset = (probingMode == 2) ? hash2(value) : 1;

    if (!animate) {
        int probe = initialProbe;
        int iteration = 0;
        while (1) {
            if (hashTable[probe] == -1) return;
            if (hashTable[probe] == value) {
                hashTable[probe] = -2;
                keysPresent--;
                return;
            }
            iteration++;
            probe = getNextProbe(value, iteration, probe, offset);
            if (probe == initialProbe) return;
        }
        return;
    }

    probePath.clear();
    int probe = initialProbe;
    int iteration = 0;
    animType = 3; // 3 = Erase
    animatingValue = value;
    currentProbeIndex = 0;
    isAnimating = true;

    while (1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -1) break;
        if (hashTable[probe] == value) break;
        iteration++;
        probe = getNextProbe(value, iteration, probe, offset);
        if (probe == initialProbe) break;
    }
}

int hashTableVis::search(int value, bool animate) {
    if (isAnimating) return -1;
    highlightedBucket = -1;

    int initialProbe = hash1(value);
    int offset = (probingMode == 2) ? hash2(value) : 1;

    if (!animate) {
        int probe = initialProbe;
        int iteration = 0;
        while (1) {
            if (hashTable[probe] == -1) return -1;
            if (hashTable[probe] == value) return probe;
            iteration++;
            probe = getNextProbe(value, iteration, probe, offset);
            if (probe == initialProbe) return -1;
        }
    }

    probePath.clear();
    int probe = initialProbe;
    int iteration = 0;
    animType = 2; // 2 = Search
    animatingValue = value;
    currentProbeIndex = 0;
    isAnimating = true;

    while (1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -1) return -1;
        if (hashTable[probe] == value) return probe;
        iteration++;
        probe = getNextProbe(value, iteration, probe, offset);
        if (probe == initialProbe) return -1;
    }
}

void hashTableVis::updateAnimation(float dt) {
    if (!isAnimating) return;

    animationTimer += dt;
    if (animationTimer >= animationDelay) {
        animationTimer = 0.0f;
        currentProbeIndex++;

        if (currentProbeIndex >= probePath.size()) {
            isAnimating = false;

            int finalProbe = probePath.back();

            if (animType == 1) { 
                hashTable[finalProbe] = animatingValue;
                keysPresent++;
            }
            else if (animType == 3) { 
                if (hashTable[finalProbe] == animatingValue) {
                    hashTable[finalProbe] = -2;
                    keysPresent--;
                }
            }
            else if (animType == 2) { 
                highlightedBucket = finalProbe;
            }
        }
    }
}

void hashTableVis::draw(int screenWidth, int screenHeight) {
    int boxWidth = 60;
    int boxHeight = 60;
    int padding = 10;

    int cols = (screenWidth - 200) / (boxWidth + padding);
    if (cols == 0) cols = 1;

    int startX = (screenWidth - (cols * (boxWidth + padding) - padding)) / 2;
    int startY = 150;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int row = i / cols;
        int col = i % cols;
        int x = startX + col * (boxWidth + padding);
        int y = startY + row * (boxHeight + padding);

        Color bgColor = LIGHTGRAY;
        string displayText = "";

        if (hashTable[i] == -1) {
            bgColor = RAYWHITE;
            displayText = "E";
        }
        else if (hashTable[i] == -2) {
            bgColor = RED;
            displayText = "D";
        }
        else {
            bgColor = GREEN;
            displayText = to_string(hashTable[i]);
        }

        if (isAnimating && currentProbeIndex < probePath.size() && i == probePath[currentProbeIndex]) {
            bgColor = YELLOW;
        }
        else if (!isAnimating && i == highlightedBucket) {
            bgColor = YELLOW;
        }

        DrawRectangle(x, y, boxWidth, boxHeight, bgColor);
        DrawRectangleLines(x, y, boxWidth, boxHeight, BLACK);
        DrawText(to_string(i).c_str(), x + 5, y + 5, 12, DARKGRAY);
        int textWidth = MeasureText(displayText.c_str(), 20);
        DrawText(displayText.c_str(), x + (boxWidth - textWidth) / 2, y + (boxHeight - 20) / 2, 20, BLACK);
    }
}