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

typedef enum {
  SIZE_640x480 = 0,
  SIZE_800x600,
  SIZE_1600x900,
} WindowSizes;

typedef struct Memory Memory;

typedef struct {
  SDL_Texture *texture;
  void (*callback)(Memory *, StateManager *);

  struct {
    SDL_Texture **possibilities;
    unsigned int selection;
    unsigned int size;
  } possibilities;
} Text;

struct Memory {
  unsigned int selection;
  SDL_Color unselectedColor, selectedColor;
  TTF_Font *font;

  Text *texts;
  unsigned int size;
};

static void onApply(Memory *memory, StateManager *manager) {
  switch (memory->texts[0].possibilities.selection) {
  case SIZE_640x480:
    SDL_SetWindowSize(manager->mainWindow, 640, 480);
    break;
  case SIZE_800x600:
    SDL_SetWindowSize(manager->mainWindow, 800, 600);
    break;
  case SIZE_1600x900:
    SDL_SetWindowSize(manager->mainWindow, 1600, 900);
    break;
  }
  SDL_SetWindowPosition(
      manager->mainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

static void onExit(Memory *, StateManager *manager) {
  StateManager_Pop(manager);
}

static SDL_Texture *createTextureText(SDL_Renderer *renderer,
                                      TTF_Font *font,
                                      const char *string,
                                      SDL_Color color) {
  SDL_Surface *surface = TTF_RenderText_Blended(font, string, 0, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);
  return texture;
}

static void init(void **memory, StateManager *manager) {
  SDL_Renderer *renderer = SDL_GetRenderer(manager->mainWindow);
  Memory *m = SDL_malloc(sizeof(Memory));
  m->selection = 0;
  m->font = TTF_OpenFont("resources/freefont-ttf/sfd/FreeSerif.ttf", 32);
  if (m->font == nullptr) {
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

  m->size = 3;
  m->texts = SDL_calloc(m->size, sizeof(Text));

  m->texts[0].texture =
      createTextureText(renderer, m->font, "Window size", white);
  m->texts[0].callback = nullptr;
  m->texts[0].possibilities.selection = 0;
  m->texts[0].possibilities.size = 3;
  m->texts[0].possibilities.possibilities =
      SDL_calloc(m->texts[0].possibilities.size, sizeof(SDL_Texture *));
  m->texts[0].possibilities.possibilities[SIZE_640x480] =
      createTextureText(renderer, m->font, "640x480", white);
  m->texts[0].possibilities.possibilities[SIZE_800x600] =
      createTextureText(renderer, m->font, "800x600", white);
  m->texts[0].possibilities.possibilities[SIZE_1600x900] =
      createTextureText(renderer, m->font, "1600x900", white);

  m->texts[1].texture = createTextureText(renderer, m->font, "Apply", white);
  m->texts[1].callback = onApply;
  m->texts[1].possibilities.selection = 0;
  m->texts[1].possibilities.possibilities = nullptr;

  m->texts[2].texture = createTextureText(renderer, m->font, "Exit", white);
  m->texts[2].callback = onExit;
  m->texts[2].possibilities.selection = 0;
  m->texts[2].possibilities.possibilities = nullptr;

  *memory = m;
}

static void destroy(void *memory) {
  Memory *m = memory;
  for (unsigned int i = 0; i < m->size; i++) {
    SDL_DestroyTexture(m->texts[i].texture);

    SDL_Texture **possibilities = m->texts[i].possibilities.possibilities;
    if (possibilities != nullptr) {
      for (unsigned int j = 0; j < m->texts[i].possibilities.size; j++) {
        SDL_DestroyTexture(possibilities[j]);
      }
      SDL_free(possibilities);
    }
  }
  SDL_free(m->texts);
  TTF_CloseFont(m->font);
  SDL_free(memory);
}

static void render(void *memory, SDL_Renderer *renderer) {
  const Memory *m = memory;

  for (unsigned int i = 0; i < m->size; i++) {
    SDL_Texture *texture = m->texts[i].texture;
    int w = 0, h = 0;
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_FRect dst;
    SDL_GetTextureSize(texture, &dst.w, &dst.h);
    dst.x = (w - dst.w) / 2;
    dst.y = (h - dst.h) / 2 + (i * 60) * 3. / 4;

    if (m->selection == i) {
      SDL_SetTextureColorMod(
          texture, m->selectedColor.r, m->selectedColor.g, m->selectedColor.b);
    } else {
      SDL_SetTextureColorMod(texture,
                             m->unselectedColor.r,
                             m->unselectedColor.g,
                             m->unselectedColor.b);
    }
    SDL_RenderTexture(renderer, texture, nullptr, &dst);

    for (unsigned int j = 0; j < m->texts[i].possibilities.size; j++) {
      texture = m->texts[i].possibilities.possibilities[j];
      SDL_FRect posDst;
      SDL_GetTextureSize(texture, &posDst.w, &posDst.h);
      posDst.x = dst.x + dst.w + 60;
      posDst.y = dst.y;

      if (m->texts[i].possibilities.selection == j) {
        if (m->selection == i) {
          SDL_SetTextureColorMod(texture,
                                 m->selectedColor.r,
                                 m->selectedColor.g,
                                 m->selectedColor.b);
        } else {
          SDL_SetTextureColorMod(texture,
                                 m->unselectedColor.r,
                                 m->unselectedColor.g,
                                 m->unselectedColor.b);
        }
        SDL_RenderTexture(renderer, texture, nullptr, &posDst);
      }
    }
  }
}

static bool
processEvent(void *memory, SDL_Event *event, StateManager *manager) {
  Memory *m = memory;
  const Bindings *bindings = Options_GetBindings(manager->options);
  unsigned int current = m->selection;

  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (Bindings_Matches(bindings, ACTION_MENU_DOWN, event->key.scancode) &&
        current + 1 < m->size) {
      m->selection++;
    } else if (Bindings_Matches(
                   bindings, ACTION_MENU_UP, event->key.scancode) &&
               current > 0) {
      m->selection--;
    } else if (Bindings_Matches(
                   bindings, ACTION_MENU_LEFT, event->key.scancode) &&
               m->texts[current].possibilities.selection > 0) {
      m->texts[current].possibilities.selection--;
    } else if (Bindings_Matches(
                   bindings, ACTION_MENU_RIGHT, event->key.scancode) &&
               m->texts[current].possibilities.selection + 1 <
                   m->texts[current].possibilities.size) {
      m->texts[current].possibilities.selection++;
    } else if (Bindings_Matches(
                   bindings, ACTION_MENU_OK, event->key.scancode)) {
      if (m->texts[m->selection].callback != nullptr) {
        m->texts[m->selection].callback(m, manager);
      }
    }
  }
  return false;
}

State *createOptionsState() {
  State *state = State_Create();
  State_SetInit(state, init);
  State_SetDestroy(state, destroy);
  State_SetRender(state, render);
  State_SetProcessEvent(state, processEvent);
  return state;
}
