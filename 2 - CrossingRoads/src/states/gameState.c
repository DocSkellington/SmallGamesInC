/* Crossing Roads, part of a collection of small games in C.
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

#include "Engine/Bindings.h"
#include "Engine/Options.h"
#include "Engine/StateManager.h"
#include "Direction.h"
#include "Level.h"
#include "SDL3/SDL_video.h"
#include "States.h"

typedef struct {
  Level *level;
} Memory;

static void init(void **m, StateManager *manager) {
  Memory *memory = SDL_malloc(sizeof(Memory));
  SDL_Rect windowSize = {.x = 0, .y = 0, .w = 0, .h = 0};
  SDL_GetWindowSize(manager->mainWindow, &(windowSize.w), &(windowSize.h));
  memory->level = createLevel(1, 3, 5, true, &windowSize, SDL_GetRenderer(manager->mainWindow));
  *m = memory;
}

static void destroy(void *m) {
  Memory *memory = m;
  freeLevel(memory->level);
  SDL_free(m);
}

static bool update(void *m, Uint64 deltaMS, StateManager *) {
  Memory *memory = m;
  updateLevel(memory->level, deltaMS);
  return false;
}

static void render(void *m, SDL_Renderer *renderer) {
  Memory *memory = m;
  renderLevel(memory->level, renderer);
}

static bool processEvent(void *m, SDL_Event *event, StateManager *manager) {
  Memory *memory = m;
  Bindings *bindings = Options_GetBindings(manager->options);
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (Bindings_Matches(bindings, ACTION_MOVE_FORWARD, event->key.scancode)) {
      moveEventLevel(memory->level, UP);
    } else if (Bindings_Matches(
                   bindings, ACTION_MOVE_BACKWARD, event->key.scancode)) {
      moveEventLevel(memory->level, DOWN);
    } else if (Bindings_Matches(
                   bindings, ACTION_MOVE_LEFT, event->key.scancode)) {
      moveEventLevel(memory->level, LEFT);
    } else if (Bindings_Matches(
                   bindings, ACTION_MOVE_RIGHT, event->key.scancode)) {
      moveEventLevel(memory->level, RIGHT);
    }
    else if (Bindings_Matches(bindings, ACTION_MENU_BACK, event->key.scancode)) {
      StateManager_Pop(manager);
      StateManager_Push(manager, createStartState());
    }
  }
  return false;
}

State *createGameState() {
  State *state = State_Create();
  State_SetInit(state, init);
  State_SetDestroy(state, destroy);
  State_SetUpdate(state, update);
  State_SetRender(state, render);
  State_SetProcessEvent(state, processEvent);
  return state;
}
