#ifndef KEYDOWNEVENT_H
#define KEYDOWNEVENT_H

#include "Event.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <string>

class KeyDownEvent : public Event {
public:
  std::string keyCode;

  KeyDownEvent(SDL_Keycode keyCode) { this->keyCode = SDL_GetKeyName(keyCode); }
};

#endif // !KEYDOWNEVENT_H
