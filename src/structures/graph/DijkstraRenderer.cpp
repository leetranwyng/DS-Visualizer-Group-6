#include "Dijkstra.h"
#include "DijkstraRenderer.h"

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
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Visual->moveNode();
        Visual->drawNode(Graph);

        EndDrawing();
    }
    
    CloseWindow();
}