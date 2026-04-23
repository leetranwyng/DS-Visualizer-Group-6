#include "HashTable.h"
#include "../../UI/FontManager.h"

void hashTableVis::__setSieve() {
    isPrime[0] = isPrime[1] = 1;
    for (long long i = 2; i * i < MAX_SIZE; i++)
        if (isPrime[i] == 0)
            for (long long j = i * i; j < MAX_SIZE; j += i)
                isPrime[j] = 1;
}

int hashTableVis::hash1(int value) { return value % TABLE_SIZE; }
int hashTableVis::hash2(int value) { return PRIME - (value % PRIME); }
bool hashTableVis::isFull() { return (TABLE_SIZE == keysPresent); }

int hashTableVis::getNextProbe(int value, int iteration, int& probe, int& offset) {
    if (probingMode == 0) probe = (probe + 1) % TABLE_SIZE;
    else if (probingMode == 1) probe = (hash1(value) + iteration * iteration) % TABLE_SIZE;
    else if (probingMode == 2) probe = (hash1(value) + iteration * offset) % TABLE_SIZE;
    return probe;
}

hashTableVis::hashTableVis(int n, int probingMode) {
    __setSieve();
    TABLE_SIZE = n;
    this->probingMode = probingMode;
    if (probingMode == 2) {
        PRIME = TABLE_SIZE - 1;
        while (isPrime[PRIME] == 1) PRIME--;
    }
    else PRIME = 0;

    keysPresent = 0;
    hasActiveAnimation = false;
    animType = 0;
    currentProbeIndex = 0;
    animatingValue = -1;
    animationTimer = 0.0f;
    animationDelay = 0.5f;
    prevBucketState = -1;
    highlightedBucket = -1;

    scHeadProbe = -1;
    highlightedSCNode = nullptr;
    newlyInsertedNode = nullptr;
    scErasedNode = nullptr;

    isRevealing = false;
    currentRevealDepth = 9999;
    revealTimer = 0.0f;

    isPaused = false;
    showCodeBox = true;
    codeTitle = "Hash Table Visualizer";
    codeStatus = "Waiting for action...";
    pseudoCode = {
        "// Select Create, Search, Insert, or Remove",
        "// from the left menu to view",
        "// the corresponding pseudo-code",
        "// and step-by-step execution."
    };
    activeCodeLine = -1;

    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable.push_back(-1);
        chainingTable.push_back(nullptr);
    }
}

hashTableVis::~hashTableVis() {
    if (probingMode == 3) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            TreeNode* curr = chainingTable[i];
            while (curr) { TreeNode* temp = curr; curr = curr->right; delete temp; }
        }
    }
}

void hashTableVis::cleanupPreviousAction() {
    if (scErasedNode) {
        int idx = hash1(scErasedNode->value);
        TreeNode* c = chainingTable[idx]; TreeNode* p = nullptr;
        while (c && c != scErasedNode) { p = c; c = c->right; }
        if (c == scErasedNode) {
            if (p) p->right = c->right; else chainingTable[idx] = c->right;
            for (auto it = scNodes.begin(); it != scNodes.end();) {
                if (it->node == scErasedNode) it = scNodes.erase(it); else ++it;
            }
            delete scErasedNode; keysPresent--;
        }
        scErasedNode = nullptr;
    }
    hasActiveAnimation = false;
    isRevealing = false;
}

void hashTableVis::startNewAction(int type, int value) {
    cleanupPreviousAction();
    hasActiveAnimation = true;
    isPaused = false;
    animType = type;
    animatingValue = value;
    currentProbeIndex = 0;
    animationTimer = 0.0f;
    probePath.clear();
    scProbePath.clear();
    highlightedBucket = -1;
    highlightedSCNode = nullptr;
    newlyInsertedNode = nullptr;
}

void hashTableVis::startReveal() {
    if (probingMode == 3) {
        cleanupPreviousAction();
        hasActiveAnimation = true;
        isRevealing = true;
        isPaused = false;
        currentRevealDepth = 0;
        revealTimer = 0.0f;
    }
}

SCNodeShape* hashTableVis::findSCNode(TreeNode* node) {
    for (auto& n : scNodes) {
        if (n.node == node) return &n;
    }
    return nullptr;
}

void hashTableVis::recomputeSCLayout(int screenWidth) {
    float startY = 150, paddingX = 90;
    float totalWidth = (TABLE_SIZE - 1) * paddingX;
    float startX = (screenWidth - totalWidth) / 2.0f;
    if (startX < 50) startX = 50;

    for (auto& n : scNodes) {
        int headIndex = hash1(n.node->value);
        int depth = 1; TreeNode* curr = chainingTable[headIndex];
        while (curr && curr != n.node) { curr = curr->right; depth++; }
        n.target = { startX + headIndex * paddingX + 35.0f, startY + depth * 80.0f };
        n.current = n.target;
    }
}

void hashTableVis::buildSCEdges(int screenWidth) {
    scEdges.clear();
    float startY = 150, paddingX = 90;
    float totalWidth = (TABLE_SIZE - 1) * paddingX;
    float startX = (screenWidth - totalWidth) / 2.0f;
    if (startX < 50) startX = 50;
    float radius = 25.0f;

    for (int i = 0; i < TABLE_SIZE; i++) {
        TreeNode* curr = chainingTable[i];
        if (!curr) continue;

        SCNodeShape* childShape = findSCNode(curr);
        if (childShape) {
            Vector2 parentPos = { startX + i * paddingX, startY };
            Vector2 childPos = childShape->current;
            Vector2 dir = { childPos.x - parentPos.x, childPos.y - parentPos.y };
            float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (dist > 0.0f) {
                dir.x /= dist;
                dir.y /= dist;
            }

            SCEdgeShape e;
            e.parent = nullptr;
            e.child = curr;
            e.start = { parentPos.x + dir.x * radius, parentPos.y + dir.y * radius };
            e.end = { childPos.x - dir.x * radius, childPos.y - dir.y * radius };
            e.color = DARKGRAY;
            scEdges.push_back(e);
        }

        while (curr && curr->right) {
            SCNodeShape* pShape = findSCNode(curr);
            SCNodeShape* cShape = findSCNode(curr->right);
            if (pShape && cShape) {
                Vector2 pPos = pShape->current;
                Vector2 cPos = cShape->current;
                Vector2 dir = { cPos.x - pPos.x, cPos.y - pPos.y };
                float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
                if (dist > 0.0f) {
                    dir.x /= dist;
                    dir.y /= dist;
                }

                SCEdgeShape e;
                e.parent = curr;
                e.child = curr->right;
                e.start = { pPos.x + dir.x * pShape->radius, pPos.y + dir.y * pShape->radius };
                e.end = { cPos.x - dir.x * cShape->radius, cPos.y - dir.y * cShape->radius };
                e.color = DARKGRAY;
                scEdges.push_back(e);
            }
            curr = curr->right;
        }
    }
}

// Visual state machine
void hashTableVis::syncCodeBoxState() {
    if (probingMode == 3) {
        if (isRevealing) { showCodeBox = false; return; }
        showCodeBox = true; highlightedSCNode = nullptr;

        if (currentProbeIndex == 0) {
            activeCodeLine = 0; codeStatus = "Calculating hash...";
        }
        else if (currentProbeIndex < scProbePath.size()) {
            activeCodeLine = 3; codeStatus = "Traversing linked list...";
            highlightedSCNode = scProbePath[currentProbeIndex - 1];
        }
        else {
            if (animType == 1) {
                if (newlyInsertedNode) { activeCodeLine = 5; codeStatus = "Appended " + to_string(animatingValue) + "."; }
                else { activeCodeLine = 2; codeStatus = to_string(animatingValue) + " is a duplicate!"; }
                if (!scProbePath.empty()) highlightedSCNode = scProbePath.back();
            }
            else if (animType == 3) {
                if (scErasedNode) { activeCodeLine = 2; codeStatus = "Removed " + to_string(animatingValue) + "."; }
                else { activeCodeLine = 5; codeStatus = to_string(animatingValue) + " not found."; }
            }
            else if (animType == 2) {
                bool found = (!scProbePath.empty() && scProbePath.back()->value == animatingValue);
                if (found) { activeCodeLine = 2; codeStatus = "Found " + to_string(animatingValue) + "."; highlightedSCNode = scProbePath.back(); }
                else { activeCodeLine = 5; codeStatus = to_string(animatingValue) + " not found."; }
            }
        }
    }
    else {
        if (isRevealing) return;
        showCodeBox = true; highlightedBucket = -1;

        if (currentProbeIndex == 0) {
            activeCodeLine = 0; codeStatus = "Calculating hash...";
        }
        else if (currentProbeIndex < probePath.size()) {
            activeCodeLine = 3; codeStatus = "Probing next bucket...";
        }
        else {
            int finalProbe = probePath.back();
            if (animType == 1) {
                if (prevBucketState == animatingValue) { activeCodeLine = 2; codeStatus = to_string(animatingValue) + " is a duplicate!"; }
                else { activeCodeLine = 5; codeStatus = "Inserted " + to_string(animatingValue) + " at index " + to_string(finalProbe) + "."; }
            }
            else if (animType == 3) {
                if (prevBucketState == animatingValue) { activeCodeLine = 2; codeStatus = "Removed " + to_string(animatingValue) + "."; }
                else { activeCodeLine = 5; codeStatus = to_string(animatingValue) + " not found."; }
            }
            else if (animType == 2) {
                if (hashTable[finalProbe] == animatingValue) { activeCodeLine = 2; codeStatus = "Found " + to_string(animatingValue) + "."; highlightedBucket = finalProbe; }
                else { activeCodeLine = 5; codeStatus = to_string(animatingValue) + " not found."; }
            }
        }
    }
}

// MEDIA CONTROLS
void hashTableVis::togglePause() { if (hasActiveAnimation) isPaused = !isPaused; }

void hashTableVis::stepForward() {
    if (!hasActiveAnimation) return;
    if (isRevealing) {
        currentRevealDepth++;
        int maxD = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            int d = 0; TreeNode* c = chainingTable[i];
            while (c) { d++; c = c->right; }
            maxD = max(maxD, d);
        }
        if (currentRevealDepth > maxD) { isRevealing = false; hasActiveAnimation = false; }
    }
    else {
        int maxIdx = (probingMode == 3) ? scProbePath.size() : probePath.size();
        if (currentProbeIndex < maxIdx) { currentProbeIndex++; syncCodeBoxState(); }
    }
    isPaused = true;
}

void hashTableVis::stepBackward() {
    if (!hasActiveAnimation) return;
    if (isRevealing) {
        if (currentRevealDepth > 0) currentRevealDepth--;
    }
    else {
        if (currentProbeIndex > 0) { currentProbeIndex--; syncCodeBoxState(); }
    }
    isPaused = true;
}

void hashTableVis::skipToEnd() {
    if (!hasActiveAnimation) return;
    if (isRevealing) {
        currentRevealDepth = 999; isRevealing = false; hasActiveAnimation = false;
    }
    else {
        currentProbeIndex = (probingMode == 3) ? scProbePath.size() : probePath.size();
        syncCodeBoxState();
    }
    isPaused = true;
}

void hashTableVis::skipToStart() {
    if (!hasActiveAnimation) return;
    if (isRevealing) currentRevealDepth = 0;
    else { currentProbeIndex = 0; syncCodeBoxState(); }
    isPaused = true;
}

// Core logic
void hashTableVis::insert(int value, bool animate) {
    if (value == -1 || value == -2) return;
    startNewAction(1, value);

    if (probingMode == 3) {
        int idx = hash1(value); scHeadProbe = idx;
        TreeNode* curr = chainingTable[idx]; TreeNode* prev = nullptr;
        bool duplicate = false;

        while (curr) {
            scProbePath.push_back(curr);
            if (curr->value == value) { duplicate = true; break; }
            prev = curr; curr = curr->right;
        }

        if (!duplicate) {
            TreeNode* newNode = new TreeNode(value);
            if (!prev) chainingTable[idx] = newNode; else prev->right = newNode;
            keysPresent++; newlyInsertedNode = newNode;

            SCNodeShape nShape;
            nShape.node = newNode;
            nShape.radius = 25.0f;
            nShape.color = RAYWHITE;
            nShape.current = { 600.0f, 50.0f };
            nShape.target = { 600.0f, 50.0f };
            scNodes.push_back(nShape);
            scProbePath.push_back(newNode);

        }
        if (!animate) { hasActiveAnimation = false; return; }

        codeTitle = "Insert " + to_string(value);
        pseudoCode = { "idx = hash(value); curr = HT[idx];", "while (curr != NULL) {", "    if (curr->value == value) return;", "    curr = curr->next;", "}", "append new Node(value) to HT[idx];" };
        syncCodeBoxState(); return;
    }

    if (isFull()) { hasActiveAnimation = false; return; }
    int initialProbe = hash1(value); int offset = (probingMode == 2) ? hash2(value) : 1;
    int probe = initialProbe; int iteration = 0; bool looped = false;

    while (hashTable[probe] != -1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -2) break;
        if (hashTable[probe] == value) break; // duplicate
        if (looped) break;
        iteration++; probe = getNextProbe(value, iteration, probe, offset);
        if (probe == initialProbe) looped = true;
    }
    if (probePath.empty() || probePath.back() != probe) probePath.push_back(probe);

    prevBucketState = hashTable[probe];
    if (hashTable[probe] != value) { hashTable[probe] = value; keysPresent++; }

    if (!animate) { hasActiveAnimation = false; return; }
    codeTitle = "Insert " + to_string(value);
    pseudoCode = { "i = hash(value);", "while (HT[i] != EMPTY && HT[i] != DELETED) {", "    if (HT[i] == value) return;", "    i = getNextProbe();", "}", "HT[i] = value;" };
    syncCodeBoxState();
}

void hashTableVis::erase(int value, bool animate) {
    startNewAction(3, value);

    if (probingMode == 3) {
        int idx = hash1(value); scHeadProbe = idx;
        TreeNode* curr = chainingTable[idx];
        while (curr) {
            scProbePath.push_back(curr);
            if (curr->value == value) { scErasedNode = curr; break; }
            curr = curr->right;
        }

        if (!animate) { cleanupPreviousAction(); return; }
        codeTitle = "Remove " + to_string(value);
        pseudoCode = { "idx = hash(value); curr = HT[idx]; prev = NULL;", "while (curr != NULL) {", "    if (curr->value == value) { remove curr; return; }", "    prev = curr; curr = curr->next;", "}", "return; // not found" };
        syncCodeBoxState(); return;
    }

    int initialProbe = hash1(value); int offset = (probingMode == 2) ? hash2(value) : 1;
    int probe = initialProbe; int iteration = 0;
    while (1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -1) break;
        if (hashTable[probe] == value) break;
        iteration++; probe = getNextProbe(value, iteration, probe, offset);
        if (probe == initialProbe) break;
    }

    int finalProbe = probePath.back();
    prevBucketState = hashTable[finalProbe];
    if (hashTable[finalProbe] == value) { hashTable[finalProbe] = -2; keysPresent--; }

    if (!animate) { hasActiveAnimation = false; return; }
    codeTitle = "Remove " + to_string(value);
    pseudoCode = { "i = hash(value);", "while (HT[i] != EMPTY) {", "    if (HT[i] == value) { HT[i] = DELETED; return; }", "    i = getNextProbe();", "}", "return; // not found" };
    syncCodeBoxState();
}

int hashTableVis::search(int value, bool animate) {
    startNewAction(2, value);

    if (probingMode == 3) {
        int idx = hash1(value); scHeadProbe = idx;
        TreeNode* curr = chainingTable[idx]; bool found = false;
        while (curr) {
            scProbePath.push_back(curr);
            if (curr->value == value) { found = true; break; }
            curr = curr->right;
        }

        if (!animate) { hasActiveAnimation = false; return (found ? idx : -1); }
        codeTitle = "Search " + to_string(value);
        pseudoCode = { "idx = hash(value); curr = HT[idx];", "while (curr != NULL) {", "    if (curr->value == value) return idx;", "    curr = curr->next;", "}", "return NOT_FOUND;" };
        syncCodeBoxState(); return -1;
    }

    int initialProbe = hash1(value); int offset = (probingMode == 2) ? hash2(value) : 1;
    int probe = initialProbe; int iteration = 0;
    while (1) {
        probePath.push_back(probe);
        if (hashTable[probe] == -1) break;
        if (hashTable[probe] == value) break;
        iteration++; probe = getNextProbe(value, iteration, probe, offset);
        if (probe == initialProbe) break;
    }

    if (!animate) { hasActiveAnimation = false; return -1; }
    codeTitle = "Search " + to_string(value);
    pseudoCode = { "i = hash(value);", "while (HT[i] != EMPTY) {", "    if (HT[i] == value) return i;", "    i = getNextProbe();", "}", "return NOT_FOUND;" };
    syncCodeBoxState(); return -1;
}

void hashTableVis::updateAnimation(float dt) {
    if (!hasActiveAnimation || isPaused) return;

    if (animationDelay <= 0.06f) {
        if (probingMode == 3 && isRevealing) {
            currentRevealDepth = 9999;
            isRevealing = false;
            hasActiveAnimation = false;
        }
        else {
            int maxIdx = (probingMode == 3) ? scProbePath.size() : probePath.size();
            currentProbeIndex = maxIdx;
            syncCodeBoxState();
        }
        return;
    }

    if (probingMode == 3 && isRevealing) {
        revealTimer += dt;
        if (revealTimer >= animationDelay * 2.0f) {
            revealTimer = 0.0f; currentRevealDepth++;
            int maxD = 0;
            for (int i = 0; i < TABLE_SIZE; i++) {
                int d = 0; TreeNode* c = chainingTable[i];
                while (c) { d++; c = c->right; }
                maxD = max(maxD, d);
            }
            if (currentRevealDepth > maxD) { isRevealing = false; hasActiveAnimation = false; }
        }
        return;
    }

    animationTimer += dt;
    float delay = (probingMode == 3) ? (animationDelay * 3.0f) : animationDelay;
    if (animationTimer >= delay) {
        animationTimer = 0.0f;
        int maxIdx = (probingMode == 3) ? scProbePath.size() : probePath.size();
        if (currentProbeIndex < maxIdx) { currentProbeIndex++; syncCodeBoxState(); }
    }
}

void hashTableVis::draw(int screenWidth, int screenHeight, Vector2 mousePos, bool mousePressed) {
    if (probingMode == 3) {
        recomputeSCLayout(screenWidth);
        buildSCEdges(screenWidth);

        TreeNode* hidingNode = nullptr;
        if (hasActiveAnimation && !isRevealing) {
            if (animType == 1 && currentProbeIndex < scProbePath.size()) hidingNode = newlyInsertedNode;
            if (animType == 3 && currentProbeIndex == scProbePath.size()) hidingNode = scErasedNode;
        }

        auto getDepth = [&](TreeNode* node) {
            int headIndex = hash1(node->value); int depth = 1;
            TreeNode* curr = chainingTable[headIndex];
            while (curr && curr != node) { depth++; curr = curr->right; }
            return depth;
            };

        for (auto& e : scEdges) {
            if (e.child == hidingNode) continue;
            if (isRevealing && getDepth(e.child) > currentRevealDepth) continue;

            DrawLineEx(e.start, e.end, 3.0f, e.color);
            Vector2 dir = { e.end.x - e.start.x, e.end.y - e.start.y };
            float length = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (length > 0) { dir.x /= length; dir.y /= length; }
            Vector2 right = { -dir.y, dir.x };
            Vector2 arrow1 = { e.end.x - dir.x * 10 + right.x * 6, e.end.y - dir.y * 10 + right.y * 6 };
            Vector2 arrow2 = { e.end.x - dir.x * 10 - right.x * 6, e.end.y - dir.y * 10 - right.y * 6 };
            DrawTriangle(e.end, arrow1, arrow2, e.color);
        }

        float startY = 150, paddingX = 90;
        float totalWidth = (TABLE_SIZE - 1) * paddingX;
        float startX = (screenWidth - totalWidth) / 2.0f;
        if (startX < 50) startX = 50;
        float radius = 25.0f;

        for (int i = 0; i < TABLE_SIZE; i++) {
            Vector2 headPos = { startX + i * paddingX, startY };
            Color headColor = RAYWHITE;

            if (hasActiveAnimation && !isRevealing && currentProbeIndex == 0 && scHeadProbe == i) headColor = YELLOW;

            DrawCircleV(headPos, radius, DARKGRAY);
            DrawCircleV(headPos, radius - 3.0f, headColor);
            DrawText("H", headPos.x - MeasureText("H", 24) / 2.0f, headPos.y - 12, 24, DARKGRAY);
            DrawText(to_string(i).c_str(), headPos.x - MeasureText(to_string(i).c_str(), 20) / 2.0f, headPos.y + radius + 5, 20, RED);
        }

        for (auto& n : scNodes) {
            if (n.node == hidingNode) continue;
            if (isRevealing && getDepth(n.node) > currentRevealDepth) continue;

            Color nodeColor = n.color;
            if (hasActiveAnimation && !isRevealing) {
                if (currentProbeIndex > 0 && currentProbeIndex <= scProbePath.size()) {
                    if (scProbePath[currentProbeIndex - 1] == n.node) nodeColor = YELLOW;
                }
                else if (highlightedSCNode == n.node) nodeColor = YELLOW;
            }

            DrawCircleV(n.current, n.radius, DARKGRAY);
            DrawCircleV(n.current, n.radius - 3.0f, nodeColor);
            string text = to_string(n.node->value);
            DrawText(text.c_str(), n.current.x - MeasureText(text.c_str(), 22) / 2.0f, n.current.y - 11, 22, BLACK);
        }
    }
    else {
        int boxWidth = 60, boxHeight = 60, padding = 10;
        int cols = (screenWidth - 200) / (boxWidth + padding);
        if (cols == 0) cols = 1;
        int startX = (screenWidth - (cols * (boxWidth + padding) - padding)) / 2;
        int startY = 150;

        for (int i = 0; i < TABLE_SIZE; i++) {
            int row = i / cols; int col = i % cols;
            int x = startX + col * (boxWidth + padding);
            int y = startY + row * (boxHeight + padding);

            int displayValue = hashTable[i];

            if (hasActiveAnimation && !isRevealing && !probePath.empty()) {
                if (animType == 1 && i == probePath.back() && currentProbeIndex < probePath.size()) displayValue = prevBucketState;
                if (animType == 3 && i == probePath.back() && currentProbeIndex < probePath.size()) displayValue = prevBucketState;
            }

            Color bgColor = LIGHTGRAY; string displayText = "";
            if (displayValue == -1) { bgColor = RAYWHITE; displayText = "E"; }
            else if (displayValue == -2) { bgColor = RED; displayText = "D"; }
            else { bgColor = GREEN; displayText = to_string(displayValue); }

            if (hasActiveAnimation && !isRevealing) {
                if (currentProbeIndex < probePath.size() && i == probePath[currentProbeIndex]) bgColor = YELLOW;
                else if (currentProbeIndex == probePath.size() && i == highlightedBucket) bgColor = YELLOW;
            }

            DrawRectangle(x, y, boxWidth, boxHeight, bgColor);
            DrawRectangleLines(x, y, boxWidth, boxHeight, BLACK);
            DrawText(to_string(i).c_str(), x + 5, y + 5, 12, DARKGRAY);
            int textWidth = MeasureText(displayText.c_str(), 20);
            DrawText(displayText.c_str(), x + (boxWidth - textWidth) / 2, y + (boxHeight - 20) / 2, 20, BLACK);
        }
    }

    if (!isRevealing) {
        int boxW = 580;
        int lineHeight = 36;
        int paddingBox = 22;
        int topH = 50;
        int codeH = pseudoCode.empty() ? 100 : (int)pseudoCode.size() * lineHeight + paddingBox * 2;
        int totalH = topH + codeH + 40;

        int cbX = screenWidth - boxW;
        int cbY = screenHeight - totalH - 85;

        int toggleW = 35;
        int toggleH = codeH;

        if (showCodeBox) {
            Button toggleBtn(cbX - toggleW, cbY + 40 + topH, toggleW, toggleH, ">", { 52, 152, 219, 255 });
            if (toggleBtn.isPressed(mousePos, mousePressed)) showCodeBox = false;
            toggleBtn.draw(false, true, false);

            DrawUIFont(codeTitle.c_str(), cbX, cbY, 26, BLACK);
            DrawRectangle(cbX, cbY + 40, boxW, topH, { 52, 152, 219, 255 });
            DrawUIFont(codeStatus.c_str(), cbX + paddingBox, cbY + 40 + 12, 22, WHITE);
            DrawRectangle(cbX, cbY + 40 + topH, boxW, codeH, { 41, 128, 185, 255 });

            for (int i = 0; i < (int)pseudoCode.size(); i++) {
                int textY = cbY + 40 + topH + paddingBox + i * lineHeight;
                if (i == activeCodeLine) {
                    DrawRectangle(cbX, textY - 4, boxW, lineHeight, BLACK);
                }
                DrawUIFont(pseudoCode[i].c_str(), cbX + paddingBox, textY, 22, WHITE);
            }
        }
        else {
            Button toggleBtn(screenWidth - toggleW, cbY + 40 + topH, toggleW, toggleH, "<", { 52, 152, 219, 255 });
            if (toggleBtn.isPressed(mousePos, mousePressed)) showCodeBox = true;
            toggleBtn.draw(false, true, false);
        }
    }
}