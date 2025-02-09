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
#pragma once

#include "SDL3/SDL.h"

#define STATEMANAGER_OK 0
#define STATEMANAGER_FULL 1
#define STATEMANAGER_EMPTY 2

typedef struct {
  void *memory;
  void (*init)(void **memory);
  void (*destroy)(void *memory);
  bool (*update)(void *memory, float delta);
  bool (*render)(const void *memory, SDL_Renderer *renderer);
  bool (*processEvent)(void *memory, const SDL_Event *event);
} State;

State *State_Create();
void State_SetInit(State *state, void (*init)(void **memory));
void State_SetDestroy(State *state, void (*destroy)(void *memory));
void State_SetUpdate(State *state, bool (*update)(void *memory, float delta));
void State_SetRender(State *state,
                     bool (*render)(const void *memory,
                                    SDL_Renderer *renderer));
void State_SetProcessEvent(State *state,
                           bool (*process)(void *memory,
                                           const SDL_Event *event));

typedef struct {
  State **states;
  int capacity;
  int top;
} StateManager;

StateManager *StateManager_Create();
void StateManager_Free(StateManager *manager);
int StateManager_Push(StateManager *manager, State *state);
int StateManager_Pop(StateManager *manager);
void StateManager_Update(StateManager *manager, float delta);
void StateManager_Render(const StateManager *manager, SDL_Renderer *renderer);
void StateManager_ProcessEvent(StateManager *manager, const SDL_Event *event);
