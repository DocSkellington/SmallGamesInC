/* Small game engine in C.
  Copyright (C) 2025 Gaëtan Staquet <gaetan.staquet@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "Engine/StateManager.h"

#define EMPTY_STACK -1

State *State_Create() {
  State *state = SDL_malloc(sizeof(State));
  state->memory = nullptr;
  state->init = nullptr;
  state->destroy = nullptr;
  state->update = nullptr;
  state->isTransparent = nullptr;
  state->render = nullptr;
  state->processEvent = nullptr;
  return state;
}

void *State_GetMemory(State *state) {
  return state->memory;
}

void State_SetInit(State *state,
                   void (*init)(void **memory, StateManager *manager)) {
  state->init = init;
}

void State_SetDestroy(State *state, void (*destroy)(void *memory)) {
  state->destroy = destroy;
}

void State_SetUpdate(State *state,
                     bool (*update)(void *memory,
                                    Uint64 delta,
                                    StateManager *manager)) {
  state->update = update;
}

void State_SetIsTransparent(State *state,
                            bool (*isTransparent)(const void *memory)) {
  state->isTransparent = isTransparent;
}

void State_SetRender(State *state,
                     void (*render)(void *memory, SDL_Renderer *renderer)) {
  state->render = render;
}

void State_SetProcessEvent(State *state,
                           bool (*process)(void *memory,
                                           SDL_Event *event,
                                           StateManager *manager)) {
  state->processEvent = process;
}

StateManager *StateManager_Create(unsigned int capacity,
                                  SDL_Window *window,
                                  Options *options) {
  if (0 == capacity) {
    return nullptr;
  }
  StateManager managerInit = {window,
                              options,
                              SDL_calloc(capacity, sizeof(State *)),
                              capacity,
                              EMPTY_STACK};
  StateManager *manager = SDL_malloc(sizeof(StateManager));
  SDL_memcpy(manager, &managerInit, sizeof(StateManager));
  return manager;
}

void StateManager_Free(StateManager *manager) {
  while (manager->top != EMPTY_STACK) {
    StateManager_Pop(manager);
  }
  SDL_free(manager->states);
  SDL_free(manager);
}

int StateManager_Push(StateManager *manager, State *state) {
  if (state == nullptr) {
    return STATEMANAGER_STATE_NULL;
  }
  if (manager->top + 1 == manager->capacity) {
    return STATEMANAGER_FULL;
  }

  manager->states[++manager->top] = state;

  if (state->init == nullptr) {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                "A state does not have an init function");
  } else {
    state->init(&state->memory, manager);
  }

  return STATEMANAGER_OK;
}

int StateManager_Pop(StateManager *manager) {
  if (manager->top == EMPTY_STACK) {
    return STATEMANAGER_EMPTY;
  }

  State *state = manager->states[manager->top];
  if (state->destroy == nullptr) {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                "A state does not have a destroy function");
  } else {
    state->destroy(state->memory);
  }
  SDL_free(state);

  manager->states[manager->top--] = nullptr;

  return STATEMANAGER_OK;
}

void StateManager_Update(StateManager *manager, float delta) {
  int current = manager->top;
  bool cont = true;
  while (current != EMPTY_STACK && cont) {
    State *state = manager->states[current];
    if (state->update == nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                  "A state does not have an update function");
      cont = false;
    } else {
      cont = state->update(state->memory, delta, manager);
    }
    current--;
  }
}

void StateManager_Render(const StateManager *manager, SDL_Renderer *renderer) {
  int current = manager->top;
  bool cont = true;
  // Seek deepest state to render in the stack
  while (current != EMPTY_STACK && cont) {
    const State *state = manager->states[current];
    if (state->isTransparent == nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                  "A state does not have a isTransparent function");
      cont = false;
    } else {
      cont = state->isTransparent(state->memory);
    }
    current--;
  }
  current++;

  // Render from the found state
  while (current != manager->top + 1) {
    State *state = manager->states[current];
    if (state->render == nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                  "A state does not have a render function");
    } else {
      state->render(state->memory, renderer);
    }
    current++;
  }
}

void StateManager_ProcessEvent(StateManager *manager, SDL_Event *event) {
  int current = manager->top;
  bool cont = true;
  while (current != EMPTY_STACK && cont) {
    State *state = manager->states[current];
    if (state->processEvent == nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                  "A state does not have a process event function");
      cont = false;
    } else {
      cont = state->processEvent(state->memory, event, manager);
    }
    current--;
  }
}
