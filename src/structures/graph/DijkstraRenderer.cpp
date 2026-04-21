#include "Dijkstra.h"
#include "DijkstraRenderer.h"
#include <iostream>


int sourceNode = -1, old_sourceNode = -1;
int isNextStep = 0, isPlaying = 0;
float currentTime = 0, speedSlider = 1.0f;

int sizeInput = -1;
bool isConfirmed = 0, isPopUp = 0, isSize = 0;
vector<state> currentHistory, oldHistory;

vector<vector<int>> disHistory(1000, vector<int>(1, 1e9));


void resetState()
{
    sourceNode = old_sourceNode = -1;
    isNextStep = isPlaying = 0;
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
        currentTime += dt * speedSlider;

        if (currentTime > 1.0)
        {

            Vector2 posU = Visual->node[s.node].pos;
            Vector2 posV = Visual->node[s.updatedNode].pos;

            DrawLineEx(posU, posV, 2.0f, GREEN);
            DrawCircle(posU.x, posU.y, Visual->radius, WHITE);
            DrawCircle(posV.x, posV.y, Visual->radius, WHITE);
            DrawCircle(posU.x, posU.y, Visual->radius, { 120,120,120,225 });
            DrawCircle(posV.x, posV.y, Visual->radius, { 120,120,120,225 });

            string s1 = tool->convert(s.node);
            const char* c1 = s1.c_str();
            DrawText(c1, posU.x - 7, posU.y - 14, 30, BLACK);

            s1 = tool->convert(s.updatedNode);
            const char* c2 = s1.c_str();
            DrawText(c2, posV.x - 7, posV.y - 14, 30, BLACK);

            tool->drawArrow(posU, posV, Visual->radius, GREEN);


        }

        if (currentTime > 2.0)
        {
            Vector2 pos = Visual->node[s.updatedNode].pos;
            DrawCircle(pos.x, pos.y, Visual->radius, ORANGE);
            //DrawCircle(pos.x, pos.y, Visual->radius, {120,120,120,225});
            //DrawCircleLines(pos.x, pos.y, radius, BLACK);

            string s1 = tool->convert(s.updatedNode);
            const char* c1 = s1.c_str();
            DrawText(c1, pos.x - 7, pos.y - 14, 30, BLACK);


        }

        if (currentTime > 2.5)
        {
            Visual->node[s.updatedNode].d = s.newDis;
        }

        if (currentTime > 3.0)
        {
            isNextStep--;
            currentTime = 0;
        }


    }
}

void back_step(Button*& back_button, Dijkstra*& Graph, UI*& Visual, Tool*& tool)
{
    if (!isNextStep && isSourceNode(sourceNode, Graph) && !isPlaying && !isPopUp && back_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
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


        }
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

    //cout<<sourceNode<<endl;

    if (!isNextStep && !isPlaying && isSourceNode(sourceNode, Graph) && !isPopUp && sourceNode != old_sourceNode)
    {
        Graph->implement(sourceNode);

        for (int i = 0; i < Graph->size; i++)
        {
            Visual->node[i].d = 1e9;
        }

        old_sourceNode = sourceNode;
        Visual->node[sourceNode].d = 0;

        for (int i = 0; i < Graph->size; i++)
        {
            disHistory[i].clear();
            disHistory[i].push_back(1e9);
        }

        disHistory[sourceNode][0] = 0;

        oldHistory.clear();
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

    Button* random_button = new Button(1000, 50, 200, 50, "Random", LIGHTGRAY);
    InputBox* source_node = new InputBox(1000, 150, 200, 50);

    Button* next_button = new Button(1000, 250, 200, 50, "Next Step", LIGHTGRAY);
    Tool* tool = new Tool;

    Button* back_button = new Button(1000, 350, 200, 50, "Back Step", LIGHTGRAY);

    Button* play_button = new Button(1000, 450, 200, 50, "Play", LIGHTGRAY);

    Slider* speed_slider = new Slider(1000, 550, 200, 20, 0.1f, 2.0f, 1.0f);

    Button* initialize_button = new Button(1000, 600, 200, 50, "Initialize", LIGHTGRAY);

    InputBox* size_input = new InputBox(680 - 200 + 100 + 450, 425 - 225 + 150, 200, 50);

    Button* confirm_button = new Button(680 - 200 + 100 + 450, 425 - 225 + 300, 200, 50, "Confirm", LIGHTGRAY);

    InputBox* u_input = new InputBox(680 - 200 + 70 + 450, 425 - 225 + 180, 100, 50);

    InputBox* v_input = new InputBox(680 - 200 + 260 + 450, 425 - 225 + 180, 100, 50);

    InputBox* w_input = new InputBox(680 - 200 + 70 + 450, 425 - 225 + 250, 100, 50);

    Button* addEdge_button = new Button(680 - 200 + 70 + 450, 425 - 225 + 320, 100, 50, "Add Edge", LIGHTGRAY);

    Button* finish_button = new Button(680 - 200 + 260 + 450, 425 - 225 + 320, 100, 50, "Finish", RED);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Visual->moveNode();
        Visual->drawNode(Graph);

        speed_slider->Draw();
        speed_slider->Update(GetMousePosition(), IsMouseButtonDown(MOUSE_LEFT_BUTTON));

        speedSlider = speed_slider->GetValue();
        speedSlider = scaleSpeed(speedSlider);
        //cout<<speedSlider<<endl;

        random_button->draw();
        random_step(random_button, Graph, Visual, tool);

        source_node->Draw();
        sourceNodeInput(source_node, Graph, Visual, tool);

        next_button->draw();
        if (!isPlaying) next_step(next_button, Graph, Visual, tool, speedSlider);

        back_button->draw();
        back_step(back_button, Graph, Visual, tool);

        play_button->draw();
        if (!isNextStep && isSourceNode(sourceNode, Graph) && !isPopUp && !isPlaying && play_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
        {
            isPlaying = 1;
            currentTime = 0;
        }

        if (isPlaying)
        {
            next_step(next_button, Graph, Visual, tool, speedSlider);
        }

        initialize_button->draw();
        if (!isPopUp && !isPlaying && !isNextStep && initialize_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
        {
            isPopUp = 1;
            isConfirmed = 0;
        }

        if (isPopUp)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0,0,0,100 });
            DrawRectangle(680 - 200 + 450, 425 - 225, 400, 450, WHITE);
            DrawRectangleLinesEx({ 680 - 200 + 450, 425 - 225, 400, 450 }, 5, DARKGRAY);
            DrawText("Initialize Graph", 680 - 200 + 80 + 450, 425 - 225 + 10, 30, BLACK);

            if (!isConfirmed)
            {
                DrawText("Number of vertices:", 680 - 200 + 100 + 450, 425 - 225 + 70, 20, BLACK);
                size_input->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                size_input->Update();
                size_input->Draw();

                sizeInput = size_input->GetValue();

                confirm_button->draw();

                if (sizeInput != -1)
                {
                    DrawText("Valid", 680 - 200 + 315 + 450, 425 - 225 + 165, 15, GREEN);

                    if (confirm_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
                    {
                        isConfirmed = 1;
                        Graph->size = sizeInput;
                        Graph->build_size();
                        Visual->placeNode(Graph);

                    }

                }
                else
                {
                    DrawText("Invalid", 680 - 200 + 315 + 450, 425 - 225 + 165, 15, RED);
                }
            }
            else
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

                //cout<<sizeInput<<endl;
                DrawRectangle(680 - 200 + 260 + 450, 425 - 225 + 250, 100, 50, { 220, 220, 220, 255 });
                if (u_input->GetValue() != -1 && u_input->GetValue() < sizeInput && v_input->GetValue() != -1 && v_input->GetValue() < sizeInput && w_input->GetValue() != -1)
                {
                    DrawText("Valid", 680 - 200 + 282 + 450, 425 - 228 + 260, 20, GREEN);
                    if (addEdge_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
                    {

                        Graph->build_edge(u_input->GetValue(), v_input->GetValue(), w_input->GetValue());
                        Visual->placeNode(Graph);

                        u_input->Clear();
                        v_input->Clear();
                        w_input->Clear();
                    }

                }
                else
                {
                    DrawText("Invalid", 680 - 200 + 277 + 450, 425 - 228 + 260, 20, RED);
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

        EndDrawing();
    }

    CloseWindow();
}