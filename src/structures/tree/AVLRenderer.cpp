#include "AVLRenderer.h"
void AVLRender::recomputeLayout() {
    if (!tree || !tree->root) return;
    float startX = 600;   // center of screen
    float startY = 100;
    float offset = 250;   // horizontal spread
    assignPosition(tree->root, startX, startY, offset);
}

NodeShape* AVLRender::findNode(TreeNode* node) {
    for (auto &n : nodes) {
        if (n.node == node) return &n;
    }
    return nullptr;
}

void AVLRender::updatePositions(float dt) {
    float speed = 5.0f;
    for (auto &n : nodes) {
        n.current.x += (n.target.x - n.current.x) * speed * dt;
        n.current.y += (n.target.y - n.current.y) * speed * dt;
    }
}

void AVLRender::syncWithTree() {
    if (!tree || !tree->root) return;
    function<void(TreeNode*)> dfs = [&](TreeNode* node) {
        if (!node) return;
        if (!findNode(node)) {
            NodeShape n;
            n.node = node;
            n.current = {600, 50}; // spawn somewhere top
            n.target = {600, 50};
            n.radius = 40;
            n.color = GREEN;
            nodes.push_back(n);
        }
        dfs(node->left);
        dfs(node->right);
    };
    dfs(tree->root);
}

void AVLRender::assignPosition(TreeNode* node, float x, float y, float offset) {
    if (!node) return;
    NodeShape* n = findNode(node);
    if (n) n->target = {x, y};
    assignPosition(node->left, x - offset, y + 100, offset/2);
    assignPosition(node->right, x + offset, y + 100, offset/2);
}

void AVLRender::handleEvent(EventType type, TreeNode* node) {
    events.push_back({type, node});
}

void AVLRender::processEvent(const AVLevent& e) {
    switch (e.type) {
        case VISIT: {
            NodeShape* n = findNode(e.nodeVal);
            if (n) {
                n->color = YELLOW;
            }
            break;
        }
        case INSERT: {
            syncWithTree();
            NodeShape* n = findNode(e.nodeVal);
            if (n) n->color = GRAY;
            recomputeLayout();
            break;
        }
        case ROTATE_LEFT:
        case ROTATE_RIGHT: {
            recomputeLayout();
            break;
        }
    }
}

void AVLRender::animation(float dt) {
    if (!isVisualizing) return;
    stepTimer += dt;
    if (step < events.size() && stepTimer >= stepDelay) {
        stepTimer = 0.0f;
        processEvent(events[step]);
        step++;
    }
    if (step >= events.size()) isVisualizing = false;
    updatePositions(dt);
}

void AVLRender::buildEdges(TreeNode* node) {
    if (!node) return;
    NodeShape* parent = findNode(node);
    if (node->left) {
        NodeShape* left = findNode(node->left);
        if (parent && left) {
            EdgeShape e;
            e.parent = node;
            e.child = node->left;
            e.start = parent->current;
            e.end = left->current;
            e.color = BLACK;
            edges.push_back(e);
        }
        buildEdges(node->left);
    }

    if (node->right) {
        NodeShape* right = findNode(node->right);
        if (parent && right) {
            EdgeShape e;
            e.parent = node;
            e.child = node->right;
            e.start = parent->current;
            e.end = right->current;
            e.color = BLACK;
            edges.push_back(e);
        }
        buildEdges(node->right);
    }
}

void AVLRender::draw() {
    edges.clear();
    if (tree && tree->root) {
        buildEdges(tree->root);
    }
    for (auto &e : edges) { //draw lines
        DrawLineV(e.start, e.end, e.color);
    }
    for (auto &n : nodes) { //draw nodes
        DrawCircleV(n.current, n.radius, n.color);
        string text = to_string(n.node->value);
        int fontSize = 20;
        int textWidth = MeasureText(text.c_str(), fontSize);

        DrawText(text.c_str(), n.current.x - textWidth / 2, n.current.y - fontSize / 2, fontSize, WHITE);
    }
}

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
    AVLRender render;
    render.tree = &tree;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_ONE)) {
            render.events.clear();
            render.step = 0;
            render.stepTimer = 0;

            TreeNode* inserted = nullptr;
            tree.root = tree.insert(tree.root, 10, inserted);
            render.handleEvent(INSERT, inserted);

            render.isVisualizing = true;
        }

        if (IsKeyPressed(KEY_TWO)) {
            render.events.clear();
            render.step = 0;
            render.stepTimer = 0;

            TreeNode* inserted = nullptr;
            tree.root = tree.insert(tree.root, 20, inserted);
            render.handleEvent(INSERT, inserted);

            render.isVisualizing = true;
        }

        if (IsKeyPressed(KEY_THREE)) {
            render.events.clear();
            render.step = 0;
            render.stepTimer = 0;

            TreeNode* inserted = nullptr;
            tree.root = tree.insert(tree.root, 30, inserted);
            render.handleEvent(INSERT, inserted);

            render.isVisualizing = true;
        }

        render.animation(dt);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Press 1 / 2 / 3 to insert", 20, 20, 20, BLACK);
        render.draw();
        EndDrawing();
    }
    CloseWindow();
}