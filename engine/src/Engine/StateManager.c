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
#include "SDL3/SDL_log.h"
#include <bits/posix2_lim.h>

#define EMPTY_STACK -1

State *State_Create() {
  State *state = SDL_malloc(sizeof(State));
  return state;
}

void State_SetInit(State *state, void (*init)(void **memory)) {
  state->init = init;
}

void State_SetDestroy(State *state, void (*destroy)(void *memory)) {
  state->destroy = destroy;
}

void State_SetUpdate(State *state, bool (*update)(void *memory, float delta)) {
  state->update = update;
}

void State_SetRender(State *state,
                     bool (*render)(const void *memory,
                                    SDL_Renderer *renderer)) {
  state->render = render;
}

void State_SetProcessEvent(State *state,
                           bool (*process)(void *memory,
                                           const SDL_Event *event)) {
  state->processEvent = process;
}

State *StateManager_Top(StateManager *manager) {
  return manager->states[manager->top];
}

StateManager *StateManager_Create() {
  StateManager *manager = SDL_malloc(sizeof(StateManager));
  manager->capacity = 3;
  manager->states = SDL_calloc(manager->capacity, sizeof(State *));
  manager->top = EMPTY_STACK;
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
  if (manager->top + 1 == manager->capacity) {
    return STATEMANAGER_FULL;
  }

  manager->top++;
  manager->states[manager->top] = state;

  if (state->init != nullptr) {
    state->init(&state->memory);
  }

  return STATEMANAGER_OK;
}

int StateManager_Pop(StateManager *manager) {
  if (manager->top == EMPTY_STACK) {
    return STATEMANAGER_EMPTY;
  }

  State *state = StateManager_Top(manager);
  if (state->destroy != nullptr) {
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                "A state does not have a destroy function");
  } else {
    state->destroy(state->memory);
  }
  SDL_free(state);

  manager->states[manager->top] = nullptr;
  manager->top--;

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
      cont = state->update(state->memory, delta);
    }
    current--;
  }
}

void StateManager_Render(const StateManager *manager, SDL_Renderer *renderer) {
  int current = manager->top;
  bool cont = true;
  while (current != EMPTY_STACK && cont) {
    State *state = manager->states[current];
    if (state->render == nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                  "A state does not have a render function");
      cont = false;
    } else {
      cont = state->render(state->memory, renderer);
    }
    current--;
  }
}

void StateManager_ProcessEvent(StateManager *manager, const SDL_Event *event) {
  int current = manager->top;
  bool cont = true;
  while (current != EMPTY_STACK && cont) {
    State *state = manager->states[current];
    if (state->processEvent == nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                  "A state does not have a process event function");
      cont = false;
    } else {
      cont = state->processEvent(state->memory, event);
    }
    current--;
  }
}
