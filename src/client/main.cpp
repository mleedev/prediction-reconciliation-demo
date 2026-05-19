#include "raylib.h"

static void updateDrawFrame(void);

int main() 
{
    // Initialize raylib window
    const int screenWidth  = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Prediction/Reconciliation Demo");

    // Set FPS for raylib window
    SetTargetFPS(60);

    // Raylib draw loop
    while (!WindowShouldClose()) updateDrawFrame();
    CloseWindow();

    return 0;
}

static void updateDrawFrame(void)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    EndDrawing();
}