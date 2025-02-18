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

typedef struct StateManager StateManager;

typedef struct State State;

/**
 * A state of the game.
 *
 * \since This struct is available since Engine 1.0.0.
 *
 * \sa State_Create to instantiate a new state.
 * \sa StateManager for the manager of states.
 */
struct State {
  /**
   * The memory of the state.
   *
   * It can be anything and is managed by the state itself.
   */
  void *memory;

  /**
   * The initialization function of the state.
   *
   * Set a value to <code>*memory</code> to initialize the memory of this state,
   * which will then be passed to the other functions.
   * Initially, <code>*memory</code> is <code>nullptr</code>.
   *
   * \param memory A pointer to a pointer.
   * \param manager The state manager that is calling the function.
   *
   * \sa State_SetInit
   */
  void (*init)(void **memory, StateManager *manager);

  /**
   * The destruction function of the state.
   *
   * If the memory has been initialized, it should be freed by this function.
   *
   * \warning Do not attempt to free the state itself. It will be done by the
   * state manager.
   *
   * \param memory The pointer to the memory of the state.
   *
   * \sa State_SetDestroy
   */
  void (*destroy)(void *memory);

  /**
   * The update function of the state.
   *
   * \param memory The memory of this state.
   * \param delta The number of milliseconds since the start of the previous frame.
   * \param manager The state manager that called this function.
   * \return <code>true</code> to let the state manager call the update function
   * of the next state, or <code>false</code> to not let it.
   *
   * \sa State_SetUpdate
   */
  bool (*update)(void *memory, Uint64 delta, StateManager *manager);

  /**
   * Determines whether the next state in the manager should be drawn before
   * this state.
   *
   * If the function returns <code>false</code>, this state is rendered first,
   * then the manager go upwards in its stack.
   *
   * \param memory The memory of this state.
   * \return <code>true</code> to let the manager render the next state before
   * this one, or <code>false</code> to render this state now.
   *
   * \sa State_SetIsTransparent
   */
  bool (*isTransparent)(const void *memory);

  /**
   * The render function of the state.

   * \param memory The memory of this state.
   * \param renderer The SDL renderer to use.
   *
   * \sa State_SetRender
   */
  void (*render)(void *memory, SDL_Renderer *renderer);

  /**
   * The function to process an event.
   *
   * \param memory The memory of this state.
   * \param event The event to process.
   * \param manager The state manager that called this function.
   * \return <code>true</code> to let the state manager call the process event
   * function of the next state, or <code>false</code> to not let it.
   *
   * \sa State_SetProcessEvent
   */
  bool (*processEvent)(void *memory, SDL_Event *event, StateManager *manager);
};

State *State_Create();
void *State_GetMemory(State *state);
void State_SetInit(State *state,
                   void (*init)(void **memory, StateManager *manager));
void State_SetDestroy(State *state, void (*destroy)(void *memory));
void State_SetUpdate(State *state,
                     bool (*update)(void *memory,
                                    Uint64 delta,
                                    StateManager *manager));
void State_SetIsTransparent(State *state,
                            bool (*isTransparent)(const void *memory));
void State_SetRender(State *state,
                     void (*render)(void *memory, SDL_Renderer *renderer));
void State_SetProcessEvent(State *state,
                           bool (*process)(void *memory,
                                           SDL_Event *event,
                                           StateManager *manager));

/**
 * Manages the states of the game.
 *
 * It is expected that there are no duplicated states inside the manager.
 * If a state appears multiple times, a double free error WILL occur.
 *
 * \since This struct is available since Engine 1.0.0.
 *
 * \sa StateManager_Create, StateManager_Free, StateManager_Push,
 * StateManager_Pop, StateManager_Update, StateManager_Render, and
 * StateManager_ProcessEvent to manipulate the manager.
 */
struct StateManager {
  /**
   * The main window of this application.
   */
  SDL_Window *mainWindow;
  /**
   * The stack of states of the manager.
   */
  State **states;
  /**
   * How many states this manager can handle.
   */
  const int capacity;
  /**
   * The index of the top element in the stack.
   */
  int top;
};

StateManager *StateManager_Create(unsigned int capacity, SDL_Window *window);
void StateManager_Free(StateManager *manager);
int StateManager_Push(StateManager *manager, State *state);
int StateManager_Pop(StateManager *manager);
void StateManager_Update(StateManager *manager, float delta);
void StateManager_Render(const StateManager *manager, SDL_Renderer *renderer);
void StateManager_ProcessEvent(StateManager *manager, SDL_Event *event);
