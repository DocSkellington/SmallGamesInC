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

#include "Direction.h"
#include "Engine/Bindings.h"
#include "Engine/Options.h"
#include "Engine/StateManager.h"
#include "Level.h"
#include "SDL3/SDL_video.h"
#include "States.h"

typedef struct {
  Level *level;
  unsigned int difficulty;
  bool lost;
  bool won;
} Memory;

static Level *setupLevel(unsigned int difficulty, StateManager *manager) {
  SDL_Rect windowSize = {.x = 0, .y = 0, .w = 0, .h = 0};
  SDL_GetWindowSize(manager->mainWindow, &(windowSize.w), &(windowSize.h));
  double speed = difficulty / 3.;
  if (speed > 2) {
    speed = 2;
  }
  unsigned int carLanes, riverLanes;
  bool safeZones = true;
  if (difficulty == 1) {
    carLanes = 3;
    riverLanes = 5;
  } else if (difficulty == 2) {
    carLanes = 5;
    riverLanes = 3;
  } else {
    carLanes = 5;
    riverLanes = 5;
  }
  return createLevel(speed, carLanes, riverLanes, safeZones, &windowSize, SDL_GetRenderer(manager->mainWindow));
}

static void init(void **m, StateManager *manager) {
  Memory *memory = SDL_malloc(sizeof(Memory));
  memory->level = setupLevel(1, manager);
  memory->difficulty = 1;
  memory->lost = false;
  memory->won = false;
  *m = memory;
}

static void destroy(void *m) {
  Memory *memory = m;
  freeLevel(memory->level);
  SDL_free(m);
}

static bool update(void *m, Uint64 deltaMS, StateManager *manager) {
  Memory *memory = m;
  if (memory->lost) {
    freeLevel(memory->level);
    memory->difficulty = 1;
    memory->level = setupLevel(1, manager);
    memory->lost = false;
  } else if (memory->won) {
    freeLevel(memory->level);
    memory->difficulty++;
    memory->level = setupLevel(memory->difficulty, manager);
    memory->won = false;
  } else {
    LevelStatus status = updateLevel(memory->level, deltaMS);
    switch (status) {
    case CONTINUE:
      break;
    case LOST:
      memory->lost = true;
      StateManager_Push(manager, createGameOverState());
      break;
    case WON:
      memory->won = true;
      StateManager_Push(manager, createVictoryState());
      break;
    }
  }
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
    } else if (Bindings_Matches(
                   bindings, ACTION_MENU_BACK, event->key.scancode)) {
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
