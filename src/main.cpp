#include "raylib.h"

int main() {
    InitWindow(800, 600, "Universal CMake Raylib Setup");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("It works on Visual Studio AND VS Code!", 150, 280, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}