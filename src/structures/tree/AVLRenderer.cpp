#include "AVL.h"
#include "AVLRenderer.h"
#include "../../UI/UI.h"

void AVLRender::recomputeLayout() {
    if (!tree || !tree->root) return;
    float startX = GetScreenWidth() / 2.0f;   // center of screen
    float startY = 100;
    float offset = 250;   // horizontal spread
    assignPosition(tree->root, startX, startY, offset);
}

NodeShape* AVLRender::findNode(TreeNode* node) {
    for (auto& n : nodes) {
        if (n.node == node) return &n;
    }
    return nullptr;
}

void AVLRender::updatePositions(float dt) {
    float speed = 2.0f + 8.0f * stepDelay;
    for (auto& n : nodes) {
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
            float centerX = GetScreenWidth() / 2.0f; //center node
            n.current = { centerX, 50 };
            n.target = { centerX, 50 };
            n.radius = 30;
            n.color = GRAY;
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
    if (n) n->target = { x, y };
    assignPosition(node->left, x - offset, y + 100, offset / 2);
    assignPosition(node->right, x + offset, y + 100, offset / 2);
}

void AVLRender::handleEvent(EventType type, TreeNode* node) {
    events.push_back({ type, node });
}

void AVLRender::processEvent(const AVLevent& e) {
    switch (e.type) {
    case VISIT: {
        for (auto& n : nodes) {
            if (n.node == e.nodeVal) n.color = GREEN;
            else n.color = GRAY;
        }
        NodeShape* n = findNode(e.nodeVal);
        if (n) {
            n->color = RED;
        }
        break;
    }
    case INSERT: {
        syncWithTree();
        NodeShape* n = findNode(e.nodeVal);
        if (n) n->color = GREEN;
        recomputeLayout();
        break;
    }
    case ROTATE_LEFT:
    case ROTATE_RIGHT: {
        recomputeLayout();
        break;
    }
    case REMOVE: {
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (it->node == e.nodeVal) {
                nodes.erase(it);
                break;
            }
        }
        recomputeLayout();
        break;
    }
    }
}

void AVLRender::animation(float dt) {
    if (!isPlaying) {
        updatePositions(dt);
        return;
    }

    stepTimer += dt;

    if (stepTimer >= stepDelay) {
        if (step < events.size()) {
            processEvent(events[step]);
            step++;
        }
        else {
            isPlaying = false;
            isVisualizing = false;
        }
        stepTimer = 0;
    }

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
    for (auto& e : edges) { //draw lines
        DrawLineV(e.start, e.end, e.color);
    }
    for (auto& n : nodes) { //draw nodes
        DrawCircleV(n.current, n.radius, n.color);
        string text = to_string(n.node->value);
        int fontSize = 20;
        int textWidth = MeasureText(text.c_str(), fontSize);

        DrawText(text.c_str(), n.current.x - textWidth / 2, n.current.y - fontSize / 2, fontSize, WHITE);
    }
}

void DrawPseudoCode(Rectangle rect, const vector<string>& lines) {
    Color TURQUOISE = { 175, 238, 238, 220 };
    DrawRectangleRec(rect, Fade(TURQUOISE, 0.95f));
    DrawRectangleLinesEx(rect, 2, DARKGRAY);
    DrawText("Pseudo-code", rect.x + 10, rect.y - 25, 20, DARKGRAY);

    int fontSize = 18;
    int padding = 10;
    for (int i = 0; i < lines.size(); i++) {
        DrawText(lines[i].c_str(), rect.x + padding, rect.y + padding + i * (fontSize + 6), fontSize, BLACK);
    }
}

void RenderAVL() {
    InitWindow(1360, 850, "AVL Visualizer");
    float screenW = GetScreenWidth(), screenH = GetScreenHeight();
    float pseudoWidth = 380, pseudoHeight = 200;
    SetTargetFPS(60);
    Rectangle pseudoRect = { GetScreenWidth() - pseudoWidth - 20, GetScreenHeight() - pseudoHeight - 20, pseudoWidth, pseudoHeight };
    vector<string> pseudo;

    AVL tree;
    AVLRender render;
    render.tree = &tree;
    bool isMenuExpanded = true;
    int currentAction = AVL_ACTION_NONE;
    TreeNode* selectedNode = nullptr; //For update and delete function
    bool isSelecting = false;

    // UI
    float menuWidth_Expanded = 180;
    float menuWidth_Collapsed = 35;
    float buttonHeight = 45;
    float bottomBarHeight = 40;
    float startY = screenH - bottomBarHeight - (5 * buttonHeight) - 40;
    float controlY = GetScreenHeight() - 70;
    float controlX = GetScreenWidth() / 2.0f - 70;
    float panelX = menuWidth_Collapsed + menuWidth_Expanded + 15;

    Color menuColor = { 210, 80, 50, 255 };
    Color toggleColor = { 190, 60, 40, 255 };
    Color buttonBlue = { 66, 133, 244, 255 };

    Button collapseButton(0, startY, menuWidth_Collapsed, buttonHeight, "<", toggleColor);
    Button expandButton(0, startY, menuWidth_Collapsed, buttonHeight, ">", toggleColor);

    Button initMenuButton(menuWidth_Collapsed, startY, menuWidth_Expanded, buttonHeight, "Init", menuColor);
    Button insertMenuButton(menuWidth_Collapsed, startY + buttonHeight, menuWidth_Expanded, buttonHeight, "Insert (v)", menuColor);
    Button findMenuButton(menuWidth_Collapsed, startY + 2 * buttonHeight, menuWidth_Expanded, buttonHeight, "Find (v)", menuColor);
    Button updateMenuButton(menuWidth_Collapsed, startY + 3 * buttonHeight, menuWidth_Expanded, buttonHeight, "Update (v)", menuColor);
    Button deleteMenuButton(menuWidth_Collapsed, startY + 4 * buttonHeight, menuWidth_Expanded, buttonHeight, "Delete (v)", menuColor);
    Button randomBtn(panelX, initMenuButton.rect.y, 100, buttonHeight, "Random", toggleColor);
    Button fileBtn(panelX + 110, initMenuButton.rect.y, 100, buttonHeight, "File", toggleColor);

    InputBox insertInput(panelX + MeasureText("v = ", 20), insertMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    Button insertGoButton(insertInput.rect.x + insertInput.rect.width + 5, insertMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    InputBox findInput(panelX + MeasureText("v = ", 20), findMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    Button findGoButton(findInput.rect.x + findInput.rect.width + 5, findMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    InputBox updateInput(panelX + MeasureText("v = ", 20), updateMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    InputBox valueUpdated(panelX + MeasureText("v = ", 20) + updateInput.rect.width + MeasureText("to v = ", 20) + 10, updateMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    Button updateGoButton(valueUpdated.rect.x + valueUpdated.rect.width + 5, updateMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    Button deleteWholeTree(panelX, deleteMenuButton.rect.y, 180, buttonHeight, "Delete Tree", toggleColor);
    Button deleteSelectBtn(panelX + 190, deleteMenuButton.rect.y, 85, buttonHeight, "Select", toggleColor);
    Button deleteGoButton(panelX + 285, deleteMenuButton.rect.y, 85, buttonHeight, "Delete", toggleColor);

    Button prevBtn(controlX, controlY, 50, 40, "<", buttonBlue);
    Button playBtn(controlX + 60, controlY, 50, 40, "||", buttonBlue);
    Button nextBtn(controlX + 120, controlY, 50, 40, ">", buttonBlue);
    Button instant(controlX + 180, controlY, 84, 40, "Instant", buttonBlue);

    Slider speedSlider(50, 815, 200, 20, 0.05f, 1.5f, 0.5f);

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        if (isSelecting && mousePressed) {
            for (auto& n : render.nodes) {
                float dx = mousePos.x - n.current.x;
                float dy = mousePos.y - n.current.y;

                if (dx * dx + dy * dy <= n.radius * n.radius) {
                    selectedNode = n.node;
                    n.color = GREEN;
                    isSelecting = false;
                    break;
                }
            }
        }

        if (isMenuExpanded) {
            if (currentAction == AVL_ACTION_INSERT) {
                insertInput.checkPressed(mousePos, mousePressed);
                insertInput.Update();
                pseudo = {
                          "insert(x):",
                          "  insert like BST",
                          "  update height",
                          "  check balance",
                          "  rotate if needed"
                };
            }

            if (currentAction == AVL_ACTION_VISIT) {
                findInput.checkPressed(mousePos, mousePressed);
                findInput.Update();
                pseudo = {
                    "visit(x):",
                    "  if less than root, move left",
                    "  if more than root, move right",
                    "  if equal to root, visit"
                };
            }

            if (currentAction == AVL_ACTION_UPDATE) {
                updateInput.checkPressed(mousePos, mousePressed);
                updateInput.Update();
                valueUpdated.checkPressed(mousePos, mousePressed);
                valueUpdated.Update();
                pseudo = {
                    "update(x → y):",
                    "  find node x",
                    "  remove(x)",
                    "  insert(y)",
                    "  rebalance"
                };
            }

            if (currentAction == AVL_ACTION_DELETE && deleteSelectBtn.isPressed(mousePos, mousePressed)) {
                isSelecting = true;
                selectedNode = nullptr;
                for (auto& n : render.nodes) n.color = GRAY;
                pseudo = {
                    "delete(x):",
                    "  find node",
                    "  remove node",
                    "  replace (child/successor)",
                    "  rebalance"
                };
            }
            if (currentAction == AVL_ACTION_DELETE && deleteWholeTree.isPressed(mousePos, mousePressed)) {
                tree.root = nullptr;
                render.nodes.clear();
                render.edges.clear();
                selectedNode = nullptr;
                pseudo = {
                    "delete whole tree:",
                    "  delete root"
                };
            }
        }

        if (!isMenuExpanded && expandButton.isPressed(mousePos, mousePressed)) isMenuExpanded = true;
        else if (isMenuExpanded && collapseButton.isPressed(mousePos, mousePressed)) {
            isMenuExpanded = false;
            currentAction = AVL_ACTION_NONE;
        }

        //Step by step animation
        if (instant.isPressed(mousePos, mousePressed)) {
            render.stepMode = false; // Set to instant mode
            render.isPlaying = false;
            render.events.clear();
            render.isVisualizing = false;
        }

        if (playBtn.isPressed(mousePos, mousePressed)) {
            render.isPlaying = !render.isPlaying;
            if (render.isPlaying) render.stepMode = true;
        }
        if (nextBtn.isPressed(mousePos, mousePressed)) {
            render.isPlaying = false;
            if (render.step < render.events.size()) {
                render.processEvent(render.events[render.step]);
                render.step++;
            }
            if (render.step == render.events.size()) render.isVisualizing = false;
        }
        if (prevBtn.isPressed(mousePos, mousePressed)) {
            render.isPlaying = false;
            if (render.step > 0) {
                render.step--;
                render.nodes.clear();
                render.edges.clear();
                render.syncWithTree();
                render.recomputeLayout();
                for (int i = 0; i < render.step; i++) {
                    render.processEvent(render.events[i]);
                }
            }
        }

        if (isMenuExpanded) {
            if (insertMenuButton.isPressed(mousePos, mousePressed)) currentAction = AVL_ACTION_INSERT;
            if (initMenuButton.isPressed(mousePos, mousePressed)) currentAction = AVL_ACTION_INIT;
            if (findMenuButton.isPressed(mousePos, mousePressed)) currentAction = AVL_ACTION_VISIT;
            if (updateMenuButton.isPressed(mousePos, mousePressed)) currentAction = AVL_ACTION_UPDATE;
            if (deleteMenuButton.isPressed(mousePos, mousePressed)) currentAction = AVL_ACTION_DELETE;
        }

        if (isMenuExpanded && !render.isVisualizing) {
            if (currentAction == AVL_ACTION_INIT && randomBtn.isPressed(mousePos, mousePressed)) {
                tree.root = nullptr;
                render.nodes.clear();
                render.edges.clear();
                for (int i = 0; i < 10; i++) {
                    int val = rand() % 100;
                    TreeNode* inserted = nullptr;
                    tree.root = tree.insert(tree.root, val, inserted);
                }
                render.syncWithTree();
                render.recomputeLayout();
            }

            if (currentAction == AVL_ACTION_INIT && fileBtn.isPressed(mousePos, mousePressed)) {
                ifstream fin(getResourcesPath("File_Input.txt").c_str());
                if (fin.is_open()) {
                    tree.root = tree.deleteTree(tree.root);
                    render.nodes.clear();
                    render.edges.clear();
                    render.isVisualizing = false;
                    render.isPlaying = false;
                    render.events.clear();
                    render.step = 0;
                    render.stepTimer = 0;
                    int val;
                    while (fin >> val) {
                        if (tree.find(tree.root, val)) continue;
                        TreeNode* inserted = nullptr;
                        tree.root = tree.insert(tree.root, val, inserted);
                    }
                    render.syncWithTree();
                    render.recomputeLayout();
                    fin.close();
                }
            }

            if (currentAction == AVL_ACTION_INSERT && insertGoButton.isPressed(mousePos, mousePressed)) {
                int v = insertInput.GetValue();
                if (v != -1) {
                    TreeNode* inserted = nullptr;
                    if (render.stepMode) {
                        render.isVisualizing = true;
                        render.events.clear();
                        render.step = 0;
                        render.stepTimer = 0;
                        tree.root = tree.insert(tree.root, v, inserted, [&](EventType t, TreeNode* n) {
                            render.handleEvent(t, n);
                            });
                        render.isPlaying = true;
                    }
                    else {
                        tree.root = tree.insert(tree.root, v, inserted);
                        render.nodes.clear();
                        render.syncWithTree();
                        render.recomputeLayout();
                        render.isVisualizing = false;
                    }
                    insertInput.Clear();
                }
            }

            if (currentAction == AVL_ACTION_VISIT && findGoButton.isPressed(mousePos, mousePressed)) {
                int v = findInput.GetValue();
                if (v != -1) {
                    for (auto& node : render.nodes) node.color = GRAY;
                    if (render.stepMode) {
                        render.isVisualizing = true;
                        render.events.clear();
                        render.step = 0;
                        render.stepTimer = 0;
                        tree.find(tree.root, v, [&](EventType t, TreeNode* n) {
                            render.handleEvent(t, n);
                            });
                        render.isPlaying = true;
                    }
                    else {
                        TreeNode* cur = tree.root;
                        while (cur) {
                            NodeShape* n = render.findNode(cur);
                            if (n) n->color = GREEN;
                            if (v == cur->value) {
                                if (n) n->color = RED;
                                break;
                            }
                            else if (v < cur->value) cur = cur->left;
                            else cur = cur->right;
                        }
                    }
                    findInput.Clear();
                }
            }

            if (currentAction == AVL_ACTION_UPDATE && updateGoButton.isPressed(mousePos, mousePressed)) {
                int v = updateInput.GetValue();
                int to = valueUpdated.GetValue();
                if (v != -1 && to != -1 && v != to && tree.find(tree.root, v) && !tree.find(tree.root, to)) {
                    for (auto& node : render.nodes) node.color = GRAY;
                    if (render.stepMode) {
                        render.isVisualizing = true;
                        render.events.clear();
                        render.step = 0;
                        render.stepTimer = 0;
                        tree.root = tree.remove(tree.root, v, [&](EventType t, TreeNode* n) {
                            render.handleEvent(t, n);
                            });
                        TreeNode* inserted = nullptr;
                        tree.root = tree.insert(tree.root, to, inserted, [&](EventType t, TreeNode* n) {
                            render.handleEvent(t, n);
                            });
                        render.isPlaying = true;
                    }
                    else {
                        tree.root = tree.remove(tree.root, v);
                        TreeNode* inserted = nullptr;
                        tree.root = tree.insert(tree.root, to, inserted);
                        render.nodes.clear();
                        render.edges.clear();
                        render.syncWithTree();
                        render.recomputeLayout();
                        NodeShape* n = render.findNode(inserted);
                        if (n) n->color = GREEN;
                        render.isVisualizing = false;
                    }
                    updateInput.Clear();
                    valueUpdated.Clear();
                }
            }

            if (currentAction == AVL_ACTION_DELETE && selectedNode != nullptr && deleteGoButton.isPressed(mousePos, mousePressed)) {
                int val = selectedNode->value;
                if (render.stepMode) {
                    render.isVisualizing = true;
                    render.events.clear();
                    render.step = 0;
                    render.stepTimer = 0;
                    tree.root = tree.remove(tree.root, val, [&](EventType t, TreeNode* n) {
                        render.handleEvent(t, n);
                        });
                    render.isPlaying = true;
                }
                else {
                    tree.root = tree.remove(tree.root, val);
                    render.nodes.clear();
                    render.edges.clear();
                    render.syncWithTree();
                    render.recomputeLayout();
                    render.isVisualizing = false;
                }
                selectedNode = nullptr;
            }
        }

        speedSlider.Update(mousePos, mouseDown);
        render.stepDelay = speedSlider.GetValue();
        float dt = GetFrameTime();
        render.animation(dt);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        render.draw();
        DrawRectangle(0, 0, 35, screenH, BLACK);

        if (isMenuExpanded) {
            DrawFlatButton(initMenuButton.rect, "Init", menuColor, true);
            DrawFlatButton(insertMenuButton.rect, "Insert (v)", menuColor, true);
            DrawFlatButton(findMenuButton.rect, "Find (v)", menuColor, true);
            DrawFlatButton(updateMenuButton.rect, "Update (v)", menuColor, true);
            DrawFlatButton(deleteMenuButton.rect, "Delete (v)", menuColor, true);
            DrawFlatButton(collapseButton.rect, "<", toggleColor, false);
        }
        else {
            DrawFlatButton(expandButton.rect, ">", toggleColor, false);
        }

        if (isMenuExpanded) {
            int labelOffsetY = 12;
            if (currentAction == AVL_ACTION_INIT) {
                DrawFlatButton(randomBtn.rect, "Random", toggleColor, false);
                DrawFlatButton(fileBtn.rect, "File", toggleColor, false);
            }

            if (currentAction == AVL_ACTION_INSERT) {
                DrawLabel(panelX, insertMenuButton.rect.y + labelOffsetY, "v =");
                insertInput.Draw();
                DrawFlatButton(insertGoButton.rect, "Go", toggleColor, false);
            }

            if (currentAction == AVL_ACTION_VISIT) {
                DrawLabel(panelX, findMenuButton.rect.y + labelOffsetY, "v =");
                findInput.Draw();
                DrawFlatButton(findGoButton.rect, "Go", toggleColor, false);
            }

            if (currentAction == AVL_ACTION_UPDATE) {
                DrawLabel(panelX, updateMenuButton.rect.y + labelOffsetY, "v =");
                updateInput.Draw();
                DrawLabel(panelX + updateInput.rect.width + 45, updateMenuButton.rect.y + labelOffsetY, "to v =");
                valueUpdated.Draw();
                DrawFlatButton(updateGoButton.rect, "Go", toggleColor, false);
            }

            if (currentAction == AVL_ACTION_DELETE) {
                DrawFlatButton(deleteWholeTree.rect, "Delete Tree", toggleColor, false);
                DrawFlatButton(deleteSelectBtn.rect, "Select", toggleColor, false);
                if (selectedNode != nullptr) DrawFlatButton(deleteGoButton.rect, "Delete", toggleColor, false);
            }
        }
        speedSlider.Draw();
        DrawText("Speed:", speedSlider.bounds.x, speedSlider.bounds.y - 30, 20, DARKGRAY);
        DrawFlatButton(prevBtn.rect, "<", buttonBlue, false);
        DrawFlatButton(playBtn.rect, render.isPlaying ? "||" : "O", buttonBlue, false);
        DrawFlatButton(nextBtn.rect, ">", buttonBlue, false);
        Color instantColor = render.stepMode ? buttonBlue : GREEN;
        DrawFlatButton(instant.rect, "Instant", instantColor, false);
        DrawPseudoCode(pseudoRect, pseudo);
        EndDrawing();
    }
    CloseWindow();
}