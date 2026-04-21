#include "HashTable.h"
#include "../../UI/UI.h"
#include "HashTableRenderer.h"

void RenderHashTable() {
    const int screenWidth = 1200;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Hash Table Visualizer (LP, QP, DH, SC)");
    SetTargetFPS(60);
    srand((unsigned int)time(0));

    bool isMenuExpanded = true;
    int currentAction = ACTION_NONE;

    int currentProbingMode = 0; // 0: LP, 1: QP, 2: DH, 3: SC
    hashTableVis* myHash = new hashTableVis(30, currentProbingMode);

    for (int i = 0; i < 20; i++) {
        int randValue = rand() % 99 + 1;
        myHash->insert(randValue, false);
    }
    myHash->startReveal();

    // UI CONFIG
    float menuWidth_Expanded = 180;
    float menuWidth_Collapsed = 35;
    float buttonHeight = 45;

    float bottomBarHeight = 40;
    float startY = screenHeight - bottomBarHeight - (4 * buttonHeight) - 80;

    Color menuColor = { 210, 80, 50, 255 };
    Color toggleColor = { 190, 60, 40, 255 };

    Button collapseButton(0, startY, menuWidth_Collapsed, 5 * buttonHeight, "<", toggleColor);
    Button expandButton(0, startY, menuWidth_Collapsed, 5 * buttonHeight, ">", toggleColor);

    Button createMenuButton(menuWidth_Collapsed, startY, menuWidth_Expanded, buttonHeight, "Create(M, N)", menuColor);
    Button searchMenuButton(menuWidth_Collapsed, startY + buttonHeight, menuWidth_Expanded, buttonHeight, "Search(v)", menuColor);
    Button insertMenuButton(menuWidth_Collapsed, startY + 2 * buttonHeight, menuWidth_Expanded, buttonHeight, "Insert(v)", menuColor);
    Button removeMenuButton(menuWidth_Collapsed, startY + 3 * buttonHeight, menuWidth_Expanded, buttonHeight, "Remove(v)", menuColor);
    Button loadMenuButton(menuWidth_Collapsed, startY + 4 * buttonHeight, menuWidth_Expanded, buttonHeight, "Load File", menuColor);

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

    Button loadGoButton(panelX + MeasureText("Read 'File_Input.txt' ", 20), loadMenuButton.rect.y, 50, buttonHeight, "Go", toggleColor);
    string loadStatusText = "";

    Slider speedSlider(40, screenHeight - 40, 200, 20, 1.5f, 0.05f, 0.5f);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        myHash->updateAnimation(deltaTime);

        Vector2 mousePosition = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        string textLP = (currentProbingMode == 0) ? "LINEAR PROBING" : "LP";
        string textQP = (currentProbingMode == 1) ? "QUADRATIC PROBING" : "QP";
        string textDH = (currentProbingMode == 2) ? "DOUBLE HASHING" : "DH";
        string textSC = (currentProbingMode == 3) ? "SEPARATE CHAINING" : "SC";

        int widthSC = MeasureText(textSC.c_str(), 25) + 20;
        int widthDH = MeasureText(textDH.c_str(), 25) + 20;
        int widthQP = MeasureText(textQP.c_str(), 25) + 20;
        int widthLP = MeasureText(textLP.c_str(), 25) + 20;

        int modeSpacing = 10;
        int startX_SC = screenWidth - widthSC - 20;
        int startX_DH = startX_SC - widthDH - modeSpacing;
        int startX_QP = startX_DH - widthQP - modeSpacing;
        int startX_LP = startX_QP - widthLP - modeSpacing;

        Color colorLP = (currentProbingMode == 0) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };
        Color colorQP = (currentProbingMode == 1) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };
        Color colorDH = (currentProbingMode == 2) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };
        Color colorSC = (currentProbingMode == 3) ? Color{ 210, 210, 210, 255 } : Color{ 240, 240, 240, 255 };

        Button modeBtnLP(startX_LP, 20, widthLP, 40, textLP, colorLP);
        Button modeBtnQP(startX_QP, 20, widthQP, 40, textQP, colorQP);
        Button modeBtnDH(startX_DH, 20, widthDH, 40, textDH, colorDH);
        Button modeBtnSC(startX_SC, 20, widthSC, 40, textSC, colorSC);

        if (modeBtnLP.isPressed(mousePosition, mousePressed) && currentProbingMode != 0) { currentProbingMode = 0; currentAction = ACTION_NONE; }
        else if (modeBtnQP.isPressed(mousePosition, mousePressed) && currentProbingMode != 1) { currentProbingMode = 1; currentAction = ACTION_NONE; }
        else if (modeBtnDH.isPressed(mousePosition, mousePressed) && currentProbingMode != 2) { currentProbingMode = 2; currentAction = ACTION_NONE; }
        else if (modeBtnSC.isPressed(mousePosition, mousePressed) && currentProbingMode != 3) { currentProbingMode = 3; currentAction = ACTION_NONE; }

        static int prevMode = -1;
        if (prevMode != currentProbingMode) {
            prevMode = currentProbingMode;
            int currentTableSize = (currentProbingMode == 3) ? 13 : 30;
            delete myHash;
            myHash = new hashTableVis(currentTableSize, currentProbingMode);
            int numItems = (currentProbingMode == 3) ? 10 : 20;
            for (int i = 0; i < numItems; i++) {
                int randValue = rand() % 99 + 1;
                myHash->insert(randValue, false);
            }
            myHash->startReveal();
        }

        if (isMenuExpanded) {
            switch (currentAction) {
            case ACTION_CREATE: mInput.checkPressed(mousePosition, mousePressed); mInput.Update(); nInput.checkPressed(mousePosition, mousePressed); nInput.Update(); break;
            case ACTION_SEARCH: searchVInput.checkPressed(mousePosition, mousePressed); searchVInput.Update(); break;
            case ACTION_INSERT: insertVInput.checkPressed(mousePosition, mousePressed); insertVInput.Update(); break;
            case ACTION_REMOVE: removeVInput.checkPressed(mousePosition, mousePressed); removeVInput.Update(); break;
            case ACTION_LOAD: break;
            case ACTION_NONE: break;
            }
        }

        if (!isMenuExpanded && expandButton.isPressed(mousePosition, mousePressed)) isMenuExpanded = true;
        else if (isMenuExpanded && collapseButton.isPressed(mousePosition, mousePressed)) { isMenuExpanded = false; currentAction = ACTION_NONE; }

        if (isMenuExpanded) {
            if (createMenuButton.isPressed(mousePosition, mousePressed)) currentAction = ACTION_CREATE;
            else if (searchMenuButton.isPressed(mousePosition, mousePressed)) currentAction = ACTION_SEARCH;
            else if (insertMenuButton.isPressed(mousePosition, mousePressed)) currentAction = ACTION_INSERT;
            else if (removeMenuButton.isPressed(mousePosition, mousePressed)) currentAction = ACTION_REMOVE;
            else if (loadMenuButton.isPressed(mousePosition, mousePressed)) { currentAction = ACTION_LOAD; loadStatusText = ""; }
        }

        if (isMenuExpanded) {
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
                    myHash->startReveal();
                    mInput.Clear(); nInput.Clear();
                }
            }
            else if (currentAction == ACTION_SEARCH && searchGoButton.isPressed(mousePosition, mousePressed)) {
                int v = searchVInput.GetValue();
                if (v != -1) { myHash->search(v); searchVInput.Clear(); }
            }
            else if (currentAction == ACTION_INSERT && insertGoButton.isPressed(mousePosition, mousePressed)) {
                int v = insertVInput.GetValue();
                if (v != -1) { myHash->insert(v); insertVInput.Clear(); }
            }
            else if (currentAction == ACTION_REMOVE && removeGoButton.isPressed(mousePosition, mousePressed)) {
                int v = removeVInput.GetValue();
                if (v != -1) { myHash->erase(v); removeVInput.Clear(); }
            }
            else if (currentAction == ACTION_LOAD && loadGoButton.isPressed(mousePosition, mousePressed)) {
                ifstream ifs("../../../resources/File_Input.txt");

                if (ifs.is_open()) {
                    vector<int> fileVals;
                    int temp;
                    while (ifs >> temp) {
                        fileVals.push_back(temp);
                    }
                    ifs.close();

                    delete myHash;
                    int currentTableSize = (currentProbingMode == 3) ? 13 : 30;
                    myHash = new hashTableVis(currentTableSize, currentProbingMode);

                    for (int v : fileVals) {
                        myHash->insert(v, false);
                    }

                    myHash->startReveal();
                    loadStatusText = "Loaded " + to_string(fileVals.size()) + " items!";
                }
                else {
                    loadStatusText = "Error: File not found!";
                }
            }
        }

        speedSlider.Update(mousePosition, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        myHash->animationDelay = speedSlider.GetValue();

        int btnW = 50;
        int btnH = 40;
        int spacing = 15;
        int btnStartX = (screenWidth - (5 * btnW + 4 * spacing)) / 2 + 10;
        int btnY = screenHeight - 50;

        Button btnSkipPrev(btnStartX, btnY, btnW, btnH, "|<", BLACK);
        Button btnStepPrev(btnStartX + btnW + spacing, btnY, btnW, btnH, "<<", BLACK);
        Button btnPlayPause(btnStartX + 2 * (btnW + spacing), btnY, btnW, btnH, myHash->isPaused ? ">" : "||", BLACK);
        Button btnStepNext(btnStartX + 3 * (btnW + spacing), btnY, btnW, btnH, ">>", BLACK);
        Button btnSkipNext(btnStartX + 4 * (btnW + spacing), btnY, btnW, btnH, ">|", BLACK);

        if (btnSkipPrev.isPressed(mousePosition, mousePressed)) myHash->skipToStart();
        if (btnStepPrev.isPressed(mousePosition, mousePressed)) myHash->stepBackward();
        if (btnPlayPause.isPressed(mousePosition, mousePressed)) myHash->togglePause();
        if (btnStepNext.isPressed(mousePosition, mousePressed)) myHash->stepForward();
        if (btnSkipNext.isPressed(mousePosition, mousePressed)) myHash->skipToEnd();

        BeginDrawing();
        ClearBackground({ 240, 240, 240, 255 });

        myHash->draw(screenWidth, screenHeight, mousePosition, mousePressed);

        string titleText = "Hash Map Visualizer";
        if (currentProbingMode == 0) titleText = "Linear Probing Hash Map";
        else if (currentProbingMode == 1) titleText = "Quadratic Probing Hash Map";
        else if (currentProbingMode == 2) titleText = "Double Hashing Hash Map";
        else if (currentProbingMode == 3) titleText = "Separate Chaining Hash Map";

        DrawText(titleText.c_str(), 60, 25, 32, DARKGRAY);
        DrawText("Green: Occupied | White (E): Empty (-1) | Red (D): Deleted (-2)", 60, 60, 20, GRAY);
        DrawRectangle(0, 0, 35, screenHeight, BLACK);

        modeBtnLP.draw(false, false, false);
        modeBtnQP.draw(false, false, false);
        modeBtnDH.draw(false, false, false);
        modeBtnSC.draw(false, false, false);

        if (isMenuExpanded) {
            createMenuButton.draw(false, true, true);
            searchMenuButton.draw(false, true, true);
            insertMenuButton.draw(false, true, true);
            removeMenuButton.draw(false, true, true);
            loadMenuButton.draw(false, true, true);
            collapseButton.draw(false, true, false);
        }
        else {
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
                createGoButton.draw(false, true, false);
                break;
            case ACTION_SEARCH:
                DrawText("v =", (int)panelX, (int)(searchMenuButton.rect.y + labelOffsetY), 20, BLACK);
                searchVInput.Draw();
                searchGoButton.draw(false, true, false);
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
                break;
            case ACTION_LOAD:
                DrawText("Read 'input.txt'", (int)panelX, (int)(loadMenuButton.rect.y + labelOffsetY), 20, BLACK);
                loadGoButton.draw(false, true, false);
                if (loadStatusText != "") {
                    Color statusColor = (loadStatusText[0] == 'E') ? RED : BLUE;
                    DrawText(loadStatusText.c_str(), (int)(loadGoButton.rect.x + loadGoButton.rect.width + 10), (int)(loadMenuButton.rect.y + labelOffsetY), 20, statusColor);
                }
                break;
            case ACTION_NONE: break;
            }
        }

        DrawRectangle(0, screenHeight - 60, screenWidth, 60, BLACK);

        speedSlider.Draw();

        string speedText = "";
        if (myHash->animationDelay <= 0.06f) speedText = "Run at once";
        else if (myHash->animationDelay <= 0.15f) speedText = "x4.0";
        else if (myHash->animationDelay <= 0.35f) speedText = "x2.0";
        else if (myHash->animationDelay <= 0.7f) speedText = "x1.0";
        else if (myHash->animationDelay <= 1.2f) speedText = "x0.5";
        else speedText = "x0.25";

        DrawText(speedText.c_str(), 260, screenHeight - 41, 23, WHITE); 

        btnSkipPrev.draw(false, true, false);
        btnStepPrev.draw(false, true, false);
        btnPlayPause.draw(false, true, false);
        btnStepNext.draw(false, true, false);
        btnSkipNext.draw(false, true, false);

        EndDrawing();
    }

    delete myHash;
    CloseWindow();
}