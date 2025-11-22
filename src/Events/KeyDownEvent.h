#ifndef KEYDOWNEVENT_H
#define KEYDOWNEVENT_H

#include "Event.h"
#include <SDL2/SDL.h>

class KeyDownEvent : public Event {
public:
  SDL_Keycode keyCode;

  KeyDownEvent(SDL_Keycode keyCode) { this->keyCode = keyCode; }
};

#endif // !KEYDOWNEVENT_H
