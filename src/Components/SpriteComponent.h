#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <string>

struct SpriteComponent {
  std::string assetId;
  int width;
  int height;
  int z_index;
  bool isFixed;
  SDL_Rect srcRect;

  SpriteComponent(std::string assetId = "", int width = 0, int height = 0,
                  int z_index = 0, bool isFixed = false, int srcRectX = 0,
                  int srcRectY = 0) {
    this->assetId = assetId;
    this->width = width;
    this->height = height;
    this->isFixed = isFixed;
    this->srcRect = {srcRectX, srcRectY, width, height};
    this->z_index = z_index;
  }
};

#endif // !SPRITECOMPONENT_H
