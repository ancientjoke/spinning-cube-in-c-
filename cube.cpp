#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <windows.h>
#include <vector>

const int width = 100; 
const int height = 42;
float A = 0, B = 0, C = 0;
float cubeWidth = 5;  
float K1 = 10;       
char buffer[height][width];
float zBuffer[height][width];

struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Edge {
    Vec3 p1, p2;
    Edge(const Vec3& p1, const Vec3& p2) : p1(p1), p2(p2) {}
};

std::vector<Edge> edges;

void initCube() {
    float w = cubeWidth;
    std::vector<Vec3> vertices = {
        Vec3(-w,-w,-w), Vec3(w,-w,-w), Vec3(w,w,-w), Vec3(-w,w,-w),
        Vec3(-w,-w,w), Vec3(w,-w,w), Vec3(w,w,w), Vec3(-w,w,w)
    };
    
    edges = {
        Edge(vertices[0], vertices[1]), Edge(vertices[1], vertices[2]),
        Edge(vertices[2], vertices[3]), Edge(vertices[3], vertices[0]),
        Edge(vertices[4], vertices[5]), Edge(vertices[5], vertices[6]),
        Edge(vertices[6], vertices[7]), Edge(vertices[7], vertices[4]),
        Edge(vertices[0], vertices[4]), Edge(vertices[1], vertices[5]),
        Edge(vertices[2], vertices[6]), Edge(vertices[3], vertices[7])
    };
}

void calculateVertex(const Vec3& v, Vec3& out) {
    float x1 = v.x;
    float y1 = cos(A) * v.y - sin(A) * v.z;
    float z1 = sin(A) * v.y + cos(A) * v.z;
    
    float x2 = cos(B) * x1 + sin(B) * z1;
    float y2 = y1;
    float z2 = -sin(B) * x1 + cos(B) * z1;
    
    float x3 = cos(C) * x2 - sin(C) * y2;
    float y3 = sin(C) * x2 + cos(C) * y2;
    float z3 = z2;
    
    float z = K1 / (K1 + z3);
    out.x = x3 * z * 2;
    out.y = y3 * z * 2;
    out.z = z3;
}

void drawPoint(int x, int y, float z, char ch) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        if (z > zBuffer[y][x]) {
            zBuffer[y][x] = z;
            buffer[y][x] = ch;
        }
    }
}

void drawLine(int x1, int y1, float z1, int x2, int y2, float z2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    
    while (true) {
        drawPoint(x1, y1, z1, '#');
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

int main() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);
    
    initCube();
    
    while (true) {
        memset(buffer, ' ', sizeof(buffer));
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                zBuffer[i][j] = -std::numeric_limits<float>::infinity();

        for (const auto& edge : edges) {
            Vec3 p1, p2;
            calculateVertex(edge.p1, p1);
            calculateVertex(edge.p2, p2);
            
            int x1 = (int)(p1.x + width / 2);
            int y1 = (int)(p1.y + height / 2);
            int x2 = (int)(p2.x + width / 2);
            int y2 = (int)(p2.y + height / 2);
            
            drawLine(x1, y1, p1.z, x2, y2, p2.z);
        }

        SetConsoleCursorPosition(out, {0, 0});
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                std::cout << buffer[i][j];
            }
            std::cout << '\n';
        }

        A += 0.05;
        B += 0.05;
        C += 0.01;

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    return 0;
}
