#include "AVLRenderer.h"

void AVLRender::handleEvent(EventType type, TreeNode* key) {
    events.push_back({type, key});
}

void AVLRender::animation(float time) {
    if (!isVisualizing) return;

    if (step >= events.size()){
        isVisualizing = false;
        return;
    }

    stepTimer += time;
    if (stepTimer < 0.5f) return;
    stepTimer = 0;
    AVLevent e = events[step];
    switch (e.type) {
        case VISIT:
            break;
        case INSERT:
            NodeShape n;
            n.node = e.nodeVal;
            n.current = {100, 100};
            n.target = {100, 100};
            n.radius = 20;
            n.color = PINK;
            nodes.push_back(n);
            break;
        case ROTATE_LEFT:
        case ROTATE_RIGHT:
            break;
    }
    step++;
}

void AVLRender::draw() {
    for (auto &n : nodes) {
        DrawCircleV(n.current, n.radius, n.color);
        DrawText(TextFormat("%d", n.node->value), n.current.x - 10, n.current.y - 10, 20, WHITE);
    }
}

void RenderAVL() {
    InitWindow(1200, 800, "AVL Visualizer");
    SetTargetFPS(60);

    AVL tree;
    AVLRender renderer;

    renderer.tree = &tree;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime(); 
        if (IsKeyPressed(KEY_ONE)) tree.root = tree.insert(tree.root, 10);
        if (IsKeyPressed(KEY_TWO)) tree.root = tree.insert(tree.root, 20);
        if (IsKeyPressed(KEY_THREE)) tree.root = tree.insert(tree.root, 30);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("OH ALR WHAT THE HELLY", 400, 400, 40, BLACK);
        renderer.draw();
        EndDrawing();
    }
    CloseWindow();
}