#include "UI/UI.h"
#include "UI/FontManager.h"
#include "structures/hash_table/HashTableRenderer.h"
#include "structures/graph/DijkstraRenderer.h"
#include "structures/tree/AVLRenderer.h"
#include "structures/linked_list/LinkedListRenderer.h"

enum class MenuScene {
    Home,
    Select,
    About
};

static const Color kMenuBg = Color{ 245, 245, 245, 255 };
static const Color kPinkAccent = Color{ 236, 120, 171, 255 };
static const Color kCyanAccent = Color{ 44, 201, 197, 255 };
static const Color kOrangeAccent = Color{ 240, 180, 20, 255 };

static Color MixColor(Color a, Color b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    Color out;
    out.r = (unsigned char)(a.r + (b.r - a.r) * t);
    out.g = (unsigned char)(a.g + (b.g - a.g) * t);
    out.b = (unsigned char)(a.b + (b.b - a.b) * t);
    out.a = (unsigned char)(a.a + (b.a - a.a) * t);
    return out;
}

static void DrawCenteredUI(const std::string& text, float y, float fontSize, Color color = BLACK) {
    Vector2 sz = MeasureUIFont(text, fontSize);
    DrawUIFont(text, (GetScreenWidth() - sz.x) / 2.0f, y, fontSize, color);
}

static void DrawMenuCard(const Button& btn, Color accent, float fontSize) {
    bool hovered = CheckCollisionPointRec(GetMousePosition(), btn.rect);

    Rectangle shadow = btn.rect;
    shadow.y += hovered ? 8.0f : 12.0f;

    Rectangle body = btn.rect;
    if (hovered) {
        body.y -= 5.0f;
    }

    DrawRectangleRounded(shadow, 0.22f, 24, accent);

    Color fill = hovered ? MixColor(WHITE, accent, 0.18f) : WHITE;
    DrawRectangleRounded(body, 0.22f, 24, fill);
    DrawRectangleRoundedLinesEx(body, 0.22f, 24, 3.0f, BLACK);

    Vector2 textSize = MeasureUIFont(btn.text, fontSize);
    DrawUIFont(
        btn.text,
        body.x + (body.width - textSize.x) / 2.0f,
        body.y + (body.height - textSize.y) / 2.0f - 2.0f,
        fontSize,
        BLACK
    );
}

int main() {
    bool keepRunning = true;

    while (keepRunning) {
        const int menuWidth = 1152;
        const int menuHeight = 640;

        InitWindow(menuWidth, menuHeight, "Data Visualizer");
        SetTargetFPS(60);
        LoadGlobalFonts();

        MenuScene scene = MenuScene::Home;
        int choice = 0;

        Button btnStart(404, 185, 344, 96, "Start", WHITE);
        Button btnAbout(404, 325, 344, 96, "About", WHITE);
        Button btnQuit(404, 465, 344, 96, "Quit", WHITE);

        Button btnBackSelect(36, 28, 176, 48, "Back", WHITE);
        Button btnBackAbout(36, 28, 176, 48, "Back", WHITE);

        Button btnHash(120, 210, 450, 96, "Hash Table", WHITE);
        Button btnDijkstra(626, 210, 450, 96, "Dijkstra's Algorithm", WHITE);
        Button btnAVL(120, 388, 450, 96, "AVL Tree", WHITE);
        Button btnDLL(626, 388, 450, 96, "Linked List", WHITE);

        while (!WindowShouldClose()) {
            Vector2 mousePos = GetMousePosition();
            bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            if (scene == MenuScene::Home) {
                if (btnStart.isPressed(mousePos, mousePressed)) {
                    scene = MenuScene::Select;
                }
                else if (btnAbout.isPressed(mousePos, mousePressed)) {
                    scene = MenuScene::About;
                }
                else if (btnQuit.isPressed(mousePos, mousePressed)) {
                    choice = 0;
                    break;
                }
            }
            else if (scene == MenuScene::Select) {
                if (btnBackSelect.isPressed(mousePos, mousePressed)) {
                    scene = MenuScene::Home;
                }
                else if (btnHash.isPressed(mousePos, mousePressed)) {
                    choice = 1;
                    break;
                }
                else if (btnDijkstra.isPressed(mousePos, mousePressed)) {
                    choice = 2;
                    break;
                }
                else if (btnAVL.isPressed(mousePos, mousePressed)) {
                    choice = 3;
                    break;
                }
                else if (btnDLL.isPressed(mousePos, mousePressed)) {
                    choice = 4;
                    break;
                }
            }
            else if (scene == MenuScene::About) {
                if (btnBackAbout.isPressed(mousePos, mousePressed)) {
                    scene = MenuScene::Home;
                }
            }

            BeginDrawing();
            ClearBackground(kMenuBg);

            if (scene == MenuScene::Home) {
                DrawCenteredUI("Data Visualizer", 54, 74);
                DrawMenuCard(btnStart, kPinkAccent, 34);
                DrawMenuCard(btnAbout, kPinkAccent, 34);
                DrawMenuCard(btnQuit, kPinkAccent, 34);
            }
            else if (scene == MenuScene::Select) {
                DrawMenuCard(btnBackSelect, kOrangeAccent, 24);
                DrawCenteredUI("Select a data structure", 92, 66);

                DrawMenuCard(btnHash, kCyanAccent, 34);
                DrawMenuCard(btnDijkstra, kCyanAccent, 30);
                DrawMenuCard(btnAVL, kCyanAccent, 34);
                DrawMenuCard(btnDLL, kCyanAccent, 34);
            }
            else if (scene == MenuScene::About) {
                DrawMenuCard(btnBackAbout, kOrangeAccent, 24);

                DrawCenteredUI("About", 58, 56);
                DrawCenteredUI("This is the final project made by group 6 in CS163 course.", 128, 31);
                DrawCenteredUI("The course is for APCS freshmen in HCMUS.", 176, 31);

                DrawCenteredUI("Group member:", 270, 31);
                DrawCenteredUI("25125065 - Bui Viet Thanh", 316, 30);
                DrawCenteredUI("25125039 - Nguyen Le Tran", 362, 30);
                DrawCenteredUI("25125071 - Le Quynh Anh", 408, 30);
                DrawCenteredUI("25125078 - Dinh Nguyen Hong Anh", 454, 30);

                DrawCenteredUI("Enjoy!", 540, 31);
            }

            EndDrawing();
        }

        UnloadGlobalFonts();
        CloseWindow();

        if (choice == 0) {
            keepRunning = false;
        }
        else if (choice == 1) {
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

    return 0;
}