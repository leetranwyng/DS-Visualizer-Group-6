#include "LinkedListRenderer.h"
#include "LinkedList.h"
#include "../../UI/UI.h"
#include "raylib.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>

enum AnimationType {
    ANIM_NONE,
    ANIM_SEARCH,
    ANIM_INSERT_FRONT,
    ANIM_INSERT_BACK,
    ANIM_DELETE,
    ANIM_UPDATE
};

struct Theme {
    Color bg;
    Color panel;
    Color panelAlt;
    Color border;
    Color shadow;

    Color text;
    Color textSoft;
    Color textMuted;

    Color primary;
    Color primarySoft;
    Color success;
    Color successSoft;
    Color danger;
    Color dangerSoft;
    Color warning;
    Color warningSoft;
    Color cyan;
    Color cyanSoft;
    Color violet;
    Color violetSoft;
};

struct ActionButton {
    Rectangle rect;
    std::string label;
    Color fill;
    Color border;
};

struct OperationState {
    AnimationType type = ANIM_NONE;

    bool playing = false;
    int step = 0;
    int foundIndex = -1;
    int targetValue = -1;
    int newValue = -1;
    float timer = 0.0f;

    bool flashActive = false;
    int flashIndex = -1;
    float flashTimer = 0.0f;
    float flashDuration = 0.85f;

    std::vector<std::string> pseudo;
    int highlightLine = -1;
};

static Font gUiFont;
static Font gCodeFont;
static bool gUnloadUiFont = false;
static bool gUnloadCodeFont = false;

static Vector2 MeasureTextFont(Font font, const std::string& text, float fontSize, float spacing = 0.0f) {
    return MeasureTextEx(font, text.c_str(), fontSize, spacing);
}

static void DrawTextFont(Font font, const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.0f) {
    DrawTextEx(font, text.c_str(), Vector2{ x, y }, fontSize, spacing, color);
}

static Vector2 MeasureUI(const std::string& text, float fontSize, float spacing = 0.0f) {
    return MeasureTextFont(gUiFont, text, fontSize, spacing);
}

static void DrawUI(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.0f) {
    DrawTextFont(gUiFont, text, x, y, fontSize, color, spacing);
}

static Vector2 MeasureCode(const std::string& text, float fontSize, float spacing = 0.0f) {
    return MeasureTextFont(gCodeFont, text, fontSize, spacing);
}

static void DrawCode(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.0f) {
    DrawTextFont(gCodeFont, text, x, y, fontSize, color, spacing);
}

static Theme MakeTheme() {
    Theme t;
    t.bg = Color{ 244, 247, 251, 255 };
    t.panel = Color{ 255, 255, 255, 255 };
    t.panelAlt = Color{ 250, 252, 255, 255 };
    t.border = Color{ 226, 232, 240, 255 };
    t.shadow = Color{ 15, 23, 42, 255 };

    t.text = Color{ 15, 23, 42, 255 };
    t.textSoft = Color{ 71, 85, 105, 255 };
    t.textMuted = Color{ 148, 163, 184, 255 };

    t.primary = Color{ 59, 130, 246, 255 };
    t.primarySoft = Color{ 219, 234, 254, 255 };

    t.success = Color{ 34, 197, 94, 255 };
    t.successSoft = Color{ 220, 252, 231, 255 };

    t.danger = Color{ 239, 68, 68, 255 };
    t.dangerSoft = Color{ 254, 226, 226, 255 };

    t.warning = Color{ 245, 158, 11, 255 };
    t.warningSoft = Color{ 254, 243, 199, 255 };

    t.cyan = Color{ 6, 182, 212, 255 };
    t.cyanSoft = Color{ 207, 250, 254, 255 };

    t.violet = Color{ 139, 92, 246, 255 };
    t.violetSoft = Color{ 237, 233, 254, 255 };

    return t;
}

static float Clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

static float EaseOutCubic(float x) {
    x = Clamp01(x);
    float inv = 1.0f - x;
    return 1.0f - inv * inv * inv;
}

static Color MixColor(Color a, Color b, float t) {
    t = Clamp01(t);

    Color out;
    out.r = (unsigned char)(a.r + (b.r - a.r) * t);
    out.g = (unsigned char)(a.g + (b.g - a.g) * t);
    out.b = (unsigned char)(a.b + (b.b - a.b) * t);
    out.a = (unsigned char)(a.a + (b.a - a.a) * t);
    return out;
}

static int GetListSize(const LinkedList& list) {
    return (int)list.toVector().size();
}

static int FindFirstIndex(const std::vector<int>& values, int target) {
    for (int i = 0; i < (int)values.size(); i++) {
        if (values[i] == target) return i;
    }
    return -1;
}

static std::vector<std::string> GetPseudoCode(AnimationType type) {
    if (type == ANIM_SEARCH) {
        return {
            "cur = head",
            "while cur != null",
            "if cur.data == x",
            "return FOUND",
            "cur = cur.next",
            "return NOT FOUND"
        };
    }
    if (type == ANIM_INSERT_FRONT) {
        return {
            "newNode = Node(x)",
            "newNode.next = head",
            "if head != null: head.prev = newNode",
            "head = newNode"
        };
    }
    if (type == ANIM_INSERT_BACK) {
        return {
            "newNode = Node(x)",
            "tail.next = newNode",
            "newNode.prev = tail",
            "tail = newNode"
        };
    }
    if (type == ANIM_DELETE) {
        return {
            "cur = head",
            "find first node with value x",
            "unlink cur.prev and cur.next",
            "delete cur"
        };
    }
    if (type == ANIM_UPDATE) {
        return {
            "cur = search(oldValue)",
            "if cur != null",
            "cur.data = newValue"
        };
    }
    return {};
}

static void DrawSoftShadow(Rectangle rect, float roundness, const Theme& theme) {
    for (int i = 4; i >= 1; --i) {
        Rectangle s = rect;
        s.x += (float)i;
        s.y += (float)i;
        DrawRectangleRounded(s, roundness, 8, Fade(theme.shadow, 0.012f * i));
    }
}

static void DrawPanel(Rectangle rect, float roundness, const Theme& theme, Color fill) {
    DrawSoftShadow(rect, roundness, theme);
    DrawRectangleRounded(rect, roundness, 8, fill);
    DrawRectangleRoundedLinesEx(rect, roundness, 8, 1.4f, theme.border);
}

static bool IsButtonHovered(const ActionButton& btn, Vector2 mousePos, bool enabled = true) {
    if (!enabled) return false;
    return CheckCollisionPointRec(mousePos, btn.rect);
}

static bool IsButtonPressed(const ActionButton& btn, Vector2 mousePos, bool mousePressed, bool enabled = true) {
    return enabled && mousePressed && CheckCollisionPointRec(mousePos, btn.rect);
}

static void DrawButton(const ActionButton& btn, Vector2 mousePos, const Theme& theme, bool enabled = true) {
    bool hovered = IsButtonHovered(btn, mousePos, enabled);

    Rectangle r = btn.rect;
    if (hovered) r.y -= 1.0f;

    Color fill = btn.fill;
    Color border = btn.border;
    Color textColor = theme.text;

    if (!enabled) {
        fill = Color{ 246, 248, 250, 255 };
        border = theme.border;
        textColor = theme.textMuted;
    }
    else if (hovered) {
        fill = MixColor(btn.fill, WHITE, 0.08f);
    }

    DrawSoftShadow(r, 0.16f, theme);
    DrawRectangleRounded(r, 0.16f, 8, fill);
    DrawRectangleRoundedLinesEx(r, 0.16f, 8, 1.6f, border);

    float fontSize = (btn.label.size() >= 9) ? 18.0f : 19.0f;
    Vector2 sz = MeasureUI(btn.label, fontSize);
    float tx = r.x + (r.width - sz.x) / 2.0f;
    float ty = r.y + (r.height - sz.y) / 2.0f - 1.0f;
    DrawUI(btn.label, tx, ty, fontSize, textColor);
}

static void DrawMetricCard(float x, float y, float w, const std::string& title, const std::string& value, const Theme& theme) {
    Rectangle rect = { x, y, w, 58 };
    DrawPanel(rect, 0.18f, theme, theme.panel);

    DrawUI(title, x + 14, y + 9, 16, theme.textMuted);
    DrawUI(value, x + 14, y + 28, 22, theme.text);
}

static void DrawInputField(const InputBox& box, const std::string& label, const std::string& hint, const Theme& theme) {
    DrawUI(label, box.rect.x, box.rect.y - 24, 18, theme.textSoft);

    Rectangle r = box.rect;
    DrawSoftShadow(r, 0.14f, theme);
    DrawRectangleRounded(r, 0.14f, 8, WHITE);

    Color border = box.boxPressed ? theme.primary : theme.border;
    DrawRectangleRoundedLinesEx(r, 0.14f, 8, box.boxPressed ? 2.0f : 1.4f, border);

    if (std::strlen(box.text) > 0) {
        DrawUI(box.text, r.x + 14, r.y + 11, 22, theme.text);
    }
    else {
        DrawUI(hint, r.x + 14, r.y + 11, 20, theme.textMuted);
    }

    if (box.boxPressed && ((int)(GetTime() * 2.0) % 2 == 0)) {
        Vector2 sz = MeasureUI(box.text, 22);
        DrawRectangle((int)(r.x + 14 + sz.x + 2), (int)r.y + 13, 2, 18, theme.primary);
    }
}

static void DrawSliderMinimal(const Slider& slider, const Theme& theme) {
    DrawUI("Speed", slider.bounds.x, slider.bounds.y - 26, 18, theme.textSoft);

    float barX = slider.bounds.x;
    float barY = slider.bounds.y;
    float barW = slider.bounds.width;

    DrawRectangleRounded(Rectangle{ barX, barY, barW, 8 }, 0.8f, 8, Color{ 226, 232, 240, 255 });

    float percent = (slider.currentValue - slider.minValue) / (slider.maxValue - slider.minValue);
    percent = Clamp01(percent);

    DrawRectangleRounded(Rectangle{ barX, barY, barW * percent, 8 }, 0.8f, 8, theme.primary);

    float knobX = barX + barW * percent;
    DrawSoftShadow(Rectangle{ knobX - 9, barY - 6, 18, 20 }, 0.7f, theme);
    DrawCircle((int)knobX, (int)(barY + 4), 9, WHITE);
    DrawCircleLines((int)knobX, (int)(barY + 4), 9, theme.primary);

    DrawUI(TextFormat("x%.1f", slider.currentValue), barX + barW + 16, barY - 8, 18, theme.text);
}

static void DrawCanvasGrid(Rectangle panel) {
    int startX = (int)panel.x + 20;
    int endX = (int)(panel.x + panel.width - 20);
    int startY = (int)panel.y + 54;
    int endY = (int)(panel.y + panel.height - 20);

    for (int x = startX; x <= endX; x += 28) {
        DrawLine(x, startY, x, endY, Fade(Color{ 148, 163, 184, 255 }, 0.08f));
    }
    for (int y = startY; y <= endY; y += 28) {
        DrawLine(startX, y, endX, y, Fade(Color{ 148, 163, 184, 255 }, 0.08f));
    }
}

static void DrawNodeBox(int x, int y, int w, int h, int value, Color fillColor, bool isHead, bool isTail, const Theme& theme) {
    DrawSoftShadow(Rectangle{ (float)x, (float)y, (float)w, (float)h }, 0.16f, theme);
    DrawRectangleRounded(Rectangle{ (float)x, (float)y, (float)w, (float)h }, 0.16f, 8, fillColor);
    DrawRectangleRoundedLinesEx(Rectangle{ (float)x, (float)y, (float)w, (float)h }, 0.16f, 8, 1.6f, MixColor(fillColor, theme.text, 0.25f));

    std::string val = std::to_string(value);
    Vector2 sz = MeasureUI(val, 24);
    DrawUI(val, x + (w - sz.x) / 2.0f, y + 16, 24, theme.text);

    if (isHead) {
        DrawUI("HEAD", x + 14, y - 22, 13, theme.danger);
    }
    if (isTail) {
        DrawUI("TAIL", x + 18, y + h + 8, 13, theme.primary);
    }
}

static void DrawDoubleArrow(int x, int y, int nodeWidth, int spacing, const Theme& theme) {
    DrawLine(x + nodeWidth, y + 22, x + nodeWidth + spacing, y + 22, theme.textSoft);
    DrawTriangle(
        Vector2{ (float)(x + nodeWidth + spacing), (float)(y + 22) },
        Vector2{ (float)(x + nodeWidth + spacing - 10), (float)(y + 16) },
        Vector2{ (float)(x + nodeWidth + spacing - 10), (float)(y + 28) },
        theme.textSoft
    );

    DrawLine(x + nodeWidth + spacing, y + 36, x + nodeWidth, y + 36, theme.textMuted);
    DrawTriangle(
        Vector2{ (float)(x + nodeWidth), (float)(y + 36) },
        Vector2{ (float)(x + nodeWidth + 10), (float)(y + 30) },
        Vector2{ (float)(x + nodeWidth + 10), (float)(y + 42) },
        theme.textMuted
    );
}

static void DrawLinkedListMinimal(const LinkedList& list, Rectangle canvasPanel, const OperationState& op, const Theme& theme) {
    DrawPanel(canvasPanel, 0.06f, theme, theme.panelAlt);
    DrawUI("Linked List", canvasPanel.x + 22, canvasPanel.y + 18, 26, theme.text);

    DrawCanvasGrid(canvasPanel);

    std::vector<int> values = list.toVector();
    if (values.empty()) {
        DrawUI("List is empty", canvasPanel.x + 46, canvasPanel.y + 158, 30, theme.textMuted);
        return;
    }

    const int nodeWidth = 96;
    const int nodeHeight = 56;
    const int spacing = 42;
    const int startX = (int)canvasPanel.x + 38;
    const int startY = (int)canvasPanel.y + 118;
    const int maxX = (int)(canvasPanel.x + canvasPanel.width - 54 - nodeWidth);

    float flashProgress = 0.0f;
    if (op.flashActive && op.flashDuration > 0.0f) {
        flashProgress = 1.0f - Clamp01(op.flashTimer / op.flashDuration);
    }

    int x = startX;
    int y = startY;

    for (int i = 0; i < (int)values.size(); i++) {
        Color fill = theme.primarySoft;

        if (op.type == ANIM_SEARCH) {
            if (i < op.step) fill = theme.warningSoft;
            if (op.playing && i == op.step && op.step < (int)values.size()) fill = theme.cyanSoft;
            if (!op.playing && op.foundIndex != -1 && op.step > op.foundIndex && i == op.foundIndex) fill = theme.successSoft;
        }

        if (op.flashActive && i == op.flashIndex) {
            Color accent = theme.successSoft;
            if (op.type == ANIM_UPDATE) accent = theme.warningSoft;
            fill = MixColor(fill, accent, flashProgress);
        }

        float drawX = (float)x;
        float drawY = (float)y;

        if (op.flashActive && (op.type == ANIM_INSERT_FRONT || op.type == ANIM_INSERT_BACK) && i == op.flashIndex) {
            float p = EaseOutCubic(Clamp01(op.flashTimer / op.flashDuration));
            drawY = y - (1.0f - p) * 18.0f;
            if (op.type == ANIM_INSERT_FRONT) drawX = x - (1.0f - p) * 14.0f;
            else drawX = x + (1.0f - p) * 14.0f;
        }

        DrawNodeBox((int)drawX, (int)drawY, nodeWidth, nodeHeight, values[i], fill, i == 0, i == (int)values.size() - 1, theme);

        if (i != (int)values.size() - 1) {
            DrawDoubleArrow((int)drawX, (int)drawY, nodeWidth, spacing, theme);
        }

        x += nodeWidth + spacing;
        if (x > maxX) {
            x = startX;
            y += 108;
        }
    }
}

static void DrawCodePanelMinimal(const OperationState& op, const Theme& theme) {
    Rectangle panel = { 1028, 112, 448, 736 };
    DrawPanel(panel, 0.06f, theme, theme.panel);

    DrawUI("Pseudocode", 1054, 132, 28, theme.text);

    std::string opName = "Idle";
    if (op.type == ANIM_SEARCH) opName = "Search";
    else if (op.type == ANIM_INSERT_FRONT) opName = "Insert Front";
    else if (op.type == ANIM_INSERT_BACK) opName = "Insert Back";
    else if (op.type == ANIM_DELETE) opName = "Delete";
    else if (op.type == ANIM_UPDATE) opName = "Update";

    DrawUI(opName, 1056, 166, 18, theme.textMuted);

    if (op.pseudo.empty()) {
        DrawUI("No active operation", 1056, 230, 20, theme.textMuted);
        return;
    }

    int startX = 1056;
    int startY = 226;
    int lineHeight = 44;

    for (int i = 0; i < (int)op.pseudo.size(); i++) {
        Rectangle lineRect = { (float)(startX - 12), (float)(startY + i * lineHeight - 6), 360, 34 };

        if (i == op.highlightLine) {
            DrawRectangleRounded(lineRect, 0.2f, 8, theme.warningSoft);
            DrawRectangleRoundedLinesEx(lineRect, 0.2f, 8, 1.2f, theme.warning);
        }

        DrawCode(TextFormat("%d", i + 1), startX, startY + i * lineHeight, 19, theme.textMuted);
        DrawCode(op.pseudo[i], startX + 34, startY + i * lineHeight, 20, i == op.highlightLine ? theme.text : theme.textSoft);
    }
}

void RenderLinkedList() {
    const int screenWidth = 1500;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Doubly Linked List Visualization");
    SetTargetFPS(60);

    Font uiTry = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 64, 0, 0);
    if (uiTry.texture.id != 0) {
        gUiFont = uiTry;
        gUnloadUiFont = true;
    }
    else {
        gUiFont = GetFontDefault();
        gUnloadUiFont = false;
    }
    SetTextureFilter(gUiFont.texture, TEXTURE_FILTER_BILINEAR);

    Font codeTry = LoadFontEx("C:/Windows/Fonts/consola.ttf", 64, 0, 0);
    if (codeTry.texture.id != 0) {
        gCodeFont = codeTry;
        gUnloadCodeFont = true;
    }
    else {
        gCodeFont = gUiFont;
        gUnloadCodeFont = false;
    }
    SetTextureFilter(gCodeFont.texture, TEXTURE_FILTER_BILINEAR);

    srand((unsigned)time(nullptr));

    Theme theme = MakeTheme();
    LinkedList list;
    OperationState op;

    std::vector<std::vector<int>> undoStack;
    std::vector<std::vector<int>> redoStack;

    std::string status = "Ready";

    auto SaveState = [&]() {
        undoStack.push_back(list.toVector());
        if (undoStack.size() > 80) {
            undoStack.erase(undoStack.begin());
        }
        redoStack.clear();
        };

    auto ClearOperation = [&](bool clearPseudo = true) {
        op.type = ANIM_NONE;
        op.playing = false;
        op.step = 0;
        op.foundIndex = -1;
        op.targetValue = -1;
        op.newValue = -1;
        op.timer = 0.0f;
        op.flashActive = false;
        op.flashIndex = -1;
        op.flashTimer = 0.0f;
        op.flashDuration = 0.85f;
        if (clearPseudo) {
            op.pseudo.clear();
            op.highlightLine = -1;
        }
        };

    auto BeginSearch = [&](int value) {
        ClearOperation();
        op.type = ANIM_SEARCH;
        op.playing = true;
        op.step = 0;
        op.targetValue = value;
        op.foundIndex = FindFirstIndex(list.toVector(), value);
        op.pseudo = GetPseudoCode(ANIM_SEARCH);
        op.highlightLine = 0;
        status = "Searching for " + std::to_string(value);

        if (GetListSize(list) == 0) {
            op.playing = false;
            op.highlightLine = 5;
            status = "List is empty";
        }
        };

    auto BeginInsertFront = [&](int value) {
        SaveState();
        list.insertFront(value);

        ClearOperation();
        op.type = ANIM_INSERT_FRONT;
        op.pseudo = GetPseudoCode(ANIM_INSERT_FRONT);
        op.highlightLine = 0;
        op.flashActive = true;
        op.flashIndex = 0;
        op.flashTimer = 0.0f;
        op.flashDuration = 0.85f;
        status = "Inserted " + std::to_string(value) + " at front";
        };

    auto BeginInsertBack = [&](int value) {
        SaveState();
        list.insertBack(value);

        ClearOperation();
        op.type = ANIM_INSERT_BACK;
        op.pseudo = GetPseudoCode(ANIM_INSERT_BACK);
        op.highlightLine = 0;
        op.flashActive = true;
        op.flashIndex = GetListSize(list) - 1;
        op.flashTimer = 0.0f;
        op.flashDuration = 0.85f;
        status = "Inserted " + std::to_string(value) + " at back";
        };

    auto BeginDelete = [&](int value) {
        int idx = FindFirstIndex(list.toVector(), value);

        ClearOperation();
        op.type = ANIM_DELETE;
        op.pseudo = GetPseudoCode(ANIM_DELETE);

        if (idx == -1) {
            op.highlightLine = 1;
            status = "Value not found";
            return;
        }

        SaveState();
        list.deleteValue(value);
        op.highlightLine = 3;
        status = "Deleted " + std::to_string(value);
        };

    auto BeginUpdate = [&](int oldValue, int newValue) {
        int idx = FindFirstIndex(list.toVector(), oldValue);

        ClearOperation();
        op.type = ANIM_UPDATE;
        op.pseudo = GetPseudoCode(ANIM_UPDATE);

        if (idx == -1) {
            op.highlightLine = 0;
            status = "Value not found";
            return;
        }

        SaveState();
        list.updateValue(oldValue, newValue);

        op.highlightLine = 2;
        op.flashActive = true;
        op.flashIndex = idx;
        op.flashTimer = 0.0f;
        op.flashDuration = 0.95f;
        status = "Updated " + std::to_string(oldValue) + " -> " + std::to_string(newValue);
        };

    auto AdvanceSearchStep = [&]() {
        int total = GetListSize(list);

        if (total == 0) {
            op.highlightLine = 5;
            op.playing = false;
            status = "List is empty";
            return;
        }

        if (op.step < total) {
            if (op.step == op.foundIndex) {
                op.step++;
                op.highlightLine = 3;
                op.playing = false;
                status = "Found " + std::to_string(op.targetValue);
            }
            else {
                op.step++;
                if (op.step >= total && op.foundIndex == -1) {
                    op.highlightLine = 5;
                    op.playing = false;
                    status = "Not found";
                }
                else {
                    op.highlightLine = 4;
                }
            }
        }
        };

    auto SearchStepBackward = [&]() {
        if (op.type != ANIM_SEARCH) return;
        if (op.step > 0) op.step--;
        if (op.step == 0) op.highlightLine = 0;
        else op.highlightLine = 1;
        op.playing = false;
        status = "Search paused";
        };

    InputBox inputValue(60, 168, 130, 48, WHITE, BLACK);
    InputBox inputNew(220, 168, 130, 48, WHITE, BLACK);
    Slider speedSlider(60, 262, 250, 14, 0.3f, 3.0f, 1.1f);

    ActionButton btnBack = { Rectangle{ 42, 34, 92, 40 },  "Back",       Color{245, 247, 250, 255}, theme.border };

    ActionButton btnFront = { Rectangle{ 392, 164, 112, 44 }, "Add Front", theme.primarySoft, theme.primary };
    ActionButton btnBackIns = { Rectangle{ 516, 164, 112, 44 }, "Add Back",  theme.primarySoft, theme.primary };
    ActionButton btnDelete = { Rectangle{ 640, 164, 92, 44 },  "Delete",    theme.dangerSoft,  theme.danger };
    ActionButton btnSearch = { Rectangle{ 744, 164, 92, 44 },  "Search",    theme.warningSoft, theme.warning };
    ActionButton btnUpdate = { Rectangle{ 848, 164, 92, 44 },  "Update",    theme.successSoft, theme.success };

    ActionButton btnPlay = { Rectangle{ 392, 222, 76, 38 },  "Play",      Color{255, 237, 213, 255}, theme.warning };
    ActionButton btnPrev = { Rectangle{ 480, 222, 76, 38 },  "Prev",      Color{241, 245, 249, 255}, theme.border };
    ActionButton btnNext = { Rectangle{ 568, 222, 76, 38 },  "Next",      Color{241, 245, 249, 255}, theme.border };
    ActionButton btnUndo = { Rectangle{ 656, 222, 76, 38 },  "Undo",      theme.violetSoft, theme.violet };
    ActionButton btnRedo = { Rectangle{ 744, 222, 76, 38 },  "Redo",      theme.successSoft, theme.success };
    ActionButton btnRandom = { Rectangle{ 832, 222, 92, 38 },  "Generate",  theme.cyanSoft, theme.cyan };
    ActionButton btnReset = { Rectangle{ 936, 222, 68, 38 },  "Reset",     Color{241, 245, 249, 255}, theme.border };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        float dt = GetFrameTime();
        float speedFactor = speedSlider.currentValue;

        inputValue.checkPressed(mousePos, mousePressed);
        inputNew.checkPressed(mousePos, mousePressed);
        inputValue.Update();
        inputNew.Update();
        speedSlider.Update(mousePos, mouseDown);

        bool searchTimelineEnabled = (op.type == ANIM_SEARCH);

        if (mousePressed && CheckCollisionPointRec(mousePos, btnPlay.rect) && !searchTimelineEnabled) {
            status = "Search controls are available after Search.";
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnPrev.rect) && !searchTimelineEnabled) {
            status = "Search controls are available after Search.";
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnNext.rect) && !searchTimelineEnabled) {
            status = "Search controls are available after Search.";
        }

        if (IsButtonPressed(btnBack, mousePos, mousePressed)) {
            break;
        }

        if (IsButtonPressed(btnFront, mousePos, mousePressed)) {
            int v = inputValue.GetValue();
            if (v == -1) status = "Enter Target first.";
            else {
                BeginInsertFront(v);
                inputValue.Clear();
            }
        }

        if (IsButtonPressed(btnBackIns, mousePos, mousePressed)) {
            int v = inputValue.GetValue();
            if (v == -1) status = "Enter Target first.";
            else {
                BeginInsertBack(v);
                inputValue.Clear();
            }
        }

        if (IsButtonPressed(btnDelete, mousePos, mousePressed)) {
            int v = inputValue.GetValue();
            if (v == -1) status = "Enter Target first.";
            else {
                BeginDelete(v);
                inputValue.Clear();
            }
        }

        if (IsButtonPressed(btnSearch, mousePos, mousePressed)) {
            int v = inputValue.GetValue();
            if (v == -1) status = "Enter Target first.";
            else BeginSearch(v);
        }

        if (IsButtonPressed(btnUpdate, mousePos, mousePressed)) {
            int oldValue = inputValue.GetValue();
            int newValue = inputNew.GetValue();

            if (oldValue == -1) {
                status = "Enter Target for Update.";
            }
            else if (newValue == -1) {
                status = "Enter Replace value.";
            }
            else {
                BeginUpdate(oldValue, newValue);
                inputValue.Clear();
                inputNew.Clear();
            }
        }

        if (IsButtonPressed(btnPlay, mousePos, mousePressed, searchTimelineEnabled)) {
            op.playing = !op.playing;
        }

        if (IsButtonPressed(btnPrev, mousePos, mousePressed, searchTimelineEnabled)) {
            SearchStepBackward();
        }

        if (IsButtonPressed(btnNext, mousePos, mousePressed, searchTimelineEnabled)) {
            AdvanceSearchStep();
        }

        if (IsButtonPressed(btnUndo, mousePos, mousePressed, !undoStack.empty())) {
            redoStack.push_back(list.toVector());
            list.loadFromVector(undoStack.back());
            undoStack.pop_back();
            ClearOperation();
            status = "Undo";
        }

        if (IsButtonPressed(btnRedo, mousePos, mousePressed, !redoStack.empty())) {
            undoStack.push_back(list.toVector());
            list.loadFromVector(redoStack.back());
            redoStack.pop_back();
            ClearOperation();
            status = "Redo";
        }

        if (IsButtonPressed(btnRandom, mousePos, mousePressed)) {
            SaveState();
            list.clear();
            int n = 6 + rand() % 4;
            for (int i = 0; i < n; i++) {
                list.insertBack(rand() % 100);
            }
            ClearOperation();
            status = "Random list generated";
        }

        if (IsButtonPressed(btnReset, mousePos, mousePressed)) {
            SaveState();
            list.clear();
            ClearOperation();
            status = "List cleared";
        }

        if (op.type == ANIM_SEARCH && op.playing) {
            op.timer += dt * speedFactor;
            if (op.timer >= 0.52f) {
                op.timer = 0.0f;
                AdvanceSearchStep();
            }
        }

        if (op.flashActive) {
            op.flashTimer += dt * speedFactor;
            float p = Clamp01(op.flashTimer / op.flashDuration);

            int lineCount = (int)op.pseudo.size();
            if (lineCount > 0) {
                int line = (int)(p * lineCount);
                if (line >= lineCount) line = lineCount - 1;
                op.highlightLine = line;
            }

            if (p >= 1.0f) {
                op.flashActive = false;
            }
        }

        std::vector<int> values = list.toVector();
        std::string sizeText = std::to_string((int)values.size());
        std::string headText = values.empty() ? "-" : std::to_string(values.front());
        std::string tailText = values.empty() ? "-" : std::to_string(values.back());

        BeginDrawing();
        ClearBackground(theme.bg);

        Rectangle header = { 24, 20, 1452, 76 };
        DrawPanel(header, 0.06f, theme, theme.panel);
        DrawRectangleGradientH((int)header.x + 1, (int)header.y + 1, 8, (int)header.height - 2, theme.primary, theme.cyan);

        DrawButton(btnBack, mousePos, theme);
        DrawUI("Doubly Linked List Visualization", 180, 31, 36, theme.text);
        DrawUI("Minimal interface with animated interaction flow", 182, 66, 18, theme.textSoft);

        DrawMetricCard(1038, 29, 118, "Size", sizeText, theme);
        DrawMetricCard(1168, 29, 118, "Head", headText, theme);
        DrawMetricCard(1298, 29, 118, "Tail", tailText, theme);

        Rectangle controls = { 24, 112, 980, 160 };
        DrawPanel(controls, 0.06f, theme, theme.panel);

        DrawUI("Target", 60, 144, 18, theme.textSoft);
        DrawUI("Replace", 220, 144, 18, theme.textSoft);

        DrawInputField(inputValue, " ", "12", theme);
        DrawInputField(inputNew, " ", "99", theme);

        DrawSliderMinimal(speedSlider, theme);

        DrawButton(btnFront, mousePos, theme);
        DrawButton(btnBackIns, mousePos, theme);
        DrawButton(btnDelete, mousePos, theme);
        DrawButton(btnSearch, mousePos, theme);
        DrawButton(btnUpdate, mousePos, theme);

        DrawButton(btnPlay, mousePos, theme, searchTimelineEnabled);
        DrawButton(btnPrev, mousePos, theme, searchTimelineEnabled);
        DrawButton(btnNext, mousePos, theme, searchTimelineEnabled);
        DrawButton(btnUndo, mousePos, theme, !undoStack.empty());
        DrawButton(btnRedo, mousePos, theme, !redoStack.empty());
        DrawButton(btnRandom, mousePos, theme);
        DrawButton(btnReset, mousePos, theme);

        Rectangle canvas = { 24, 288, 980, 560 };
        DrawLinkedListMinimal(list, canvas, op, theme);

        DrawCodePanelMinimal(op, theme);

        Rectangle statusBar = { 24, 856, 1452, 24 };
        DrawPanel(statusBar, 0.4f, theme, theme.panel);

        Color dot = theme.primary;
        if (status.find("Deleted") != std::string::npos) dot = theme.danger;
        else if (status.find("Found") != std::string::npos) dot = theme.success;
        else if (status.find("Updated") != std::string::npos) dot = theme.warning;
        else if (status.find("not found") != std::string::npos || status.find("Not found") != std::string::npos) dot = theme.warning;

        DrawCircle(44, 868, 6, dot);
        DrawUI(status, 58, 858, 19, theme.textSoft);

        EndDrawing();
    }

    if (gUnloadCodeFont) UnloadFont(gCodeFont);
    if (gUnloadUiFont) UnloadFont(gUiFont);

    CloseWindow();
}