#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H

#include "../Components/KeyboardControlledComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyDownEvent.h"

class KeyboardControlSystem : public System {
public:
  KeyboardControlSystem() {
    RequireComponent<KeyboardControlledComponent>();
    RequireComponent<SpriteComponent>();
    RequireComponent<RigidBodyComponent>();
  }

  void SubscribeToEvents(std::unique_ptr<EventManager>& eventManager) {
    eventManager->SubscribeToEvent<KeyDownEvent>(
        this, &KeyboardControlSystem::OnKeyPressed);
  }

  void OnKeyPressed(KeyDownEvent& event) {
    for (auto entity : GetSystemEntities()) {
      const auto keyboardcontrol =
          entity.GetComponent<KeyboardControlledComponent>();
      auto& sprite = entity.GetComponent<SpriteComponent>();
      auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

      switch (event.keyCode) {
      case SDLK_UP:
        rigidbody.velocity = keyboardcontrol.upVelocity;
        sprite.srcRect.y = sprite.height * 0;
        break;
      case SDLK_RIGHT:
        rigidbody.velocity = keyboardcontrol.rightVelocity;
        sprite.srcRect.y = sprite.height * 1;
        break;
      case SDLK_DOWN:
        rigidbody.velocity = keyboardcontrol.downVelocity;
        sprite.srcRect.y = sprite.height * 2;
        break;
      case SDLK_LEFT:
        rigidbody.velocity = keyboardcontrol.leftVelocity;
        sprite.srcRect.y = sprite.height * 3;
        break;
      }
    }
  }

  void Update() {}
};

#endif
