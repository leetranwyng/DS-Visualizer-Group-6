#include "UI/UI.h"
#include "structures/hash_table/HashTable.h"
#include "structures/hash_table/HashTableRenderer.h"
#include "structures/graph/Dijkstra.h"
#include "structures/graph/DijkstraRenderer.h"
#include "structures/tree/AVL.h"
#include "structures/tree/AVLRenderer.h"
#include "structures/linked_list/LinkedList.h"
#include "structures/linked_list/LinkedListRenderer.h"

int main() {
   bool keepRunning = true;
   while (keepRunning) {
       const int menuWidth = 600;
       const int menuHeight = 500;
       InitWindow(menuWidth, menuHeight, "Data Structure Visualizer - Main Menu");
       SetTargetFPS(60);

       float btnWidth = 350;
       float btnHeight = 60;
       float startX = (menuWidth - btnWidth) / 2.0f;
       float startY = 120;
       float spacing = 80;

       Button btnHash(startX, startY, btnWidth, btnHeight, "1. Hash Table", LIGHTGRAY);
       Button btnDijkstra(startX, startY + spacing, btnWidth, btnHeight, "2. Dijkstra's Algorithm", LIGHTGRAY);
       Button btnAVL(startX, startY + spacing * 2, btnWidth, btnHeight, "3. AVL Tree", LIGHTGRAY);
       Button btnDLL(startX, startY + spacing * 3, btnWidth, btnHeight, "4. Linked List", LIGHTGRAY);

       int choice = 0;

       while (!WindowShouldClose()) {
           Vector2 mousePos = GetMousePosition();
           bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

           if (btnHash.isPressed(mousePos, mousePressed)) { choice = 1; break; }
           if (btnDijkstra.isPressed(mousePos, mousePressed)) { choice = 2; break; }
           if (btnAVL.isPressed(mousePos, mousePressed)) { choice = 3; break; }
           if (btnDLL.isPressed(mousePos, mousePressed)) { choice = 4; break; }

           BeginDrawing();
           ClearBackground(RAYWHITE);

           btnHash.draw();
           btnDijkstra.draw();
           btnAVL.draw();
           btnDLL.draw();

           EndDrawing();
       }

       if (choice == 0) {
           keepRunning = false;
       }

       CloseWindow();

       if (choice == 1) {
           RenderHashTable();
       }
       else if (choice == 2) {
           RenderDijkstra();
       }
       else if (choice == 3) {
           RenderAVL();
       }
       else if (choice == 4) {
           RenderLinkedList();
       }
   }
}
