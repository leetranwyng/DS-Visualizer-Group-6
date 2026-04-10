#pragma once
#include "raylib.h"
#include <string>
using namespace std;

class Button {
public:
    Rectangle rect;
    string text;
    Color color;
    Button(float x, float y, float width, float height, string text, Color color);
    void draw(bool haveBorder = true, bool isFlat = false, bool leftAlign = false);

    bool isPressed(Vector2 mousePos, bool mousePressed);
};

class InputBox {
public:
    Rectangle rect;
    char text[6];
    bool boxPressed;
    Color boxColor;
    Color textColor;

    InputBox(float x, float y, float width, float height, Color bg = LIGHTGRAY, Color txt = MAROON);
    void Update();
    void Draw();
    int GetValue();
    void Clear();
    void checkPressed(Vector2 mousePos, bool mousePressed);
private:
    int letterCount;
    int framesCounter;
};

class Slider {
public:
    Rectangle bounds;
    float minValue;
    float maxValue;
    float currentValue;
    bool isDragging;

    Slider(float x, float y, float width, float height, float minVal, float maxVal, float initialVal);
    void Update(Vector2 mousePos, bool mouseDown);
    void Draw();
    float GetValue();
};
