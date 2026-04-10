#include "HashTable.h"
#include "../../UI/UI.h"
#include "HashTableRenderer.h"

void RenderHashTable() {
    const int screenWidth = 1200;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Hash Table Visualizer (LP, QP, DH)");
    SetTargetFPS(60);
    srand((unsigned int)time(0));

    bool isMenuExpanded = true;
    int currentAction = ACTION_NONE;

    string searchResultText = " ";

    int currentProbingMode = 0; // 0: LP, 1: QP, 2: DH
    hashTableVis* myHash = new hashTableVis(30, currentProbingMode);

    for (int i = 0; i < 20; i++) {
        int randValue = rand() % 99 + 1;
        myHash->insert(randValue, false);
    }

    // UI CONFIG
    float menuWidth_Expanded = 180;
    float menuWidth_Collapsed = 35;
    float buttonHeight = 45;

    float bottomBarHeight = 40;
    float startY = screenHeight - bottomBarHeight - (4 * buttonHeight) - 80;

    Color menuColor = { 210, 80, 50, 255 };
    Color toggleColor = { 190, 60, 40, 255 };

    // COLLAPSE EXPAND CONTROL
    Button collapseButton(0, startY, menuWidth_Collapsed, 4 * buttonHeight, "<", toggleColor);
    Button expandButton(0, startY, menuWidth_Collapsed, 4 * buttonHeight, ">", toggleColor);

    // MAIN MENU
    Button createMenuButton(menuWidth_Collapsed, startY, menuWidth_Expanded, buttonHeight, "Create(M, N)", menuColor);
    Button searchMenuButton(menuWidth_Collapsed, startY + buttonHeight, menuWidth_Expanded, buttonHeight, "Search(v)", menuColor);
    Button insertMenuButton(menuWidth_Collapsed, startY + 2 * buttonHeight, menuWidth_Expanded, buttonHeight, "Insert(v)", menuColor);
    Button removeMenuButton(menuWidth_Collapsed, startY + 3 * buttonHeight, menuWidth_Expanded, buttonHeight, "Remove(v)", menuColor);

    float panelX = menuWidth_Collapsed + menuWidth_Expanded + 15;

    InputBox mInput(panelX + MeasureText("New HT of size M = ", 20), createMenuButton.rect.y, 60, buttonHeight, BLACK, WHITE);
    InputBox nInput(mInput.rect.x + mInput.rect.width + MeasureText(" and N = ", 20), createMenuButton.rect.y, 60, buttonHeight, BLACK, WHITE);
    Button createGoButton(nInput.rect.x + nInput.rect.width + MeasureText(" random integers ", 20), createMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    InputBox searchVInput(panelX + MeasureText("v = ", 20), searchMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    Button searchGoButton(searchVInput.rect.x + searchVInput.rect.width + 5, searchMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    InputBox insertVInput(panelX + MeasureText("v = ", 20), insertMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    Button insertGoButton(insertVInput.rect.x + insertVInput.rect.width + 5, insertMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    InputBox removeVInput(panelX + MeasureText("v = ", 20), removeMenuButton.rect.y, 100, buttonHeight, BLACK, WHITE);
    Button removeGoButton(removeVInput.rect.x + removeVInput.rect.width + 5, removeMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);

    Slider speedSlider(50, 600, 200, 20, 0.05f, 1.5f, 0.5f);
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        myHash->updateAnimation(deltaTime);

        Vector2 mousePosition = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        string textLP = (currentProbingMode == 0) ? "LINEAR PROBING" : "LP";
        string textQP = (currentProbingMode == 1) ? "QUADRATIC PROBING" : "QP";
        string textDH = (currentProbingMode == 2) ? "DOUBLE HASHING" : "DH";

        int widthDH = MeasureText(textDH.c_str(), 25) + 20;
        int widthQP = MeasureText(textQP.c_str(), 25) + 20;
        int widthLP = MeasureText(textLP.c_str(), 25) + 20;

        int modeSpacing = 10;
        int startX_DH = screenWidth - widthDH - 20;
        int startX_QP = startX_DH - widthQP - modeSpacing;
        int startX_LP = startX_QP - widthLP - modeSpacing;

        // Visual indicator for the active mode
        Color colorLP = (currentProbingMode == 0) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };
        Color colorQP = (currentProbingMode == 1) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };
        Color colorDH = (currentProbingMode == 2) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };

        Button modeBtnLP(startX_LP, 20, widthLP, 40, textLP, colorLP);
        Button modeBtnQP(startX_QP, 20, widthQP, 40, textQP, colorQP);
        Button modeBtnDH(startX_DH, 20, widthDH, 40, textDH, colorDH);

        // Mode Switching
        if (!myHash->isAnimating) {
            int nextMode = currentProbingMode;

            if (modeBtnLP.isPressed(mousePosition, mousePressed)) nextMode = 0;
            else if (modeBtnQP.isPressed(mousePosition, mousePressed)) nextMode = 1;
            else if (modeBtnDH.isPressed(mousePosition, mousePressed)) nextMode = 2;

            if (nextMode != currentProbingMode) {
                currentProbingMode = nextMode;
                int currentTableSize = myHash->TABLE_SIZE;
                delete myHash;

                myHash = new hashTableVis(currentTableSize, currentProbingMode);

                for (int i = 0; i < 20; i++) {
                    int randValue = rand() % 99 + 1;
                    myHash->insert(randValue, false);
                }
                searchResultText = " ";
                currentAction = ACTION_NONE;
            }
        }

        // INPUT BOX
        if (isMenuExpanded) {
            switch (currentAction) {
            case ACTION_CREATE: mInput.checkPressed(mousePosition, mousePressed); mInput.Update(); nInput.checkPressed(mousePosition, mousePressed); nInput.Update(); break;
            case ACTION_SEARCH: searchVInput.checkPressed(mousePosition, mousePressed); searchVInput.Update(); break;
            case ACTION_INSERT: insertVInput.checkPressed(mousePosition, mousePressed); insertVInput.Update(); break;
            case ACTION_REMOVE: removeVInput.checkPressed(mousePosition, mousePressed); removeVInput.Update(); break;
            case ACTION_NONE: break;
            }
        }

        // EVENT HANDLING
        if (!isMenuExpanded && expandButton.isPressed(mousePosition, mousePressed)) {
            isMenuExpanded = true;
        }
        else if (isMenuExpanded && collapseButton.isPressed(mousePosition, mousePressed)) {
            isMenuExpanded = false;
            currentAction = ACTION_NONE;
        }

        if (isMenuExpanded) {
            if (createMenuButton.isPressed(mousePosition, mousePressed)) {
                currentAction = ACTION_CREATE; searchResultText = " ";
            }
            else if (searchMenuButton.isPressed(mousePosition, mousePressed)) {
                currentAction = ACTION_SEARCH; searchResultText = " ";
            }
            else if (insertMenuButton.isPressed(mousePosition, mousePressed)) {
                currentAction = ACTION_INSERT; searchResultText = " ";
            }
            else if (removeMenuButton.isPressed(mousePosition, mousePressed)) {
                currentAction = ACTION_REMOVE; searchResultText = " ";
            }
        }

        // GO BUTTON
        if (isMenuExpanded && !myHash->isAnimating) {
            if (currentAction == ACTION_CREATE && createGoButton.isPressed(mousePosition, mousePressed)) {
                int m = mInput.GetValue();
                int n = nInput.GetValue();

                if (m > 0 && n >= 0 && n <= m) {
                    delete myHash;
                    myHash = new hashTableVis(m, currentProbingMode);

                    for (int i = 0; i < n; i++) {
                        int randValue = rand() % 99 + 1;
                        myHash->insert(randValue, false);
                    }

                    mInput.Clear(); nInput.Clear();
                }
            }
            else if (currentAction == ACTION_SEARCH && searchGoButton.isPressed(mousePosition, mousePressed)) {
                int v = searchVInput.GetValue();
                if (v != -1) {
                    int pos = myHash->search(v);
                    if (pos != -1) {
                        searchResultText = TextFormat("%d found at index %d", v, pos);
                    }
                    else {
                        searchResultText = TextFormat("%d not found", v);
                    }
                    searchVInput.Clear();
                }
            }
            else if (currentAction == ACTION_INSERT && insertGoButton.isPressed(mousePosition, mousePressed)) {
                int v = insertVInput.GetValue();
                if (v != -1) {
                    myHash->insert(v);
                    insertVInput.Clear();
                    searchResultText = " ";
                }
            }
            else if (currentAction == ACTION_REMOVE && removeGoButton.isPressed(mousePosition, mousePressed)) {
                int v = removeVInput.GetValue();
                if (v != -1) {
                    myHash->erase(v);
                    searchResultText = TextFormat("%d removed (if present)", v);
                    removeVInput.Clear();
                }
            }
        }

        speedSlider.Update(mousePosition, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        myHash->animationDelay = speedSlider.GetValue();

        // BEGIN DRAWING
        BeginDrawing();
        ClearBackground({ 240, 240, 240, 255 });

        myHash->draw(screenWidth, screenHeight);

        // Dynamic Title
        string titleText = "Hash Map Visualizer";
        if (currentProbingMode == 0) titleText = "Linear Probing Hash Map";
        else if (currentProbingMode == 1) titleText = "Quadratic Probing Hash Map";
        else if (currentProbingMode == 2) titleText = "Double Hashing Hash Map";

        DrawText(titleText.c_str(), 60, 25, 32, DARKGRAY);
        DrawText("Green: Occupied | White (E): Empty (-1) | Red (D): Deleted (-2)", 60, 60, 20, GRAY);
        DrawRectangle(0, 0, 35, screenHeight, BLACK);

        // Draw mode buttons (no border, NOT flat, center aligned)
        modeBtnLP.draw(false, false, false);
        modeBtnQP.draw(false, false, false);
        modeBtnDH.draw(false, false, false);

        // MENU
        if (isMenuExpanded) {
            // Main menu items (no border, flat style, left aligned)
            createMenuButton.draw(false, true, true);
            searchMenuButton.draw(false, true, true);
            insertMenuButton.draw(false, true, true);
            removeMenuButton.draw(false, true, true);

            // Collapse button (no border, flat style, center aligned)
            collapseButton.draw(false, true, false);
        }
        else {
            // Expand button (no border, flat style, center aligned)
            expandButton.draw(false, true, false);
        }

        if (isMenuExpanded) {
            int labelOffsetY = 12;
            switch (currentAction) {
            case ACTION_CREATE:
                DrawText("New HT of size M =", (int)panelX, (int)(createMenuButton.rect.y + labelOffsetY), 20, BLACK);
                mInput.Draw();
                DrawText("and N =", (int)(mInput.rect.x + mInput.rect.width + 5), (int)(createMenuButton.rect.y + labelOffsetY), 20, BLACK);
                nInput.Draw();
                DrawText("random integers", (int)(nInput.rect.x + nInput.rect.width + 5), (int)(createMenuButton.rect.y + labelOffsetY), 20, BLACK);
                createGoButton.draw(false, true, false); // Go buttons (no border, flat style, center aligned)
                break;
            case ACTION_SEARCH:
                DrawText("v =", (int)panelX, (int)(searchMenuButton.rect.y + labelOffsetY), 20, BLACK);
                searchVInput.Draw();
                searchGoButton.draw(false, true, false);
                DrawText(searchResultText.c_str(), (int)(searchGoButton.rect.x + searchGoButton.rect.width + 20), (int)(searchGoButton.rect.y + labelOffsetY), 20, BLUE);
                break;
            case ACTION_INSERT:
                DrawText("v =", (int)panelX, (int)(insertMenuButton.rect.y + labelOffsetY), 20, BLACK);
                insertVInput.Draw();
                insertGoButton.draw(false, true, false);
                break;
            case ACTION_REMOVE:
                DrawText("v =", (int)panelX, (int)(removeMenuButton.rect.y + labelOffsetY), 20, BLACK);
                removeVInput.Draw();
                removeGoButton.draw(false, true, false);
                DrawText(searchResultText.c_str(), (int)(removeGoButton.rect.x + removeGoButton.rect.width + 20), (int)(removeGoButton.rect.y + labelOffsetY), 20, ORANGE);
                break;
            case ACTION_NONE: break;
            }
        }

        speedSlider.Draw();

        EndDrawing();
    }

    delete myHash;
    CloseWindow();
}