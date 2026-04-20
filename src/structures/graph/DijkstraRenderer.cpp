#include "Dijkstra.h"
#include "DijkstraRenderer.h"
#include <iostream>

void RenderDijkstra()
{
    constexpr int screenWidth = 1360;
    constexpr int screenHeight = 850;
    
    InitWindow(screenWidth, screenHeight, "Dijkstra visualization");
    SetTargetFPS(60);

    Dijkstra* Graph = new Dijkstra;
    Graph->randomGraph(5);

    UI* Visual = new UI;
    Visual->placeNode(Graph);
    
    Button* random_button = new Button(1000, 50, 200, 50, "Random", LIGHTGRAY);
    InputBox* source_node = new InputBox(1000, 150, 200, 50);

    Button* next_button = new Button(1000, 250, 200, 50, "Next Step", LIGHTGRAY);
    Tool* tool = new Tool;

    int sourceNode = -1, old_sourceNode = -1;
    int isNextStep = 0;
    vector<state> currentHistory;

    float sTime, interval;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Visual->moveNode();
        Visual->drawNode(Graph);
        
        
        random_button->draw();
        if (random_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
        {
            Graph->randomGraph(tool->random(2,7));
            Visual->placeNode(Graph);
        }

        
        source_node->Draw();
        source_node->checkPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
        
        sourceNode = source_node->GetValue();
        source_node->Update();
        
        
        if (sourceNode!=-1 && sourceNode<Graph->size && sourceNode!=old_sourceNode)
        { 
            Graph->implement(sourceNode);
            old_sourceNode = sourceNode;
            Visual->node[sourceNode].d = 0;
        }

        
        next_button->draw();
        if (next_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
        {
            if (Graph->history.size())
            {
                state s = Graph->history[0];
                currentHistory.clear();
                currentHistory.push_back(s);

                Graph->history.erase(Graph->history.begin());
                while(Graph->history.size() && Graph->history[0].node==s.node)
                {
                    currentHistory.push_back(Graph->history[0]);
                    Graph->history.erase(Graph->history.begin());
                }
                isNextStep = currentHistory.size();
                interval = GetTime();
            }
        }

        if (isNextStep)
        {
            state s = currentHistory[currentHistory.size()-isNextStep];
            
            if (GetTime()-interval > 1.0)
            {

                Vector2 posU = Visual->node[s.node].pos;
                Vector2 posV = Visual->node[s.updatedNode].pos;

                DrawLineEx(posU, posV, 2.0f, GREEN);
                DrawCircle(posU.x, posU.y, Visual->radius, WHITE);
                DrawCircle(posV.x, posV.y, Visual->radius, WHITE);
                DrawCircle(posU.x, posU.y, Visual->radius, {120,120,120,225});
                DrawCircle(posV.x, posV.y, Visual->radius, {120,120,120,225});

                string s1 = tool->convert(s.node);
                const char* c1 = s1.c_str();
                DrawText(c1, posU.x-7, posU.y-14, 30, BLACK);

                s1 = tool->convert(s.updatedNode);
                const char* c2 = s1.c_str();
                DrawText(c2, posV.x-7, posV.y-14, 30, BLACK);

                tool->drawArrow(posU, posV, Visual->radius, GREEN);
                
                
            }

            if (GetTime()-interval > 2.0)
            {
                Vector2 pos = Visual->node[s.updatedNode].pos;
                DrawCircle(pos.x, pos.y, Visual->radius, ORANGE);
                //DrawCircle(pos.x, pos.y, Visual->radius, {120,120,120,225});
                //DrawCircleLines(pos.x, pos.y, radius, BLACK);

                string s1 = tool->convert(s.updatedNode);
                const char* c1 = s1.c_str();
                DrawText(c1, pos.x-7, pos.y-14, 30, BLACK);

               
            }

            if (GetTime()-interval > 2.5)
            {
                Visual->node[s.updatedNode].d = s.newDis;
            }

            if (GetTime()-interval > 3.0)
            {
                isNextStep--;
                interval = GetTime();
            }


        }






        
        EndDrawing();
    }
    
    CloseWindow();
}