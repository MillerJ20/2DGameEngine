#ifndef KEYPRESSSYSTEM_H
#define KEYPRESSSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyDownEvent.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_keyboard.h>
#include <string>

class KeypressSystem : public System {
public:
  KeypressSystem() = default;

  void SubscribeToEvents(std::unique_ptr<EventManager>& eventManager) {
    eventManager->SubscribeToEvent<KeyDownEvent>(this,
                                                 &KeypressSystem::onKeypress);
  }

  void onKeypress(KeyDownEvent& event) {
    Logger::Log("Key pressed: " + event.keyCode);
  }
};

#endif // !KEYPRESSSYSTEM_H
