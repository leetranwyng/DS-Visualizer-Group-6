#pragma once
#include "raylib.h"
#include <string>

extern Font gUIFont;
extern Font gCodeFont;

void LoadGlobalFonts();
void UnloadGlobalFonts();

Vector2 MeasureUIFont(const std::string& text, float fontSize, float spacing = 0.0f);
Vector2 MeasureCodeFont(const std::string& text, float fontSize, float spacing = 0.0f);

void DrawUIFont(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.0f);
void DrawCodeFont(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.0f);