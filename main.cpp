#include "raylib.h"
#include <vector>

// Enum
enum GameScreen { TITLE, GAMEPLAY, ENDING, CREDITS };
enum ObstacleType { GROUND, AIR };

class Player {
public:
  Vector2 pos;
  float speed;
  bool canJump;
};

class Obstacle {
public:
  ObstacleType obstacleType = GROUND;
  Vector2 pos{0, 200};
};

// Generic game variables
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 300;

static const float PLAYER_JMP_SPEED = 350.0f;
static const float GRAVITY = 900;
static const int MAX_OBSTACLES = 100;

// TODO: Change whenever testing specific parts (Default: TITLE)
static int currentScreen = GAMEPLAY;
static bool exitGame = false;
static int optionSelect = 0;

static Player player{};
static std::vector<Obstacle> obstacles;
static std::vector<Vector2> obstaclePos;

static int obstacleSpeed;
static float deltaTime;

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
  InitGame();

  while (!WindowShouldClose() && !exitGame) {
    UpdateGame();
    DrawGame();
  }

  CloseWindow();

  return 0;
}

void InitGame() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dino Run");
  SetTargetFPS(60);
  GenerateObstacles();

  player.canJump = true;
  player.pos.y = 200;

  screenTarget = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
  SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_BILINEAR);

  player_atlas = LoadTexture("res/player_atlas.png");
}

void GenerateObstacles() {
  obstacles.reserve(MAX_OBSTACLES);
  obstaclePos.reserve(MAX_OBSTACLES);
  obstacleSpeed = 5;

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstaclePos[i].x = 400 + GetRandomValue(350, 600) * i;
    obstaclePos[i].y = 200;
  }

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].pos.x = obstaclePos[i].x;
    obstacles[i].pos.y = obstaclePos[i].y;
  }
}

void UpdateGame() {
  deltaTime = GetFrameTime();
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
    UpdateObstacles();
    UpdatePlayer();
  } break;

  case ENDING: {
  } break;
  }
}

void UpdatePlayer() {
  // TODO: Collision detection
  if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && player.canJump) {
    player.speed = -PLAYER_JMP_SPEED;
    player.canJump = false;
  }

  player.pos.y += player.speed * deltaTime;
  player.speed += GRAVITY * deltaTime;
  if (player.pos.y >= 200) {
    player.canJump = true;
    player.pos.y = 200;
  }
}

void UpdateObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].pos.x -= obstacleSpeed;
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
                   Rectangle{player.pos.x, player.pos.y, 60, 60},
                   Vector2{12, 12}, 0.0f, RAYWHITE);
    for (int i = 0; i < MAX_OBSTACLES; i++) {
      DrawRectangle(obstacles[i].pos.x, obstacles[i].pos.y, 50, 50, RED);
    }
  } break;

  case ENDING: {
  } break;
  }
  EndDrawing();
}
