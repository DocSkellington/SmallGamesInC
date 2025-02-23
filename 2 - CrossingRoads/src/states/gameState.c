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
#include "Entities.h"
#include "States.h"

typedef struct {
  Entity *player;
} Memory;

static void init(void **m, StateManager *) {
  Memory *memory = SDL_malloc(sizeof(Memory));
  memory->player = createPlayerEntity();
  *m = memory;
}

static void destroy(void *m) {
  Memory *memory = m;
  freeEntity(memory->player);
  SDL_free(m);
}

static bool update(void *m, Uint64 deltaMS, StateManager *) {
  Memory *memory = m;
  updateEntity(memory->player, deltaMS);
  return false;
}

static void render(void *m, SDL_Renderer *renderer) {
  Memory *memory = m;
  renderEntity(memory->player, renderer);
}

static bool processEvent(void *m, SDL_Event *event, StateManager *manager) {
  Memory *memory = m;
  Bindings *bindings = Options_GetBindings(manager->options);
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (Bindings_Matches(bindings, ACTION_MOVE_FORWARD, event->key.scancode)) {
      Player_move(memory->player, UP);
    } else if (Bindings_Matches(
                   bindings, ACTION_MOVE_BACKWARD, event->key.scancode)) {
      Player_move(memory->player, DOWN);
    } else if (Bindings_Matches(
                   bindings, ACTION_MOVE_LEFT, event->key.scancode)) {
      Player_move(memory->player, LEFT);
    } else if (Bindings_Matches(
                   bindings, ACTION_MOVE_RIGHT, event->key.scancode)) {
      Player_move(memory->player, RIGHT);
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
