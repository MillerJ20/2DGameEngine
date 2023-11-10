#ifndef GAME_H
#define GAME_H

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game {
private:
  bool isRunning;
  int msPreviousFrame = 0;
  SDL_Window* window;
  SDL_Renderer* renderer;

  std::unique_ptr<Registry> registry;
  std::unique_ptr<AssetStore> assetStore;

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
