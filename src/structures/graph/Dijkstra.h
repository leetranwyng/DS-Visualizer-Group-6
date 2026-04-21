#pragma once
#include <vector>
#include <raylib.h>
#include <string>
#include "../../UI/UI.h"
using namespace std;

struct edge
{
    int node, weight;
};

struct cmp
{
    bool operator()(edge u, edge v)
    {
        return u.weight > v.weight;
    }
};

struct state
{
    int node, updatedNode, newDis;
};

struct Dijkstra
{
    int size;
    vector<vector<edge>> adj;
    vector<state> history;

    void build_size();
    void build_edge(int u, int v, int w);
    void implement(int sNode);
    void randomGraph(int n);
};

struct Node
{
    Vector2 pos;
    int d;
    int state;
};

struct Tool
{
    int random(int l, int r);
    void shuffleVector(vector<Vector2>& contain);
    string convert(int w);
    bool isInNode(Vector2 pos, Vector2 node, float radius);
    int posToNode(Vector2 pos, vector<Node>& node, float radius);
    void drawArrow(Vector2 u, Vector2 v, float radius, Color color);
};

struct UI
{
    vector<Node> node;
    float w, h, cellW, cellH, lim, radius;
    int usedNode;
    UI()
    {
        w = 800;
        h = 600;
        cellW = 800 / 8;
        cellH = 600 / 8;
        lim = 8;
        radius = 30;
        usedNode = -1;
    }
    void placeNode(Dijkstra*& logic);
    void drawNode(Dijkstra*& logic);
    void moveNode();
};