#include "UI.h"

// BUTTON
Button::Button(float x, float y, float width, float height, string text, Color color) {
    this->rect = { x, y, width, height };
    this->text = text;
    this->color = color;
}

void Button::draw() {
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 2, BLACK);

    int textWidth = MeasureText(text.c_str(), 25);
    DrawText(text.c_str(), rect.x + (rect.width - textWidth) / 2, rect.y + (rect.height - 20) / 2, 25, BLACK);
}

bool Button::isPressed(Vector2 mousePos, bool mousePressed) {
    if (CheckCollisionPointRec(mousePos, rect) && mousePressed) return true;
    return false;
}

// INPUT TEXT BOX
InputBox::InputBox(float x, float y, float width, float height, Color bg, Color txt) {
    rect = { x, y, width, height };
    text[0] = '\0';
    letterCount = 0;
    framesCounter = 0;
    boxPressed = false;
    this->boxColor = bg;
    this->textColor = txt;
}

void InputBox::Update() {
    if (boxPressed) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (letterCount < 5)) {
                text[letterCount] = (char)key;
                text[letterCount + 1] = '\0';
                letterCount++;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            text[letterCount] = '\0';
        }
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    if (boxPressed) framesCounter++;
    else framesCounter = 0;
}

void InputBox::Draw() {
    DrawRectangleRec(rect, boxColor);
    if (boxPressed) DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, RED);
    else DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, DARKGRAY);

    DrawText(text, (int)rect.x + 5, (int)rect.y + 10, 21, textColor);

    if (boxPressed && letterCount < 5) {
        if (((framesCounter / 20) % 2) == 0) {
            DrawText("_", (int)rect.x + 8 + MeasureText(text, 20), (int)rect.y + 12, 20, textColor);
        }
    }
}

int InputBox::GetValue() {
    if (letterCount > 0) return std::stoi(text);
    return -1;
}

void InputBox::Clear() {
    text[0] = '\0';
    letterCount = 0;
}

void InputBox::checkPressed(Vector2 mousePos, bool mousePressed) {
    if (mousePressed) {
        if (CheckCollisionPointRec(mousePos, rect)) boxPressed = true;
        else boxPressed = false;
    }
}

// UI HELPER
void DrawLabel(float x, float y, string text, Color color) {
    DrawText(text.c_str(), (int)x, (int)y, 20, color);
}

void DrawFlatButton(Rectangle rect, string text, Color bgColor, bool leftAlign) {
    DrawRectangleRec(rect, bgColor);
    DrawLine((int)rect.x, (int)(rect.y + rect.height), (int)(rect.x + rect.width), (int)(rect.y + rect.height), { 255, 255, 255, 40 });
    int textY = (int)rect.y + ((int)rect.height - 20) / 2;
    if (leftAlign) {
        DrawText(text.c_str(), (int)rect.x + 15, textY, 20, RAYWHITE);
    }
    else {
        int textWidth = MeasureText(text.c_str(), 20);
        DrawText(text.c_str(), (int)rect.x + ((int)rect.width - textWidth) / 2, textY, 20, RAYWHITE);
    }
}

// SLIDER
Slider::Slider(float x, float y, float width, float height, float minVal, float maxVal, float initialVal) {
    bounds = { x, y, width, height };
    minValue = minVal;
    maxValue = maxVal;
    currentValue = initialVal;
    isDragging = false;
}

void Slider::Update(Vector2 mousePos, bool mouseDown) {
    if (mouseDown) {
        Rectangle grabArea = { bounds.x, bounds.y - 10, bounds.width, bounds.height + 20 };
        if (CheckCollisionPointRec(mousePos, grabArea)) {
            isDragging = true;
        }
    }
    else {
        isDragging = false;
    }

    if (isDragging) {
        float percent = (mousePos.x - bounds.x) / bounds.width;
        if (percent < 0.0f) percent = 0.0f;
        if (percent > 1.0f) percent = 1.0f;

        currentValue = minValue + percent * (maxValue - minValue);
    }
}

void Slider::Draw() {
    DrawRectangle(bounds.x, bounds.y + bounds.height / 2 - 2, bounds.width, 4, LIGHTGRAY);

    float percent = (currentValue - minValue) / (maxValue - minValue);
    DrawRectangle(bounds.x, bounds.y + bounds.height / 2 - 2, bounds.width * percent, 4, DARKGRAY);

    float knobX = bounds.x + bounds.width * percent;
    Color knobColor;
    if (isDragging) knobColor = RED;
    else knobColor = { 210, 80, 50, 255 }; 
    DrawRectangle(knobX - 5, bounds.y, 10, bounds.height, knobColor);
}

float Slider::GetValue() {
    return currentValue;
}