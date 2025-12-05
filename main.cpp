#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const double GRAVITY = -0.5;
const double RADIUS = 10;
const double e = 0.99;
const double TOLERANCE = 0.2;
const double MASS = 1;

vector<Vector2> pos;
vector<Vector2> vel;

void Spawn(int n) {
    for (int i = 0; i < n; i++) {
        pos.push_back({(float)(GetRandomValue(50, SCREEN_WIDTH - 50)), (float)(GetRandomValue(50, SCREEN_HEIGHT - 50))});
        vel.push_back({0.0f, 0.0f});
    }
}

void Draw() {
    ClearBackground(RAYWHITE);
    DrawText("Press Space to Spawn Balls", 2, 0, 20, LIGHTGRAY);

    // draw balls
    for (int i = 0; i < vel.size(); i++) {
        DrawCircle(pos[i].x, pos[i].y, RADIUS, RED);
    }
}

void Update() {
    for (int i = 0; i < vel.size(); i++) {
        // update velocity with gravity
        vel[i].y -= GRAVITY;

        // update position
        pos[i].x += vel[i].x;
        pos[i].y += vel[i].y;

        // screen collision
        if (pos[i].x < 0 + RADIUS || pos[i].x > SCREEN_WIDTH - RADIUS) vel[i].x *= -e;
        if (pos[i].y < 0 + RADIUS || pos[i].y > SCREEN_HEIGHT - RADIUS) vel[i].y *= -e;

        // clamp position
        if (pos[i].x < 0 + RADIUS) pos[i].x = 0 + RADIUS;
        if (pos[i].x > SCREEN_WIDTH - RADIUS) pos[i].x = SCREEN_WIDTH - RADIUS;
        if (pos[i].y < 0 + RADIUS) pos[i].y = 0 + RADIUS;
        if (pos[i].y > SCREEN_HEIGHT - RADIUS) pos[i].y = SCREEN_HEIGHT - RADIUS;
    }
}

void ResolveCollision(int i, int j) {
    Vector2 normal = Vector2Subtract(pos[j], pos[i]);
    float dist = Vector2Length(normal);

    float nx = normal.x / dist;
    float ny = normal.y / dist;
    float vrel = (vel[j].x - vel[i].x) * nx + (vel[j].y - vel[i].y) * ny;

    if (vrel > 0) return;

    float jImpulse = -(1 + e) * vrel / (1 / MASS + 1 / MASS);
    vel[i].x -= (jImpulse / MASS) * nx;
    vel[i].y -= (jImpulse / MASS) * ny;
    vel[j].x += (jImpulse / MASS) * nx;
    vel[j].y += (jImpulse / MASS) * ny;

    // overlap correction
    float overlap = 2 * RADIUS - dist;

    if (overlap > 0) {
        float total = MASS * 2;
        pos[i].x -= nx * (overlap * (MASS / total));
        pos[i].y -= ny * (overlap * (MASS / total));
        pos[j].x += nx * (overlap * (MASS / total));
        pos[j].y += ny * (overlap * (MASS / total));
    }
}


void Physics() {
    for (int i = 0; i < vel.size(); i++) {
        for (int j = i + 1; j < vel.size(); j++) {
            Vector2 normal = Vector2Subtract(pos[j], pos[i]);
            float dist = Vector2Length(normal);
            if (dist < 2 * RADIUS + TOLERANCE) {
                ResolveCollision(i, j);
            }
        }
    }
}

void Inputs() {
    if (IsKeyDown(KEY_SPACE)) {
        Spawn(1);
    }
}

int main () {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Balls");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        Physics();
        Update();
        Inputs();
        Draw();
        EndDrawing();
    }
    
    CloseWindow();
}

