#ifndef RENDERGUISYSTEM_H
#define RENDERGUISYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <memory>

class RenderGUISystem : public System {
public:
  RenderGUISystem() = default;

  void Update(const std::unique_ptr<Registry>& registry) {
    ImGui::NewFrame();

    if (ImGui::Begin("Spawn Enemies")) {
      static int enemyXPos = 0;
      static int enemyYPos = 0;

      ImGui::InputInt("Enemy X Position", &enemyXPos);
      ImGui::InputInt("Enemy Y Position", &enemyYPos);

      if (ImGui::Button("Create new enemy")) {
        Entity enemy = registry->CreateEntity();
        enemy.Group("enemies");
        enemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos),
                                               glm::vec2(1.0, 1.0), 0.0);
        enemy.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
        enemy.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
        enemy.AddComponent<BoxColliderComponent>(32, 32);
        enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0),
                                                       2000, 3000, 25, false);
        enemy.AddComponent<HealthComponent>(100);
      }
    };

    ImGui::End();

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
  }
};

#endif // !RENDERGUISYSTEM_H
