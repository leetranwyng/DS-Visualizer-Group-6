#include "HashTable.h"
#include "../../UI/UI.h"
#include "HashTableRenderer.h"

void RenderHashTable() {
    const int screenWidth = 1200;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Double Hashing Hash Table Visualizer");
    SetTargetFPS(60);
    srand((unsigned int)time(0));

    bool isMenuExpanded = true;
    int currentAction = ACTION_NONE;

    string alphaText = " ";
    string searchResultText = " ";

    doubleHash* myHash = new doubleHash(11);
    myHash->insert(66, false);
    myHash->insert(90, false);
    myHash->insert(14, false);
    myHash->insert(81, false);
    myHash->insert(16, false);
    myHash->insert(52, false);

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
                    myHash = new doubleHash(m);

                    for (int i = 0; i < n; i++) {
                        int randValue = rand() % 99 + 1;
                        myHash->insert(randValue, false);
                    }
                    float alpha = (float)n / (float)m;
                    alphaText = TextFormat("random integers (α = N/M = %0.1f)", alpha);

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

        DrawText("Double Hashing Hash Map", 60, 25, 32, DARKGRAY);
        DrawText("Green: Occupied | White (E): Empty (-1) | Red (D): Deleted (-2)", 60, 60, 20, GRAY);
        DrawRectangle(0, 0, 35, screenHeight, BLACK);

        // MENU
        if (isMenuExpanded) {
            DrawFlatButton(createMenuButton.rect, "Create(M, N)", menuColor, true);
            DrawFlatButton(searchMenuButton.rect, "Search(v)", menuColor, true);
            DrawFlatButton(insertMenuButton.rect, "Insert(v)", menuColor, true);
            DrawFlatButton(removeMenuButton.rect, "Remove(v)", menuColor, true);
            DrawFlatButton(collapseButton.rect, "<", toggleColor, false);
        }
        else {
            DrawFlatButton(expandButton.rect, ">", toggleColor, false);
        }

        if (isMenuExpanded) {
            int labelOffsetY = 12;
            switch (currentAction) {
            case ACTION_CREATE:
                DrawLabel(panelX, createMenuButton.rect.y + labelOffsetY, "New HT of size M =");
                mInput.Draw();
                DrawLabel(mInput.rect.x + mInput.rect.width + 5, createMenuButton.rect.y + labelOffsetY, "and N =");
                nInput.Draw();
                DrawLabel(nInput.rect.x + nInput.rect.width + 5, createMenuButton.rect.y + labelOffsetY, "random integers");
                DrawFlatButton(createGoButton.rect, "Go", toggleColor, false);
                break;
            case ACTION_SEARCH:
                DrawLabel(panelX, searchMenuButton.rect.y + labelOffsetY, "v =");
                searchVInput.Draw();
                DrawFlatButton(searchGoButton.rect, "Go", toggleColor, false);
                DrawLabel(searchGoButton.rect.x + searchGoButton.rect.width + 20, searchGoButton.rect.y + labelOffsetY, searchResultText, BLUE);
                break;
            case ACTION_INSERT:
                DrawLabel(panelX, insertMenuButton.rect.y + labelOffsetY, "v =");
                insertVInput.Draw();
                DrawFlatButton(insertGoButton.rect, "Go", toggleColor, false);
                break;
            case ACTION_REMOVE:
                DrawLabel(panelX, removeMenuButton.rect.y + labelOffsetY, "v =");
                removeVInput.Draw();
                DrawFlatButton(removeGoButton.rect, "Go", toggleColor, false);
                DrawLabel(removeGoButton.rect.x + removeGoButton.rect.width + 20, removeGoButton.rect.y + labelOffsetY, searchResultText, ORANGE);
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