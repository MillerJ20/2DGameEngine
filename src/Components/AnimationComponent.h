#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <SDL2/SDL.h>

struct AnimationComponent {
  int numFrames;
  int currentFrame;
  int frameSpeedRate;
  bool isLoop;
  int startTime;

  AnimationComponent(int numFrames = 1, int frameSpeedRate = 1,
                     bool shouldLoop = true) {
    this->numFrames = numFrames;
    this->currentFrame = 1;
    this->frameSpeedRate = frameSpeedRate;
    this->isLoop = shouldLoop;
    this->startTime = SDL_GetTicks();
  };
};
#endif // !ANIMATION_COMPONENT_H
