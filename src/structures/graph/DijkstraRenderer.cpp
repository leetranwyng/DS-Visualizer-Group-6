#include "Dijkstra.h"
#include "DijkstraRenderer.h"
#include <iostream>
#include <fstream>

int sourceNode = -1, old_sourceNode = -1;
int isNextStep = 0, isPlaying = 0, currentLine = -100;
float currentTime = 0, speedSlider = 1.0f;

int sizeInput = -1;
bool isConfirmed = 0, isPopUp = 0, isSize = 0, isBeginStep = 0;
vector<state> currentHistory, oldHistory;

vector<vector<int>> disHistory(1000, vector<int>(1, 1e9));

vector<pair<string, int>> pseudo = {
    {"for each node u: dist[u] = INF", -1},
    {"dist[source] = 0", -1},
    {"push source into priority queue", -1},
    {"while priority queue is not empty:", 0},
    {"    u = extract node with smallest distance", 1},
    {"    for each neighbor v of u:", 2},
    {"        if dist[v] > dist[u] + weight(u,v):", 3},
    {"            dist[v] = dist[u] + weight(u,v)", 4},
    {"            push v into priority queue", 4}
};

void drawPseudo(Dijkstra* &Graph)
{
    float posX = 860, posY = 420;

    DrawRectangleRounded({posX-10, posY-20, 500, 300}, 0.1f, 5, {255,255,255,200});
    DrawRectangleRoundedLines({posX-10, posY-20, 500, 300}, 0.1f, 5, DARKGRAY);

    DrawText("Pseudocode:", posX+12, posY-60, 25, BLACK);

    for (int i = 0; i < pseudo.size(); i++)
    {
        Color c = GRAY;
        if (pseudo[i].second == currentLine) 
        {
            c = ORANGE;
            if (currentLine == 0 || currentLine == 2) c = BLACK;
        }

        DrawText(pseudo[i].first.c_str(), posX+7, posY+i*30, 20, c);
    }
    if (sourceNode!=-1 && sourceNode<Graph->size)
    {
        DrawText(pseudo[0].first.c_str(), posX+7, posY+0*30, 20, BLUE);
        DrawText(pseudo[1].first.c_str(), posX+7, posY+1*30, 20, BLUE);
        DrawText(pseudo[2].first.c_str(), posX+7, posY+2*30, 20, BLUE);
    }
}

void resetState()
{
    sourceNode = old_sourceNode = -1;
    isNextStep = isPlaying = isBeginStep = 0;
    currentLine = -100;
    currentTime = 0;
    speedSlider = 1.0f;
    sizeInput = -1;
    isConfirmed = isPopUp = isSize = 0;
    currentHistory.clear();
    oldHistory.clear();
    for (auto& u : disHistory)
    {
        u.clear();
        u.push_back(1e9);
    }
}

bool isSourceNode(int sourceNode, Dijkstra*& Graph)
{
    if (sourceNode != -1 && sourceNode < Graph->size) return 1;
    return 0;
}

void next_step(Button*& next_button, Dijkstra*& Graph, UI*& Visual, Tool*& tool, float speedSlider)
{
    if (!isNextStep && isSourceNode(sourceNode, Graph) && !isPopUp && (next_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || isPlaying))
    {
        if (Graph->history.size())
        {
            state s = Graph->history[0];
            currentHistory.clear();
            currentHistory.push_back(s);

            Graph->history.erase(Graph->history.begin());

            state upd = s;
            disHistory[upd.updatedNode].push_back(upd.newDis);
            int indexMax = disHistory[upd.updatedNode].size();
            oldHistory.push_back(upd);


            while (Graph->history.size() && Graph->history[0].node == s.node)
            {

                upd = Graph->history[0];
                disHistory[upd.updatedNode].push_back(upd.newDis);
                indexMax = disHistory[upd.updatedNode].size();

                currentHistory.push_back(Graph->history[0]);
                oldHistory.push_back(Graph->history[0]);
                Graph->history.erase(Graph->history.begin());
            }

            for (int i = 0; i < Graph->size; i++)
            {
                if (disHistory[i].size() < indexMax)
                {
                    disHistory[i].push_back(disHistory[i].back());
                }
            }

            isNextStep = currentHistory.size();
            currentTime = 0;
        }
        else isPlaying = 0;
    }

    if (isNextStep)
    {
        state s = currentHistory[currentHistory.size() - isNextStep];
        float dt = GetFrameTime();
        currentTime += dt*speedSlider;
        if (currentTime <= 0.5)
        {
            currentLine = 0;
        } else
        if (currentTime <= 1.5)
        {
            currentLine = 1; 
        } else 
        if (currentTime <= 2.5)
        {
            currentLine = 2;
        } else 
        if (currentTime <= 3)
        {
            currentLine = 3;
        } else 
        if (currentTime <= 3.5)
        {
            if (Visual->node[s.updatedNode].d != s.newDis) currentLine = 4;
        } else
        {
            currentLine = -100;
        }

        if (currentTime > 0.5)
        {
            Vector2 posU = Visual->node[s.node].pos;
            DrawCircle(posU.x, posU.y, Visual->radius, WHITE);
            DrawCircle(posU.x, posU.y, Visual->radius, {80,140,255,180});

            string s1 = tool->convert(s.node);
            const char* c1 = s1.c_str();
            DrawText(c1, posU.x-MeasureText(c1, 30)/2, posU.y-30/2, 30, BLACK);
        }

        if (currentTime > 1.5)
        {
            
            Vector2 posU = Visual->node[s.node].pos;
            Vector2 posV = Visual->node[s.updatedNode].pos;

            DrawLineEx(posU, posV, 2.0f, GREEN);
            DrawCircle(posU.x, posU.y, Visual->radius, WHITE);
            DrawCircle(posV.x, posV.y, Visual->radius, WHITE);
            DrawCircle(posU.x, posU.y, Visual->radius, {80,140,255,180});
            DrawCircle(posV.x, posV.y, Visual->radius, {120,120,120,225});

            string s1 = tool->convert(s.node);
            const char* c1 = s1.c_str();
            DrawText(c1, posU.x-MeasureText(c1, 30)/2, posU.y-30/2, 30, BLACK);

            s1 = tool->convert(s.updatedNode);
            const char* c2 = s1.c_str();
            DrawText(c2, posV.x-MeasureText(c2, 30)/2, posV.y-30/2, 30, BLACK);

            tool->drawArrow(posU, posV, Visual->radius, GREEN);


        }

        if (currentTime > 2.5)
        {
            
            Vector2 pos = Visual->node[s.updatedNode].pos;
            DrawCircle(pos.x, pos.y, Visual->radius, ORANGE);

            string s1 = tool->convert(s.updatedNode);
            const char* c1 = s1.c_str();
            DrawText(c1, pos.x-MeasureText(c1, 30)/2, pos.y-30/2, 30, BLACK);

        }

        if (currentTime > 3)
        {
            
            Visual->node[s.updatedNode].d = s.newDis;
            if (currentLine == 4)
            {
                Vector2 posU = Visual->node[s.updatedNode].pos;
                DrawCircle(posU.x, posU.y, Visual->radius, WHITE);
                DrawCircle(posU.x, posU.y, Visual->radius, {80,200,120,160});

                string s1 = tool->convert(s.updatedNode);
                const char* c1 = s1.c_str();
                DrawText(c1, posU.x-MeasureText(c1, 30)/2, posU.y-30/2, 30, BLACK);
            }
        }

        if (currentTime > 3.5)
        {
            isNextStep--;
            currentTime = 0;
        }


    }
}

void back_step(Button*& back_button, Dijkstra*& Graph, UI*& Visual, Tool*& tool)
{
    if (!isNextStep && isSourceNode(sourceNode, Graph) && !isPlaying && !isPopUp && (back_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || isBeginStep))
    {
        if (oldHistory.size())
        {
            state s = oldHistory.back();
            oldHistory.pop_back();

            Graph->history.insert(Graph->history.begin(), s);
            while (oldHistory.size() && oldHistory.back().node == s.node)
            {
                Graph->history.insert(Graph->history.begin(), oldHistory.back());
                oldHistory.pop_back();
            }

            for (int i = 0; i < Graph->size; i++)
            {
                disHistory[i].pop_back();
                Visual->node[i].d = disHistory[i].back();
            }


        } else isBeginStep = 0;
    }
}

void random_step(Button*& random_button, Dijkstra*& Graph, UI*& Visual, Tool*& tool)
{
    if (!isNextStep && !isPlaying && !isPopUp && random_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
    {
        Graph->randomGraph(tool->random(2, 7));
        Visual->placeNode(Graph);
        sourceNode = -1;
        old_sourceNode = -1;

        oldHistory.clear();

        for (int i = 0; i < Graph->size; i++)
        {
            disHistory[i].clear();
            disHistory[i].push_back(1e9);
        }
    }
}

float scaleSpeed(float speedSlider)
{
    if (speedSlider <= 1.0) return speedSlider;
    return (speedSlider - 1.0f) * 9.0f + 1.0f;
}

void sourceNodeInput(InputBox*& source_node, Dijkstra*& Graph, UI*& Visual, Tool*& tool)
{
    if (!isNextStep && !isPlaying && !isPopUp)
    {
        source_node->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

        sourceNode = source_node->GetValue();
        source_node->Update();
    }

    if (!isSourceNode(sourceNode, Graph))
    {
        for (int i = 0; i < Graph->size; i++)
        {
            Visual->node[i].d = 1e9;
        }
    }

    if (sourceNode!=old_sourceNode)
    {
        if (isSourceNode(sourceNode, Graph))
        {
            Graph->implement(sourceNode);
        
            for (int i = 0; i < Graph->size; i++)
            {
                Visual->node[i].d = 1e9;
            }

            Visual->node[sourceNode].d = 0;
            
            for (int i = 0; i < Graph->size; i++)
            {
                disHistory[i].clear();
                disHistory[i].push_back(1e9);
            }

            disHistory[sourceNode][0] = 0;

            oldHistory.clear();
        }
        old_sourceNode = sourceNode;
    }
}


void play_step(Dijkstra*& Graph, UI*& Visual, Tool*& tool, Button*& play_button, Button*& next_button)
{
    if (!isNextStep && isSourceNode(sourceNode, Graph) && !isPopUp && !isPlaying && play_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
    {
        isPlaying = 1;
        currentTime = 0;
    }

    if (isPlaying)
    {
        next_step(next_button, Graph, Visual, tool, speedSlider);
    }
}



void initialize_step(Dijkstra*& Graph, UI*& Visual, Button*& initialize_button, InputBox*& size_input, Button*& confirm_button, Button*& addEdge_button, Button*& finish_button, InputBox*& u_input, InputBox*& v_input, InputBox*& w_input)
{
    if (!isPopUp && !isPlaying && !isNextStep && initialize_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
    {
        isPopUp = 1;
        isConfirmed = 0;
    }

    if (isPopUp)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0,0,0,100});
        DrawRectangle(680 - 200 + 450, 425 - 225, 400, 450, WHITE);
        DrawRectangleLinesEx({680 - 200 + 450, 425 - 225, 400, 450}, 5, DARKGRAY);
        DrawText("Initialize Graph", 680 - 200 + 80 + 450, 425 - 225 + 10, 30, BLACK);

        if (!isConfirmed)
        {
            DrawText("Number of vertices:", 680 - 200 + 100 + 450, 425 - 225 + 70, 20, BLACK);
            size_input->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            size_input->Update();
            size_input->Draw();

            sizeInput = size_input->GetValue();
            
            confirm_button->draw();

            if (sizeInput!=-1)
            {
                DrawText("Valid", 680 - 200 + 315 + 450, 425 - 225 + 165, 15, GREEN);
                
                if (confirm_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
                {
                    isConfirmed = 1;
                    Graph->size = sizeInput;
                    Graph->build_size();
                    Visual->placeNode(Graph);

                }

            } else
            {
                DrawText("Invalid", 680 - 200 + 315 + 450, 425 - 225 + 165, 15, RED);
            }
        } else
        {
            DrawText("Vertices are indexed from 0 to n-1", 680 - 200 + 25 + 450, 425 - 225 + 60, 20, BLUE);

            DrawText("From u to v with weight w", 680 - 200 + 70 + 450, 425 - 225 + 120 + 1, 20, GREEN);
            DrawText("From u to v with weight w", 680 - 200 + 70 + 450, 425 - 225 + 120, 20, BLACK);

            DrawText("u:", 680 - 200 + 30 + 450, 425 - 225 + 190, 20, BLACK);
            DrawText("v:", 680 - 200 + 220 + 450, 425 - 225 + 190, 20, BLACK);
            DrawText("w:", 680 - 200 + 30 + 450, 425 - 225 + 260, 20, BLACK);

            u_input->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            v_input->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            w_input->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

            u_input->Update();
            v_input->Update();
            w_input->Update();

            u_input->Draw();
            v_input->Draw();
            w_input->Draw();

            addEdge_button->draw();
            finish_button->draw();

            DrawRectangle(680 - 200 + 260 + 450, 425 - 225 + 250, 100, 50, {220, 220, 220, 255});
            if (u_input->GetValue()!=-1 && u_input->GetValue()<sizeInput && v_input->GetValue()!=-1 && v_input->GetValue()<sizeInput && w_input->GetValue()!=-1)
            {
                DrawText("Valid", 680 - 200 + 282 + 450, 425 - 228 + 265, 20, GREEN);
                if (addEdge_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
                {
                    
                    Graph->build_edge(u_input->GetValue(), v_input->GetValue(), w_input->GetValue());
                    Visual->placeNode(Graph);

                    u_input->Clear();
                    v_input->Clear();
                    w_input->Clear();
                }

            } else
            {
                DrawText("Invalid", 680 - 200 + 277 + 450, 425 - 228 + 265, 20, RED);
            }

            if (finish_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            {
                isPopUp = 0;
                isConfirmed = 0;
                u_input->Clear();
                v_input->Clear();
                w_input->Clear();
                size_input->Clear();

                sourceNode = -1;
                old_sourceNode = -1;

                oldHistory.clear();

                for (int i = 0; i < Graph->size; i++)
                {
                    disHistory[i].clear();
                    disHistory[i].push_back(1e9);
                }

            }
        }
        

    }
}

void begin_step(Dijkstra*& Graph, UI*& Visual, Tool*& tool, Button*& back_button, Button*& begin_button)
{
    if (!isPopUp && !isPlaying && !isNextStep && begin_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && isSourceNode(sourceNode, Graph))
    {
        isBeginStep = 1;
        while(isBeginStep)
        {
            back_step(back_button, Graph, Visual, tool);
        }
    }
}

void end_step(Dijkstra*& Graph, UI*& Visual, Button*& end_button)
{
    if (!isPopUp && !isPlaying && !isNextStep && end_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && isSourceNode(sourceNode, Graph))
    {
        while (Graph->history.size())
        {
            state s = Graph->history[0];

            Graph->history.erase(Graph->history.begin());
            
            state upd = s;
            disHistory[upd.updatedNode].push_back(upd.newDis);
            int indexMax = disHistory[upd.updatedNode].size();
            oldHistory.push_back(upd);


            while(Graph->history.size() && Graph->history[0].node==s.node)
            {

                upd = Graph->history[0];
                disHistory[upd.updatedNode].push_back(upd.newDis);
                indexMax = disHistory[upd.updatedNode].size();

                oldHistory.push_back(Graph->history[0]);
                Graph->history.erase(Graph->history.begin());
            }

            for (int i = 0; i < Graph->size; i++)
            {
                if (disHistory[i].size() < indexMax)
                {
                    disHistory[i].push_back(disHistory[i].back());
                }
            }
            
        }

        for (int i = 0; i < Graph->size; i++)
        {
            Visual->node[i].d = disHistory[i].back();
        }
    }
}

void loadFile_step(Dijkstra*& Graph, UI*& Visual, Button*& loadFile_button)
{
    if (!isPopUp && !isPlaying && !isNextStep && loadFile_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
    {
        ifstream fin(getResourcesPath("File_Input_Dijkstra.txt").c_str());
        int n, m;
        fin >> n >> m;

        Graph->size = n;
        Graph->build_size();

        for (int i = 0; i < m; i++)
        {
            int u, v, w;
            fin >> u >> v >> w;
            Graph->build_edge(u, v, w);
            
        }

        Visual->placeNode(Graph);
        sourceNode = -1;
        old_sourceNode = -1;

        oldHistory.clear();

        for (int i = 0; i < Graph->size; i++)
        {
            disHistory[i].clear();
            disHistory[i].push_back(1e9);
        }

        fin.close();
    }
}

void RenderDijkstra()
{
    constexpr int screenWidth = 1360;
    constexpr int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "Dijkstra visualization");
    SetTargetFPS(60);

    resetState();

    Dijkstra* Graph = new Dijkstra;
    Graph->randomGraph(5);

    UI* Visual = new UI;
    Visual->placeNode(Graph);
    
    Button* random_button = new Button(1100, 40, 200, 50, "Random", LIGHTGRAY);

    InputBox* source_node = new InputBox(1100, 220, 200, 50);

    Button* next_button = new Button(480, 655, 120, 50, "Next", LIGHTGRAY);
    Tool* tool = new Tool;

    Button* back_button = new Button(170, 655, 120, 50, "Back", LIGHTGRAY);

    Button* play_button = new Button(325, 655, 120, 50, "Play", LIGHTGRAY);

    Button* begin_button = new Button(70, 655, 80, 50, "|<", LIGHTGRAY);

    Button* end_button = new Button(620, 655, 80, 50, ">|", LIGHTGRAY);

    Slider* speed_slider = new Slider(230, 770, 350, 20, 0.1f, 2.0f, 1.0f);

    Button* initialize_button = new Button(870, 40, 200, 50, "Initialize", LIGHTGRAY);

    InputBox* size_input = new InputBox(680 - 200 + 100 + 450, 425 - 225 + 150, 200, 50);

    Button* confirm_button = new Button(680 - 200 + 100 + 450, 425 - 225 + 300, 200, 50, "Confirm", LIGHTGRAY);

    InputBox* u_input = new InputBox(680 - 200 + 70 + 450, 425 - 225 + 180, 100, 50);

    InputBox* v_input = new InputBox(680 - 200 + 260 + 450, 425 - 225 + 180, 100, 50);

    InputBox* w_input = new InputBox(680 - 200 + 70 + 450, 425 - 225 + 250, 100, 50);

    Button* addEdge_button = new Button(680 - 200 + 70 + 450, 425 - 225 + 320, 100, 50, "Add Edge", LIGHTGRAY);

    Button* finish_button = new Button(680 - 200 + 260 + 450, 425 - 225 + 320, 100, 50, "Finish", RED);

    Button* loadFile_button = new Button(1100, 130, 200, 50, "Load File", LIGHTGRAY);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Rectangle rec = {3, 3, 828, 628};

        DrawRectangleRounded(rec, 0.03f, 8, WHITE);
        DrawRectangleRoundedLines(rec, 0.03f, 8, {180, 180, 180, 255});


        Visual->moveNode();
        Visual->drawNode(Graph);

        speed_slider->Draw();
        if (!isPopUp) speed_slider->Update(GetMousePosition(), IsMouseButtonDown(MOUSE_LEFT_BUTTON));
        
        speedSlider = speed_slider->GetValue();
        speedSlider = scaleSpeed(speedSlider);
        DrawText("Speed:", 150, 770, 20, BLACK);
        char buf[20];
        sprintf(buf, "%.1fx", speedSlider);
        DrawText(buf, 590, 770, 20, BLACK);

        random_button->draw();
        random_step(random_button, Graph, Visual, tool);

        source_node->Draw();
        sourceNodeInput(source_node, Graph, Visual, tool);
        DrawText("Start Node:", 910, 230, 25, BLACK);
        
        next_button->draw();
        if (!isPlaying) next_step(next_button, Graph, Visual, tool, speedSlider);

        drawPseudo(Graph);

        back_button->draw();
        back_step(back_button, Graph, Visual, tool);

        play_button->draw();
        play_step(Graph, Visual, tool, play_button, next_button);


        begin_button->draw();
        begin_step(Graph, Visual, tool, back_button, begin_button);


        end_button->draw();
        end_step(Graph, Visual, end_button);

        loadFile_button->draw();
        DrawRectangleRounded({895, 140, 165, 30}, 0.3f, 8, LIGHTGRAY);
        DrawText("n m | m lines: u v w", 905, 145, 18, BLACK);
        loadFile_step(Graph, Visual, loadFile_button);

        initialize_button->draw();
        initialize_step(Graph, Visual, initialize_button, size_input, confirm_button, addEdge_button, finish_button, u_input, v_input, w_input);
        

        EndDrawing();
    }

    CloseWindow();
}