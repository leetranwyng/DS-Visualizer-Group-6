#include "UI.h"
#include "FontManager.h"
#include <filesystem>
#include <cstring>
#include <cstdlib>

using namespace std;

InputBox::InputBox(float x, float y, float width, float height, Color txtColor, Color bgColor) {
    rect = { x, y, width, height };
    text[0] = '\0';
    letterCount = 0;
    boxPressed = false;
    boxColor = bgColor;
    textColor = txtColor;
}

void InputBox::checkPressed(Vector2 mousePos, bool mousePressed) {
    if (mousePressed) {
        boxPressed = CheckCollisionPointRec(mousePos, rect);
    }
}

void InputBox::Update() {
    if (!boxPressed) return;

    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && letterCount < 63) {
            text[letterCount] = (char)key;
            letterCount++;
            text[letterCount] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
        letterCount--;
        text[letterCount] = '\0';
    }
}

void InputBox::Draw() {
    DrawModernPanel(rect, WHITE, boxPressed ? Color{ 59, 130, 246, 255 } : Color{ 205, 213, 225, 255 });

    if (strlen(text) > 0) {
        DrawUIFont(text, rect.x + 12, rect.y + 10, 21, textColor);
    }

    if (boxPressed && (((int)(GetTime() * 2.0)) % 2 == 0)) {
        Vector2 sz = MeasureUIFont(text, 21);
        DrawRectangle((int)(rect.x + 12 + sz.x + 2), (int)rect.y + 12, 2, 18, Color{ 59, 130, 246, 255 });
    }
}

void InputBox::Clear() {
    text[0] = '\0';
    letterCount = 0;
}

int InputBox::GetValue() {
    if (letterCount == 0) return -1;
    return atoi(text);
}

Slider::Slider(float x, float y, float width, float height, float minVal, float maxVal, float currentVal) {
    bounds = { x, y, width, height };
    minValue = minVal;
    maxValue = maxVal;
    currentValue = currentVal;
    isDragging = false;
}

void Slider::Update(Vector2 mousePos, bool mouseDown) {
    if (mouseDown && CheckCollisionPointRec(mousePos, bounds)) {
        isDragging = true;
    }
    if (!mouseDown) {
        isDragging = false;
    }

    if (isDragging) {
        float t = (mousePos.x - bounds.x) / bounds.width;
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        currentValue = minValue + t * (maxValue - minValue);
    }
}

void Slider::Draw() {
    DrawRectangleRounded(Rectangle{ bounds.x, bounds.y, bounds.width, 8 }, 0.9f, 8, Color{ 220, 226, 235, 255 });

    float t = (currentValue - minValue) / (maxValue - minValue);
    float fillW = bounds.width * t;
    DrawRectangleRounded(Rectangle{ bounds.x, bounds.y, fillW, 8 }, 0.9f, 8, Color{ 59, 130, 246, 255 });

    float knobX = bounds.x + fillW;
    DrawCircle((int)knobX, (int)(bounds.y + 4), 10, WHITE);
    DrawCircleLines((int)knobX, (int)(bounds.y + 4), 10, Color{ 59, 130, 246, 255 });
}

float Slider::GetValue() const {
    return currentValue;
}

bool Button::isPressed(Vector2 mousePos, bool mousePressed) {
    return mousePressed && CheckCollisionPointRec(mousePos, rect);
}

void DrawModernPanel(Rectangle rect, Color fill, Color border, float shadowAlpha) {
    Rectangle shadow = rect;
    shadow.x += 3;
    shadow.y += 3;
    DrawRectangleRounded(shadow, 0.08f, 8, Fade(BLACK, shadowAlpha));
    DrawRectangleRounded(rect, 0.08f, 8, fill);
    DrawRectangleRoundedLinesEx(rect, 0.08f, 8, 1.3f, border);
}

void Button::draw(bool haveBorder, bool isFlat, bool leftAlign) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rect);

    Rectangle r = rect;
    if (hovered) r.y -= 1;

    Color fill = color;
    Color border = haveBorder ? BLACK : Color{ 205, 213, 225, 255 };
    Color textColor = BLACK;

    if (isFlat) {
        fill = color;
        textColor = RAYWHITE;
        border = Fade(WHITE, 0.12f);
    }
    else {
        fill = hovered ? Color{ 230, 234, 240, 255 } : color;
        border = Color{ 205, 213, 225, 255 };
        textColor = Color{ 30, 41, 59, 255 };
    }

    DrawModernPanel(r, fill, border, 0.08f);

    float fontSize = 20;
    Vector2 sz = MeasureUIFont(text, fontSize);

    float textX = leftAlign ? (r.x + 15) : (r.x + (r.width - sz.x) / 2.0f);
    float textY = r.y + (r.height - sz.y) / 2.0f;

    DrawUIFont(text, textX, textY, fontSize, textColor);
}

void DrawLabel(float x, float y, string text, Color color) {
    DrawUIFont(text, x, y, 20, color);
}

void DrawFlatButton(Rectangle rect, string text, Color bgColor, bool leftAlign) {
    DrawModernPanel(rect, bgColor, Fade(WHITE, 0.12f), 0.08f);

    Vector2 sz = MeasureUIFont(text, 20);
    float textX = leftAlign ? (rect.x + 15) : (rect.x + (rect.width - sz.x) / 2.0f);
    float textY = rect.y + (rect.height - sz.y) / 2.0f;

    DrawUIFont(text, textX, textY, 20, RAYWHITE);
}

void DrawModernTitle(float x, float y, const string& text, Color color) {
    DrawUIFont(text, x, y, 34, color);
}

void DrawModernSubtitle(float x, float y, const string& text, Color color) {
    DrawUIFont(text, x, y, 18, color);
}

string getResourcesPath(const string& filename) {
    namespace fs = std::filesystem;

    fs::path current = fs::current_path();
    for (int i = 0; i < 6; i++) {
        fs::path candidate = current / "resources" / filename;
        if (fs::exists(candidate)) {
            return candidate.string();
        }
        if (current.has_parent_path()) current = current.parent_path();
    }

    return (fs::current_path() / "resources" / filename).string();
}