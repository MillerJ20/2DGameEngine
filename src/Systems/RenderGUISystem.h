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

  void Update(const std::unique_ptr<Registry>& registry,
              const SDL_Rect& camera) {
    ImGui::NewFrame();

    if (ImGui::Begin("Spawn Enemies")) {
      static int enemyXPos = 0;
      static int enemyYPos = 0;
      static int enemyHealth = 0;
      static int enemyXScale = 0;
      static int enemyYScale = 0;
      static float enemyRotation = 0;
      static float enemyXVel = 0;
      static float enemyYVel = 0;
      static float projectileAngle = 0.0;
      static float projectileSpeed = 100.0;
      static int projectileRepeat = 0;
      static int projectileDuration = 0;
      const char* spriteArray[] = {"tank-image", "truck-image"};
      static int selectedSpriteIndex = 0;

      if (ImGui::CollapsingHeader("Sprite Image",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Combo("Texture Id", &selectedSpriteIndex, spriteArray,
                     IM_ARRAYSIZE(spriteArray));
      }
      ImGui::Spacing();

      if (ImGui::CollapsingHeader("Transform Data",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("Enemy X Position", &enemyXPos);
        ImGui::InputInt("Enemy Y Position", &enemyYPos);
        ImGui::SliderInt("Enemy X Scale", &enemyXScale, 1, 10);
        ImGui::SliderInt("Enemy Y Scale", &enemyYScale, 1, 10);
        ImGui::SliderAngle("Enemy Rotation", &enemyRotation, 0, 360);
      }
      ImGui::Spacing();

      if (ImGui::CollapsingHeader("Velocity Data",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputFloat("Enemy X Velocity", &enemyXVel);
        ImGui::InputFloat("Enemy Y Velocity", &enemyYVel);
      }
      ImGui::Spacing();

      if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderInt("Enemy Health", &enemyHealth, 0, 100);
      }
      // Section to input enemy projectile emitter values
      if (ImGui::CollapsingHeader("Projectile emitter",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderAngle("angle (deg)", &projectileAngle, 0, 360);
        ImGui::SliderFloat("speed (px/sec)", &projectileSpeed, 10, 500);
        ImGui::InputInt("repeat (sec)", &projectileRepeat);
        ImGui::InputInt("duration (sec)", &projectileDuration);
      }
      ImGui::Spacing();

      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();

      if (ImGui::Button("Create new enemy")) {
        Entity enemy = registry->CreateEntity();
        enemy.Group("enemies");
        enemy.AddComponent<TransformComponent>(
            glm::vec2(enemyXPos, enemyYPos),
            glm::vec2(enemyXScale, enemyYScale), enemyRotation);
        enemy.AddComponent<RigidBodyComponent>(glm::vec2(enemyXVel, enemyYVel));
        enemy.AddComponent<SpriteComponent>(spriteArray[selectedSpriteIndex],
                                            32, 32, 2);
        enemy.AddComponent<BoxColliderComponent>(32, 32);
        double projectileVelX = cos(projectileAngle) * projectileSpeed;
        double projectileVelY = sin(projectileAngle) * projectileSpeed;
        enemy.AddComponent<ProjectileEmitterComponent>(
            glm::vec2(projectileVelX, projectileVelY), projectileRepeat * 1000,
            projectileDuration * 100, 10, false);
        enemy.AddComponent<HealthComponent>(enemyHealth);

        enemyXPos = enemyYPos = enemyRotation = projectileAngle = 0;
        enemyXScale = enemyYScale = 1;
        projectileRepeat = projectileDuration = 10;
        projectileSpeed = 100;
        enemyHealth = 0;
      }
    };

    ImGui::End();
    // Display a small overlay window to display the map position using the
    // mouse
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration |
                                   ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoNav;
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.9f);
    if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
      ImGui::Text("Map coordinates (x=%.1f, y=%.1f)",
                  ImGui::GetIO().MousePos.x + camera.x,
                  ImGui::GetIO().MousePos.y + camera.y);
    }
    ImGui::End();

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
  }
};

#endif // !RENDERGUISYSTEM_H
