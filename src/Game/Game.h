#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game {
private:
  bool isRunning;
  int msPreviousFrame = 0;
  SDL_Window* window;
  SDL_Renderer* renderer;

  Registry* registry;

public:
  Game();
  ~Game();
  void Setup();
  void Initialize();
  void Run();
  void ProcessInput();
  void Update();
  void Render();
  void Destroy();

  int windowWidth;
  int windowHeight;
};

#endif
