#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>

class AnimationSystem : public System {
public:
  AnimationSystem() {
    RequireComponent<SpriteComponent>();
    RequireComponent<AnimationComponent>();
  }

  void Update() {
    for (auto entity : GetSystemEntities()) {
      auto& animation = entity.GetComponent<AnimationComponent>();
      auto& sprite = entity.GetComponent<SpriteComponent>();

      // TODO: Change current frame
      animation.currentFrame = ((SDL_GetTicks() - animation.startTime) *
                                animation.frameRateSpeed / 1000) %
                               animation.numFrames;
      // Change srcRect of the sprite
      sprite.srcRect.x = animation.currentFrame * sprite.width;
    }
  }
};

#endif // !ANIMATIONSYSTEM_H
