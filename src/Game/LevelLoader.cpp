#include "LevelLoader.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Game/Game.h"
#include <fstream>
#include <glm/glm.hpp>

LevelLoader::LevelLoader() {}

LevelLoader::~LevelLoader() {}

void LevelLoader::LoadLevel(const std::unique_ptr<Registry>& registry,
                            const std::unique_ptr<AssetStore>& assetStore,
                            SDL_Renderer* renderer, int level) {
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
  Game::mapWidth = mapNumCols * tileSize * tileScale;
  Game::mapHeight = mapNumRows * tileSize * tileScale;

  // Create an entity
  Entity chopper = registry->CreateEntity();
  chopper.Tag("player");
  chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0),
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
  radar.AddComponent<TransformComponent>(
      glm::vec2(Game::windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
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
