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
#include "Engine/StateManager.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "States.h"

typedef struct Memory Memory;

struct Memory {
  unsigned int selection;
  SDL_Color unselectedColor, selectedColor;
  TTF_Font *font;

  struct {
    SDL_Surface **surfaces;
    SDL_Texture **textures;
    void (**callbacks)(Memory *, StateManager *);
    unsigned int size;
  } texts;
};

void on_start(Memory *memory, StateManager *manager) {
  SDL_Log("Start not implemented");
}

void on_options(Memory *memory, StateManager *manager) {
  SDL_Log("Options not implemented");
}

void on_exit(Memory *memory, StateManager *manager) {
  SDL_Event quitEvent;
  SDL_zero(quitEvent);
  quitEvent.type = SDL_EVENT_QUIT;
  quitEvent.quit.timestamp = SDL_GetTicksNS();
  SDL_PushEvent(&quitEvent); // SDL copies the event
}

void init(void **memory, StateManager *) {
  Memory *m = SDL_malloc(sizeof(Memory));
  m->selection = 0;
  m->font = TTF_OpenFont("resources/freefont-ttf/sfd/FreeSerif.ttf", 32);
  if (m->font == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                 "Impossible to load font file: %s",
                 SDL_GetError());
  }
  m->selectedColor.r = 255;
  m->selectedColor.g = 255;
  m->selectedColor.b = 0;
  m->selectedColor.a = SDL_ALPHA_OPAQUE;
  SDL_Color white = {255, 255, 255, SDL_ALPHA_OPAQUE};
  m->unselectedColor = white;

  m->texts.size = 3;
  m->texts.surfaces = SDL_calloc(m->texts.size, sizeof(SDL_Surface *));
  m->texts.textures = SDL_calloc(m->texts.size, sizeof(SDL_Texture *));
  m->texts.callbacks = SDL_calloc(m->texts.size, sizeof(void (*)()));
  m->texts.surfaces[0] = TTF_RenderText_Blended(m->font, "Start", 0, white);
  m->texts.textures[0] = nullptr;
  m->texts.callbacks[0] = on_start;
  m->texts.surfaces[1] = TTF_RenderText_Blended(m->font, "Options", 0, white);
  m->texts.textures[1] = nullptr;
  m->texts.callbacks[1] = on_options;
  m->texts.surfaces[2] = TTF_RenderText_Blended(m->font, "Exit", 0, white);
  m->texts.textures[2] = nullptr;
  m->texts.callbacks[2] = on_exit;

  *memory = m;
}

void destroy(void *memory) {
  Memory *m = memory;
  for (unsigned int i = 0; i < m->texts.size; i++) {
    SDL_DestroySurface(m->texts.surfaces[i]);
    if (m->texts.textures[i] != nullptr) {
      SDL_DestroyTexture(m->texts.textures[i]);
    }
  }
  SDL_free(m->texts.callbacks);
  SDL_free(m->texts.surfaces);
  TTF_CloseFont(m->font);
  SDL_free(memory);
}

void render(void *memory, SDL_Renderer *renderer) {
  const Memory *m = memory;

  for (unsigned int i = 0; i < m->texts.size; i++) {
    if (m->texts.textures[i] == nullptr) {
      m->texts.textures[i] =
          SDL_CreateTextureFromSurface(renderer, m->texts.surfaces[i]);
    }

    SDL_Texture *texture = m->texts.textures[i];
    int w = 0, h = 0;
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_FRect dst;
    SDL_GetTextureSize(texture, &dst.w, &dst.h);
    dst.x = (w - dst.w) / 2;
    dst.y = (h - dst.h) / 2 + (i * dst.h) * 3. / 4;

    if (m->selection == i) {
      SDL_SetTextureColorMod(
          texture, m->selectedColor.r, m->selectedColor.g, m->selectedColor.b);
    } else {
      SDL_SetTextureColorMod(texture,
                             m->unselectedColor.r,
                             m->unselectedColor.g,
                             m->unselectedColor.b);
    }
    SDL_RenderTexture(renderer, texture, NULL, &dst);
  }
}

bool processEvent(void *memory, SDL_Event *event, StateManager *manager) {
  Memory *m = memory;
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_DOWN &&
        m->selection + 1 < m->texts.size) {
      m->selection++;
    } else if (event->key.scancode == SDL_SCANCODE_UP && m->selection > 0) {
      m->selection--;
    } else if (event->key.scancode == SDL_SCANCODE_SPACE ||
               event->key.scancode == SDL_SCANCODE_KP_ENTER ||
               event->key.scancode == SDL_SCANCODE_RETURN) {
      if (m->texts.callbacks[m->selection] != nullptr) {
        m->texts.callbacks[m->selection](m, manager);
      }
    }
  }
  return false;
}

State *createStartState() {
  State *state = State_Create();
  State_SetInit(state, init);
  State_SetDestroy(state, destroy);
  State_SetRender(state, render);
  State_SetProcessEvent(state, processEvent);
  return state;
}
