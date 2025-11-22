#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System {
public:
  DamageSystem() { RequireComponent<BoxColliderComponent>(); };

  void SubscribeToEvents(std::unique_ptr<EventManager>& eventManager) {
    eventManager->SubscribeToEvent<CollisionEvent>(this,
                                                   &DamageSystem::onCollision);
  };

  void onCollision(CollisionEvent& event) {
    Entity a = event.a;
    Entity b = event.b;

    if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
      OnProjectileHitPlayer(a, b);
    }

    if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
      OnProjectileHitPlayer(b, a);
    }

    if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
      OnProjectileHitEnemy(a, b);
    }

    if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
      OnProjectileHitEnemy(b, a);
    }
  }

  void OnProjectileHitPlayer(Entity projectile, Entity player) {
    auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

    if (!projectileComponent.isFriendly &&
        player.HasComponent<HealthComponent>()) {
      Logger::Log(
          "Projectile id: " + std::to_string(projectile.GetId()) +
          " collided with player id: " + std::to_string(player.GetId()));

      auto& healthComponent = player.GetComponent<HealthComponent>();

      healthComponent.healthPercentage -= projectileComponent.hitPercentDamage;

      if (healthComponent.healthPercentage <= 0) {
        player.Kill();
      }

      projectile.Kill();
    }
  }

  void OnProjectileHitEnemy(Entity projectile, Entity enemy) {
    auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

    if (projectileComponent.isFriendly &&
        enemy.HasComponent<HealthComponent>()) {
      Logger::Log("Projectile id: " + std::to_string(projectile.GetId()) +
                  " collided with enemy id: " + std::to_string(enemy.GetId()));

      auto& healthComponent = enemy.GetComponent<HealthComponent>();

      healthComponent.healthPercentage -= projectileComponent.hitPercentDamage;

      if (healthComponent.healthPercentage <= 0) {
        enemy.Kill();
      }

      projectile.Kill();
    }
  }
};

#endif // !DAMAGESYSTEM_H
