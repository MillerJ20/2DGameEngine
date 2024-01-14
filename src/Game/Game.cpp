#include "Game.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyDownEvent.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/CircleCollisionSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderCircleColliderSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/RenderHealthSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "glm/fwd.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_sdl.h>
#include <memory>
#include <sstream>
#include <string>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
  isRunning = false;
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();
  eventManager = std::make_unique<EventManager>();
  Logger::Log("Game constructor called!");
}

Game::~Game() { Logger::Log("Game destructor called!"); }

void Game::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err("Error initializing SDL.");
    return;
  }

  if (TTF_Init() != 0) {
    Logger::Err("Error initializing SDL TTF.");
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

  // Initialize imgui context
  ImGui::CreateContext();
  ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

  // Initialize the camera
  camera.x = 0;
  camera.y = 0;
  camera.w = windowWidth;
  camera.h = windowHeight;

  isRunning = true;
}

void Game::ProcessInput() {
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent)) {
    // ImGui SDL Events
    ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
    ImGuiIO& io = ImGui::GetIO();

    int mouseX, mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

    io.MousePos = ImVec2(mouseX, mouseY);
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

    // Core SDL Events
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
      eventManager->EmitEvent<KeyDownEvent>(sdlEvent.key.keysym.sym);
      break;
    }
  }
}

// This is far from perfect and does not provide dynamic resolution scaling
// TODO: Make this perfect and provide dynamic resolution scaling
// TODO: Allow this to change/rerender based on settings change
void Game::ReadMapFile(std::string filePath) {
  // TODO: Ensure the file type is of the proper .map (csv) format that is
  // expected

  std::fstream file;

  // TODO: Does this assign the string to the "file" var?
  file.open(filePath.c_str());

  std::string word, row;
  double x = 0, y = 0;

  // TODO: Find a better way to loop through this csv and build tiles, this is
  // so bad
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

      // Scale to 1080p (THE DUMB AND UGLY WAY)
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

  // Add the sytems that need to be processed in our game
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderCircleColliderSystem>();
  registry->AddSystem<RenderSystem>();
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<CircleCollisionSystem>();
  registry->AddSystem<RenderColliderSystem>();
  registry->AddSystem<DamageSystem>();
  registry->AddSystem<KeyboardControlSystem>();
  registry->AddSystem<CameraMovementSystem>();
  registry->AddSystem<ProjectileEmitSystem>();
  registry->AddSystem<ProjectileLifecycleSystem>();
  registry->AddSystem<RenderTextSystem>();
  registry->AddSystem<RenderHealthSystem>();

  // Adding assets to the asset store
  assetStore->AddTexture(renderer, "tank-image",
                         "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image",
                         "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper-spritesheet.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
  assetStore->AddTexture(renderer, "bullet-image",
                         "./assets/images/bullet.png");
  assetStore->AddTexture(renderer, "tilemap-image",
                         "./assets/tilemaps/jungle.png");
  assetStore->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 14);

  // Load the tilemap
  int tileSize = 32;
  double tileScale = 2.5;
  int mapNumCols = 25;
  int mapNumRows = 20;

  std::fstream mapFile;
  mapFile.open("./assets/tilemaps/jungle.map");

  for (int y = 0; y < mapNumRows; y++) {
    for (int x = 0; x < mapNumCols; x++) {
      char ch;
      mapFile.get(ch);
      int srcRectY = std::atoi(&ch) * tileSize;
      mapFile.get(ch);
      int srcRectX = std::atoi(&ch) * tileSize;
      mapFile.ignore();

      Entity tile = registry->CreateEntity();
      tile.Group("tiles");
      tile.AddComponent<TransformComponent>(
          glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
          glm::vec2(tileScale, tileScale), 0.0);
      tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0,
                                         false, srcRectX, srcRectY);
    }
  }
  mapFile.close();
  mapWidth = mapNumCols * tileSize * tileScale;
  mapHeight = mapNumRows * tileSize * tileScale;

  // Create an entity
  Entity chopper = registry->CreateEntity();
  chopper.Tag("player");
  chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0),
                                           glm::vec2(1.0, 1.0), 0.0);
  chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
  chopper.AddComponent<AnimationComponent>(2, 15, true);
  chopper.AddComponent<BoxColliderComponent>(32, 32);
  chopper.AddComponent<KeyboardControlledComponent>(
      glm::vec2(0, -200), glm::vec2(200, 0), glm::vec2(0, 200),
      glm::vec2(-200, 0));
  chopper.AddComponent<CameraFollowComponent>();
  chopper.AddComponent<HealthComponent>(100);
  chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(350.0, 350.0), 0,
                                                   10000, 25, true);

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1, true);
  radar.AddComponent<AnimationComponent>(8, 5, true);

  Entity tank = registry->CreateEntity();
  tank.Group("enemies");
  tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
  tank.AddComponent<BoxColliderComponent>(32, 32);
  tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000,
                                                3000, 25, false);
  tank.AddComponent<HealthComponent>(100);

  Entity truck = registry->CreateEntity();
  truck.Group("enemies");
  truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
  truck.AddComponent<BoxColliderComponent>(32, 32);
  truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000,
                                                 3000, 25, false);
  truck.AddComponent<HealthComponent>(100);
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

  eventManager->Reset();

  // Subscribe to events
  registry->GetSystem<DamageSystem>().SubscribeToEvents(eventManager);
  registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventManager);
  registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvent(eventManager);

  // Update the registry to process the entities that are waiting to be
  // created/deleted
  registry->Update();

  // Ask all the systems to update
  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CollisionSystem>().Update(eventManager);
  registry->GetSystem<CircleCollisionSystem>().Update(eventManager);
  registry->GetSystem<CameraMovementSystem>().Update(camera);
  registry->GetSystem<ProjectileEmitSystem>().Update(registry);
  registry->GetSystem<ProjectileLifecycleSystem>().Update();
}

void Game::Render() {
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);

  registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
  registry->GetSystem<RenderHealthSystem>().Update(renderer, assetStore,
                                                   camera);
  registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
  if (isDebug) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
    registry->GetSystem<RenderCircleColliderSystem>().Update(renderer, camera);
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
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
  ImGuiSDL::Deinitialize();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
