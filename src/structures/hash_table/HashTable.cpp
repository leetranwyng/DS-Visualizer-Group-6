#include "HashTable.h"

void doubleHash::__setSieve() {
    isPrime[0] = isPrime[1] = 1;
    for (long long i = 2; i * i < MAX_SIZE; i++)
        if (isPrime[i] == 0)
            for (long long j = i * i; j < MAX_SIZE; j += i)
                isPrime[j] = 1;
}

int doubleHash::hash1(int value) {
    return value % TABLE_SIZE;
}

int doubleHash::hash2(int value) {
    return PRIME - (value % PRIME);
}

bool doubleHash::isFull() {
    return (TABLE_SIZE == keysPresent);
}

doubleHash::doubleHash(int n) {
    __setSieve();
    TABLE_SIZE = n;
    PRIME = TABLE_SIZE - 1;
    while (isPrime[PRIME] == 1) PRIME--;
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

void doubleHash::insert(int value, bool animate) {
    if (value == -1 || value == -2) return;
    if (isFull() || isAnimating) return;

    highlightedBucket = -1;

    int probe = hash1(value), offset = hash2(value);

    if (!animate) {
        while (hashTable[probe] != -1) {
            if (hashTable[probe] == -2) break;
            probe = (probe + offset) % TABLE_SIZE;
        }
        hashTable[probe] = value;
        keysPresent++;
        return;
    }

    probePath.clear();
    int initialPos = probe;
    bool firstItr = true;

    while (hashTable[probe] != -1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -2) break;

        if (probe == initialPos && !firstItr) break;
        probe = (probe + offset) % TABLE_SIZE;
        firstItr = false;
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

void doubleHash::updateAnimation(float dt) {
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

void doubleHash::erase(int value, bool animate) {
    if (isAnimating) return;

    highlightedBucket = -1;

    int probe = hash1(value), offset = hash2(value), initialPos = probe;
    bool firstItr = true;

    if (!animate) {
        while (hashTable[probe] != -1) {
            if (hashTable[probe] == value) {
                hashTable[probe] = -2;
                keysPresent--;
                return;
            }
            if (probe == initialPos && !firstItr) break;
            probe = (probe + offset) % TABLE_SIZE;
            firstItr = false;
        }
        return;
    }

    probePath.clear();
    animType = 3; // 3 = Erase
    animatingValue = value;
    currentProbeIndex = 0;
    isAnimating = true;

    while (1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -1) break; 
        if (hashTable[probe] == value) break; 
        if (probe == initialPos && !firstItr) break; 
        probe = (probe + offset) % TABLE_SIZE;
        firstItr = false;
    }
}

int doubleHash::search(int value, bool animate) {
    if (isAnimating) return -1;

    highlightedBucket = -1;

    int probe = hash1(value), offset = hash2(value), initialPos = probe;
    bool firstItr = true;

    if (!animate) {
        while (1) {
            if (hashTable[probe] == -1) return -1;
            if (hashTable[probe] == value) return probe;
            if (probe == initialPos && !firstItr) return -1;
            probe = ((probe + offset) % TABLE_SIZE);
            firstItr = false;
        }
    }

    probePath.clear();
    animType = 2; // 2 = Search
    animatingValue = value;
    currentProbeIndex = 0;
    isAnimating = true;

    while (1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -1) return -1;
        if (hashTable[probe] == value) return probe;
        if (probe == initialPos && !firstItr) return -1;
        probe = ((probe + offset) % TABLE_SIZE);
        firstItr = false;
    }
}

void doubleHash::draw(int screenWidth, int screenHeight) {
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