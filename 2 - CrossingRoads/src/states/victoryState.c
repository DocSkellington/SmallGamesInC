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
#include "Engine/StateManager.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "States.h"

typedef struct Memory Memory;

struct Memory {
  TTF_Font *font;
  SDL_Texture *victory;
  SDL_Texture *instruction;
};

static void init(void **memory, StateManager *manager) {
  SDL_Renderer *renderer = SDL_GetRenderer(manager->mainWindow);
  Memory *m = SDL_malloc(sizeof(Memory));
  m->font = TTF_OpenFont("resources/freefont-ttf/sfd/FreeSerif.ttf", 32);
  if (m->font == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                 "Impossible to load font file: %s",
                 SDL_GetError());
  }
  SDL_Color white = {255, 255, 255, SDL_ALPHA_OPAQUE};

  SDL_Surface *surface = TTF_RenderText_Blended(m->font, "Level finished!", 0, white);
  m->victory = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

  surface = TTF_RenderText_Blended(m->font, "Press SPACE to play next level", 0, white);
  m->instruction = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

  *memory = m;
}

static void destroy(void *memory) {
  Memory *m = memory;
  SDL_DestroyTexture(m->victory);
  SDL_DestroyTexture(m->instruction);
  TTF_CloseFont(m->font);
  SDL_free(memory);
}

static bool isTransparent(const void *) {
  return true;
}

static void render(void *memory, SDL_Renderer *renderer) {
  const Memory *m = memory;

  int w = 0, h = 0;
  SDL_GetRenderOutputSize(renderer, &w, &h);
  SDL_FRect gameOverDst, instructionDst;
  SDL_GetTextureSize(m->victory, &gameOverDst.w, &gameOverDst.h);
  SDL_GetTextureSize(m->instruction, &instructionDst.w, &instructionDst.h);
  gameOverDst.x = (w - gameOverDst.w) / 2;
  gameOverDst.y = (h - gameOverDst.h) / 2 - instructionDst.h / 2;
  instructionDst.x = (w - instructionDst.w) / 2;
  instructionDst.y = (h - instructionDst.h) / 2 + instructionDst.h / 2;

  SDL_RenderTexture(renderer, m->victory, nullptr, &gameOverDst);
  SDL_RenderTexture(renderer, m->instruction, nullptr, &instructionDst);
}

static bool processEvent(void *, SDL_Event *event, StateManager *manager) {
  const Bindings *bindings = Options_GetBindings(manager->options);

  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (Bindings_Matches(bindings, ACTION_MENU_OK, event->key.scancode)) {
      StateManager_Pop(manager);
    }
  }

  return false;
}

State *createVictoryState() {
  State *state = State_Create();
  State_SetInit(state, init);
  State_SetDestroy(state, destroy);
  State_SetIsTransparent(state, isTransparent);
  State_SetRender(state, render);
  State_SetProcessEvent(state, processEvent);
  return state;
}
