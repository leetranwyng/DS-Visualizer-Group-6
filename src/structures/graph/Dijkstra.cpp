#include "dijkstra.h"
#include <iostream>
#include <raylib.h>
#include <queue>
#include <cmath>
#include <random>
#include <algorithm>
#include <map>

void Dijkstra::build_size()
{
    adj.clear();
    adj.resize(size);
}

void Dijkstra::build_edge(int u, int v, int w)
{
    adj[u].push_back({v,w});
}

void Dijkstra::implement(int sNode)
{
    vector<int> d;
    d.resize(size);
    history.clear();

    for (int i=0;i<size;i++) d[i] = INT_MAX;

    priority_queue<edge,vector<edge>,cmp> q;

    d[sNode] = 0;
    q.push({sNode,0});

    while(q.size())
    {
        edge u = q.top();
        q.pop();
        if (u.weight > d[u.node]) continue;

        for (edge v: adj[u.node])
        {
            if (d[v.node] > d[u.node] + v.weight)
            {
                d[v.node] = d[u.node] + v.weight;
                q.push({v.node, d[v.node]});
            } 
            history.push_back({u.node, v.node, d[v.node]});
        }
    }

}

int Tool::random(int l, int r)
{
    static mt19937 gen(random_device{}());
    return uniform_int_distribution<>(l,r)(gen);
}

void Tool::shuffleVector(vector<Vector2> &contain)
{
    random_device rd;
    mt19937 gen(rd());
    shuffle(contain.begin(),contain.end(),gen);
}

string Tool::convert(int w)
{
    if (w<1e9)
    {
        if (!w) return "0";

        string s = "";
        while(w>0) s = char('0' + w%10) + s, w/=10;
        return s;
    }

    return "INF";
}

bool Tool::isInNode(Vector2 pos, Vector2 node, float radius)
{
    float dist = (pos.x-node.x)*(pos.x-node.x) + (pos.y-node.y)*(pos.y-node.y);
    if (dist<=radius*radius) return 1;
    return 0;
}

int Tool::posToNode(Vector2 pos, vector<Node> &node, float radius)
{
    for (int i=0;i<node.size();i++)
    {
        if (isInNode(pos,node[i].pos,radius)) return i;
    }
    return -1;
}

void Tool::drawArrow(Vector2 u, Vector2 v, float radius, Color color)
{
    Vector2 direction = {v.x - u.x, v.y - u.y};
    float len = sqrt(direction.x*direction.x + direction.y*direction.y);
    //if (len==0) cout<<u.x<<' '<<u.y<<' '<<v.x<<' '<<v.y<<'\n';
    direction.x/=len;
    direction.y/=len;

    Vector2 touch = {v.x - direction.x*radius, v.y - direction.y*radius};
    Vector2 per = {-direction.y, direction.x};

    float size = 10;

    Vector2 v1 = {touch.x - direction.x*size + per.x*size*0.5f, touch.y - direction.y*size + per.y*size*0.5f};
    Vector2 v2 = {touch.x - direction.x*size - per.x*size*0.5f, touch.y - direction.y*size - per.y*size*0.5f};
    //cout<< touch.x << ' '<<touch.y<<' '<<v1.x<<' '<<v1.y<<' '<<v2.x<< ' '<<v2.y<<endl;
    DrawTriangle(touch,v2,v1, color);
}

void Dijkstra::randomGraph(int n)
{
    size = n;
    build_size();
    Tool tool;
    int max = min(n*3/2,n*(n-1)/2);
    int edge = tool.random(min(n,max),max);
    map<pair<int,int>, bool> used;

    for (int i=0;i<edge;i++)
    {
        int u = tool.random(0,n-1);
        int v = tool.random(0,n-1);
        int cnt = 0;
        while(u==v || used[{u,v}] || used[{v,u}])
        {
            v = tool.random(0,n-1);
            cnt++;
            if (cnt>100) break;
        }
        int w = tool.random(1,50);
        build_edge(u,v,w);
        used[{u,v}] = 1;
    }
}

void UI::placeNode(Dijkstra* &logic)
{
    node.clear();
    node.resize(logic->size);
    vector<Vector2> pos;

    for (int i=1;i<=lim;i++) for (int j=1;j<=lim;j++) pos.push_back({i*cellW,j*cellH});

    Tool tool;
    tool.shuffleVector(pos);

    for (int i=0;i<logic->size;i++)
    {
        node[i] = {pos[i],1000000000,0};
    }

}

void UI::drawNode(Dijkstra* &logic)
{
    Tool tool;
    for (int u=0;u<logic->size;u++)
    {
        for (edge v: logic->adj[u])
        {
            Vector2 posU = node[u].pos;
            Vector2 posV = node[v.node].pos;

            DrawLineEx(posU, posV, 2.0f, DARKGRAY);
            tool.drawArrow(posU, posV, radius, DARKGRAY);
            Vector2 midpoint = {(posU.x+posV.x)/2, (posU.y+posV.y)/2};
            string s = tool.convert(v.weight);
            const char* d = s.c_str();
            DrawText(d, midpoint.x, midpoint.y, 20, BLACK);
        }
    }

    for (int i=0;i<logic->size;i++)
    {
        Vector2 pos = node[i].pos;
        DrawCircle(pos.x, pos.y, radius, WHITE);
        DrawCircle(pos.x, pos.y, radius, {120,120,120,225});
        //DrawCircleLines(pos.x, pos.y, radius, BLACK);

        string s = tool.convert(i);
        const char* c1 = s.c_str();
        DrawText(c1, pos.x-MeasureText(c1, 30)/2, pos.y-30/2, 30, BLACK);

        s = tool.convert(node[i].d);
        const char* c2 = s.c_str();
        DrawText(c2, pos.x-MeasureText(c2, 20)/2, pos.y-radius-18, 20, BLACK);

    }

}

void UI::moveNode()
{
    Vector2 mouse = GetMousePosition();
    Tool tool;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        usedNode = tool.posToNode(mouse, node, radius);
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && usedNode!=-1)
    {
        if (mouse.x<=800 && mouse.x>=30 && mouse.y<=600 && mouse.y>=30)
        {
            node[usedNode].pos = mouse;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        usedNode = -1;
    }

}