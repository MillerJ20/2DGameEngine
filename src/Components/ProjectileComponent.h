#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include <SDL2/SDL_timer.h>

struct ProjectileComponent {
  bool isFriendly;
  int hitPercentDamage;
  unsigned int duration;
  unsigned int startTime;

  ProjectileComponent(bool isFriendly = false, int hitPercentDamage = 0,
                      int duration = 0) {
    this->isFriendly = isFriendly;
    this->hitPercentDamage = hitPercentDamage;
    this->duration = duration;
    this->startTime = SDL_GetTicks();
  }
};

#endif // !PROJECTILECOMPONENT_H
