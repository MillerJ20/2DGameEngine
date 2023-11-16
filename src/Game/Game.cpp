#include "Game.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/RenderSystem.h"
#include "glm/fwd.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <glm/glm.hpp>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

Game::Game() {
  isRunning = false;
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();
  Logger::Log("Game constructor called!");
}

Game::~Game() { Logger::Log("Game destructor called!"); }

void Game::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err("Error initializing SDL.");
    return;
  }
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  windowWidth = displayMode.w;
  windowHeight = displayMode.h;
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    Logger::Err("Error creating SDL window.");
    return;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    Logger::Err("Error creating SDL renderer.");
    return;
  }
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  isRunning = true;
}

void Game::ProcessInput() {
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent)) {
    switch (sdlEvent.type) {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
        isRunning = false;
      }
      if (sdlEvent.key.keysym.sym == SDLK_d) {
        isDebug = !isDebug;
      }
      break;
    }
  }
}

void Game::ReadMapFile(std::string filePath) {
  std::fstream file;

  file.open(filePath.c_str());

  std::string word, row;
  double x = 0, y = 0;

  while (file >> row) {
    std::stringstream stream(row);
    while (std::getline(stream, word, ',')) {
      int srcRectX, srcRectY;
      int tileNum = std::stoi(word);

      if (tileNum >= 20) {
        srcRectY = 64;
      } else if (tileNum >= 10) {
        srcRectY = 32;
      } else {
        srcRectY = 0;
      }

      srcRectX = (tileNum % 10) * 32;

      Entity mapTile = registry->CreateEntity();

      // Scale to 1080p
      mapTile.AddComponent<TransformComponent>(glm::vec2(x * 2.56, y * 1.78),
                                               glm::vec2(2.6, 1.8), 0.0);
      mapTile.AddComponent<SpriteComponent>("jungle-map", 32, 32, 0, srcRectX,
                                            srcRectY);
      x = x + 32;
      if (x >= 800) {
        y = y + 32;
        x = 0;
      }
    }
  }
}

void Game::LoadLevel(int levelNumber) {
  // Add the systems that need to be processed in our game
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<RenderColliderSystem>();

  // Adding assets to the assetStore
  assetStore->AddTexture(renderer, "tank-image",
                         "./assets/images/tank-tiger-left.png");
  assetStore->AddTexture(renderer, "truck-image",
                         "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");

  // Load texture from ./assets/tilemaps/jungle.png
  assetStore->AddTexture(renderer, "jungle-map",
                         "./assets/tilemaps/jungle.png");

  ReadMapFile("./assets/tilemaps/jungle.map");

  // Create an entity
  Entity chopper = registry->CreateEntity();
  chopper.AddComponent<TransformComponent>(glm::vec2(500.0, 100.0),
                                           glm::vec2(1.0, 1.0), 0.0);
  chopper.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
  chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
  chopper.AddComponent<AnimationComponent>(2, 10, true);

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 80, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
  radar.AddComponent<AnimationComponent>(8, 10, true);

  Entity truck = registry->CreateEntity();
  truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0.0));
  truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
  truck.AddComponent<BoxColliderComponent>(32, 32);

  Entity tank = registry->CreateEntity();
  tank.AddComponent<TransformComponent>(glm::vec2(300.0, 10.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
  tank.AddComponent<BoxColliderComponent>(32, 32);
}

void Game::Setup() { LoadLevel(1); }

void Game::Update() {
  // If we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
  int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - msPreviousFrame);
  if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME) {
    SDL_Delay(timeToWait);
  }

  // The difference in ticks since the last frame, converted to seconds
  double deltaTime = (SDL_GetTicks() - msPreviousFrame) / 1000.0;

  // Store the "previous" frame time
  msPreviousFrame = SDL_GetTicks();

  // Update the registry to process the entities that are waiting to be
  // created/deleted
  registry->Update();

  // Ask all the systems to update
  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CollisionSystem>().Update();
}

void Game::Render() {
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);

  registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
  if (isDebug) {
    registry->GetSystem<RenderColliderSystem>().Update(renderer);
  }

  SDL_RenderPresent(renderer);
}

void Game::Run() {
  Setup();
  while (isRunning) {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::Destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
