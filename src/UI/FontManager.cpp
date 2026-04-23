#include "FontManager.h"
#include "UI.h"

#include <iostream>
#include <string>

Font gUIFont = {};
Font gCodeFont = {};

static bool gUIFontLoaded = false;
static bool gCodeFontLoaded = false;

static Font TryLoadFont(const std::string& path, int size) {
    Font f = LoadFontEx(path.c_str(), size, 0, 0);
    if (f.texture.id != 0) {
        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
    }
    return f;
}

void LoadGlobalFonts() {
    if (gUIFont.texture.id != 0 || gCodeFont.texture.id != 0) return;

    std::string uiPath = getResourcesPath("segoeui.ttf");
    std::string codePath = getResourcesPath("consola.ttf");

    std::cout << "[FontManager] UI font path   : " << uiPath << '\n';
    std::cout << "[FontManager] Code font path : " << codePath << '\n';

    gUIFont = TryLoadFont(uiPath, 64);
    if (gUIFont.texture.id != 0) {
        gUIFontLoaded = true;
        std::cout << "[FontManager] Loaded UI font successfully\n";
    }
    else {
        gUIFont = GetFontDefault();
        gUIFontLoaded = false;
        std::cout << "[FontManager] Failed to load UI font, fallback to default\n";
    }

    gCodeFont = TryLoadFont(codePath, 64);
    if (gCodeFont.texture.id != 0) {
        gCodeFontLoaded = true;
        std::cout << "[FontManager] Loaded code font successfully\n";
    }
    else {
        gCodeFont = gUIFont;
        gCodeFontLoaded = false;
        std::cout << "[FontManager] Failed to load code font, fallback to UI font\n";
    }
}

void UnloadGlobalFonts() {
    if (gUIFontLoaded && gUIFont.texture.id != 0) {
        UnloadFont(gUIFont);
    }

    if (gCodeFontLoaded && gCodeFont.texture.id != 0 && gCodeFont.texture.id != gUIFont.texture.id) {
        UnloadFont(gCodeFont);
    }

    gUIFont = {};
    gCodeFont = {};
    gUIFontLoaded = false;
    gCodeFontLoaded = false;
}

Vector2 MeasureUIFont(const std::string& text, float fontSize, float spacing) {
    return MeasureTextEx(gUIFont, text.c_str(), fontSize, spacing);
}

Vector2 MeasureCodeFont(const std::string& text, float fontSize, float spacing) {
    return MeasureTextEx(gCodeFont, text.c_str(), fontSize, spacing);
}

void DrawUIFont(const std::string& text, float x, float y, float fontSize, Color color, float spacing) {
    DrawTextEx(gUIFont, text.c_str(), Vector2{ x, y }, fontSize, spacing, color);
}

void DrawCodeFont(const std::string& text, float x, float y, float fontSize, Color color, float spacing) {
    DrawTextEx(gCodeFont, text.c_str(), Vector2{ x, y }, fontSize, spacing, color);
}