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

    int c;
    bool isNextStep = 0;
    vector<state> currentHistory;

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
        c = source_node->GetValue();
        source_node->Update();
        
        
        if (c!=-1 && c<Graph->size)
        { 
            Graph->implement(c);
            c = -1;
        }

        
        next_button->draw();
        if (next_button->isPressed(GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
        {
            if (Graph->history.size())
            {
                state s = Graph->history[0];
                currentHistory.push_back(s);

                Graph->history.erase(Graph->history.begin());
                while(Graph->history.size() && Graph->history[0].node==s.node)
                {
                    currentHistory.push_back(Graph->history[0]);
                    Graph->history.erase(Graph->history.begin());
                }
                isNextStep = 1;
            }
        }

        if (isNextStep)
        {
            cout<<1;
        }






        
        EndDrawing();
    }
    
    CloseWindow();
}