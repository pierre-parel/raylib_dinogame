#include "raylib.h"
#include <cmath>
#include <iostream>
#include <vector>

// Enum
enum GameScreen { TITLE, GAMEPLAY, ENDING, CREDITS };
enum ObstacleType { GROUND, AIR };

class Player {
public:
  Rectangle rect;
  float speed;
  bool canJump;
};

class Obstacle {
public:
  ObstacleType obstacleType = GROUND;
  Rectangle rect;
};

// Generic game variables
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 300;

static const float PLAYER_JMP_SPEED = 450.0f;
static const float GRAVITY = 900;
static const int MAX_OBSTACLES = 500;

// TODO: Change whenever testing specific parts (Default: TITLE)
static int currentScreen = GAMEPLAY;
static bool exitGame = false;
static bool isAlive = true;
static int optionSelect = 0;
static int score = 0;

static Player player{};
static std::vector<Obstacle> obstacles;
static std::vector<Vector2> obstaclePos;

static int obstacleSpeed;
static float deltaTime;
static float lastUpdateTime;

static RenderTexture2D screenTarget;
static Texture2D player_atlas;

// Module Functions Declarations
void InitGame();
void GenerateObstacles();
void UpdateGame();
void UpdatePlayer();
void UpdateObstacles();
void DrawGame();

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dino Run");
  SetTargetFPS(60);
  InitGame();

  while (!WindowShouldClose() && !exitGame) {
    UpdateGame();
    DrawGame();
  }

  CloseWindow();

  return 0;
}

void InitGame() {
  GenerateObstacles();

  currentScreen = GAMEPLAY;
  exitGame = false;
  isAlive = true;
  optionSelect = 0;
  score = 0;
  player.canJump = true;
  player.rect.y = 200;
  player.rect.x = 60;
  player.speed = 0;
  player.rect.width = 60;
  player.rect.height = 60;

  screenTarget = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
  SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_BILINEAR);

  player_atlas = LoadTexture("res/player_atlas.png");
}

void GenerateObstacles() {
  obstacles.reserve(MAX_OBSTACLES);
  obstaclePos.reserve(MAX_OBSTACLES);
  obstacleSpeed = 8;

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstaclePos[i].x = 700 + GetRandomValue(600, 800) * i;
    obstaclePos[i].y = 210;
  }

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].rect.x = obstaclePos[i].x;
    obstacles[i].rect.y = obstaclePos[i].y;
    obstacles[i].rect.width = GetRandomValue(5, 10) * 10;
    obstacles[i].rect.height = 50;
  }
}

void UpdateGame() {
  deltaTime = GetFrameTime();
  lastUpdateTime = 0;

  switch (currentScreen) {
  case TITLE: {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
      optionSelect--;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
      optionSelect++;
    }

    if (optionSelect < 0) {
      optionSelect = 0;
    } else if (optionSelect > 2) {
      optionSelect = 2;
    }

    if (IsKeyPressed(KEY_ENTER)) {
      if (optionSelect == 0) {
        currentScreen = GAMEPLAY;
      } else if (optionSelect == 1) {
        currentScreen = CREDITS;
      } else if (optionSelect == 2) {
        exitGame = true;
      }
    }

  } break;

  case GAMEPLAY: {
    DrawText(TextFormat("Score: %4i", score), WINDOW_WIDTH - 150, 0, 20,
             RAYWHITE);
    if (isAlive) {
      double currentTime = GetTime();

      if (currentTime - lastUpdateTime >= 0.1) {
        score++;
      }

      UpdateObstacles();
      UpdatePlayer();
    } else {
      DrawText("Game Over", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, 20,
               RAYWHITE);
      if (IsKeyPressed(KEY_R)) {
        currentScreen = GAMEPLAY;
        InitGame();
      }
      break;
    }
  } break;

  case ENDING: {
    DrawText("Game Over", WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2, 20,
             RAYWHITE);
    DrawText(TextFormat("Score: %4i", score), WINDOW_WIDTH / 2 - 220,
             WINDOW_HEIGHT / 2 + 20, 20, RAYWHITE);

  } break;
  }
}

void UpdatePlayer() {
  if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && player.canJump) {
    player.speed = -PLAYER_JMP_SPEED;
    player.canJump = false;
  }

  // TODO: Collision detection
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (CheckCollisionRecs(player.rect, obstacles[i].rect)) {
      isAlive = false;
    }
  }

  player.rect.y += player.speed * deltaTime;
  player.speed += GRAVITY * deltaTime;
  if (player.rect.y >= 200) {
    player.canJump = true;
    player.rect.y = 200;
  }
}

void UpdateObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].rect.x -= obstacleSpeed;
  }
}

void DrawGame() {
  BeginDrawing();
  ClearBackground(DARKGRAY);

  switch (currentScreen) {
  case TITLE: {
    DrawText("Start game", 350, 200, 20, (optionSelect == 0) ? RED : LIGHTGRAY);
    DrawText("Credits", 350, 200 + 20, 20,
             (optionSelect == 1) ? RED : LIGHTGRAY);
    DrawText("Exit game", 350, 200 + 40, 20,
             (optionSelect == 2) ? RED : LIGHTGRAY);
  } break;

  case GAMEPLAY: {
    // TODO: Sprite animation
    DrawTexturePro(player_atlas, Rectangle{0, 0, 24, 24},
                   Rectangle{player.rect.x, player.rect.y, player.rect.width,
                             player.rect.height},
                   Vector2{0, 0}, 0.0f, RAYWHITE);
    DrawRectangleLinesEx(player.rect, 2.0f, RED);

    for (int i = 0; i < MAX_OBSTACLES; i++) {
      DrawRectangle(obstacles[i].rect.x, obstacles[i].rect.y,
                    obstacles[i].rect.width, obstacles[i].rect.height, RED);
      DrawRectangleLinesEx(obstacles[i].rect, 2.0f, BLUE);
    }
  } break;

  case ENDING: {
  } break;
  }
  EndDrawing();
}
