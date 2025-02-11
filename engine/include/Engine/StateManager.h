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
#define STATEMANAGER_STATE_NULL 3

/**
 * Manages the states of the game.
 *
 * It is expected that there are no duplicated states inside the manager.
 * If a state appears multiple times, a double free error WILL occur.
 *
 * \since This struct is available since Engine 1.0.0.
 */
typedef struct StateManager StateManager;

/**
 * A state of the game.
 *
 * \since This struct is available since Engine 1.0.0.
 *
 * \sa State_Create
 */
typedef struct State State;

struct State {
  void *memory; /**< The memory of the state. It can be anything and is managed
                   by the state itself. */
  void (*init)(
      void **memory,
      StateManager *manager); /**< The initialization function of the state. */
  void (*destroy)(void *memory); /**< The destroying function of the state. */
  bool (*update)(
      void *memory,
      float delta,
      StateManager *manager); /**< The function that is called to let the state
                                 updates. The delta parameter is the number of
                                 seconds since previous call. */
  bool (*render)(const void *memory,
                 SDL_Renderer *renderer); /**< The function that is called to
                                             let the state renders. */
  bool (*processEvent)(void *memory,
                       const SDL_Event *event,
                       StateManager *manager); /**< The function to let the
                                                  state process an event. */
};

State *State_Create();
void State_SetInit(State *state,
                   void (*init)(void **memory, StateManager *manager));
void State_SetDestroy(State *state, void (*destroy)(void *memory));
void State_SetUpdate(State *state,
                     bool (*update)(void *memory,
                                    float delta,
                                    StateManager *manager));
void State_SetRender(State *state,
                     bool (*render)(const void *memory,
                                    SDL_Renderer *renderer));
void State_SetProcessEvent(State *state,
                           bool (*process)(void *memory,
                                           const SDL_Event *event,
                                           StateManager *manager));

struct StateManager {
  State **states;
  int capacity;
  int top;
};

StateManager *StateManager_Create(unsigned int capacity);
void StateManager_Free(StateManager *manager);
int StateManager_Push(StateManager *manager, State *state);
int StateManager_Pop(StateManager *manager);
void StateManager_Update(StateManager *manager, float delta);
void StateManager_Render(const StateManager *manager, SDL_Renderer *renderer);
void StateManager_ProcessEvent(StateManager *manager, const SDL_Event *event);
