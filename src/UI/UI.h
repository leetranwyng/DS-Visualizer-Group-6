#pragma once
#include "raylib.h"
#include <vector>
#include <string>
using namespace std;
struct TreeNode;

class Button {
public:
    Rectangle rect;
    Button(float x, float y, float width, float height, string text, Color color);
    void draw();
    bool isPressed(Vector2 mousePos, bool mousePressed);
private:
    string text;
    Color color;
};

class InputBox {
public:
    Rectangle rect;
    char text[4];
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

void DrawLabel(float x, float y, string text, Color color = DARKGRAY);
void DrawFlatButton(Rectangle rect, string text, Color bgColor, bool leftAlign = true);

struct NodeShape {
    TreeNode* node;
    Vector2 current; //current position
    Vector2 target; //final position
    float radius;
    Color color;
};

struct EdgeShape {
    TreeNode* parent;
    TreeNode* child;
    Vector2 start;
    Vector2 end;
    Color color;
};