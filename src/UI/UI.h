#pragma once
#include "raylib.h"
#include <string>
using namespace std;

struct Button {
    Rectangle rect;
    string text;
    Color color;

    Button(float x, float y, float width, float height, string txt, Color col)
        : rect{ x, y, width, height }, text(txt), color(col) {
    }

    void draw(bool haveBorder = true, bool isFlat = false, bool leftAlign = false);
    bool isPressed(Vector2 mousePos, bool mousePressed);
};

struct InputBox {
    Rectangle rect;
    char text[64];
    int letterCount;
    bool boxPressed;
    Color boxColor;
    Color textColor;

    InputBox(float x, float y, float width, float height, Color txtColor, Color bgColor);

    void Update();
    void Draw();
    void Clear();
    int GetValue();
    void checkPressed(Vector2 mousePos, bool mousePressed);
};

struct Slider {
    Rectangle bounds;
    float minValue;
    float maxValue;
    float currentValue;
    bool isDragging;

    Slider(float x, float y, float width, float height, float minVal, float maxVal, float currentVal);

    void Update(Vector2 mousePos, bool mouseDown);
    void Draw();
    float GetValue() const;
};

string getResourcesPath(const string& filename);

void DrawLabel(float x, float y, string text, Color color = DARKGRAY);
void DrawFlatButton(Rectangle rect, string text, Color bgColor, bool leftAlign = false);

void DrawModernPanel(Rectangle rect, Color fill, Color border = Color{ 210, 218, 230, 255 }, float shadowAlpha = 0.10f);
void DrawModernTitle(float x, float y, const string& text, Color color = Color{ 40, 48, 64, 255 });
void DrawModernSubtitle(float x, float y, const string& text, Color color = Color{ 120, 130, 150, 255 });