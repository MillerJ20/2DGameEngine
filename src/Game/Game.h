#ifndef GAME_H
#define GAME_H

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>
#include <string>

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game {
private:
  bool isRunning;
  bool isDebug;
  int msPreviousFrame = 0;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Rect camera;

  std::unique_ptr<Registry> registry;
  std::unique_ptr<AssetStore> assetStore;
  std::unique_ptr<EventManager> eventManager;

public:
  Game();
  ~Game();
  void LoadLevel(int levelNumber);
  void BuildLevelMap(std::string filePath);
  void ReadMapFile(std::string filePath);
  void Setup();
  void Initialize();
  void Run();
  void ProcessInput();
  void Update();
  void Render();
  void Destroy();

  static int windowWidth;
  static int windowHeight;
  static int mapWidth;
  static int mapHeight;
};

#endif
