/* Flappy Bird, part of a collection of small games in C.
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

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include "Objects.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Palette *palette;

  Object *ground;
  Object *bird;
} AppState;

void init_palette(SDL_Palette **palette) {
  SDL_Color background;
  background.r = 170;
  background.g = 85;
  background.b = 30;
  background.a = SDL_ALPHA_OPAQUE;

  SDL_Color bird;
  bird.r = 255;
  bird.g = 240;
  bird.b = 0;
  bird.a = SDL_ALPHA_OPAQUE;

  SDL_Color pipe;
  pipe.r = 40;
  pipe.g = 54;
  pipe.b = 24;
  pipe.a = SDL_ALPHA_OPAQUE;

  SDL_Color ground;
  ground.r = 96;
  ground.g = 108;
  ground.b = 56;
  ground.a = SDL_ALPHA_OPAQUE;

  SDL_Color colors[4];
  colors[BACKGROUND] = background;
  colors[BIRD] = bird;
  colors[PIPE] = pipe;
  colors[GROUND] = ground;

  *palette = SDL_CreatePalette(4);
  SDL_SetPaletteColors(*palette, colors, 0, 4);
}

void init_ground(Object **ground) {
  Object *g = SDL_malloc(sizeof(Object));
  g->type = GROUND;
  g->rectangle.x = 0;
  g->rectangle.y = 400;
  g->rectangle.w = 640;
  g->rectangle.h = 100;
  g->speedX = 0;
  g->speedY = 0;
  *ground = g;
}

void init_bird(Object **bird) {
  Object *b = SDL_malloc(sizeof(Object));
  b->type = BIRD;
  b->rectangle.x = 50;
  b->rectangle.y = 200;
  b->rectangle.w = 20;
  b->rectangle.h = 20;
  b->speedX = 0;
  b->speedY = 0;
  *bird = b;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_SetAppMetadata("Flappy Bird", "0.1", "com.gaetanstaquet.flappybird");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(
        SDL_LOG_CATEGORY_SYSTEM, "Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppState *state = SDL_malloc(sizeof(AppState));
  init_palette(&state->palette);
  init_ground(&state->ground);
  init_bird(&state->bird);
  *appstate = state;

  state->window =
      SDL_CreateWindow("Flappy Bird", 640, 480, SDL_WINDOW_RESIZABLE);
  if (state->window == nullptr) {
    SDL_LogCritical(
        SDL_LOG_CATEGORY_SYSTEM, "Couldn't create window: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->renderer = SDL_CreateRenderer(state->window, "vulkan,opengl");
  if (state->renderer == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM,
                    "Couldn't create renderer: %s",
                    SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = appstate;

  SDL_Color backgroundColor = state->palette->colors[BACKGROUND];

  if (!SDL_SetRenderDrawColor(state->renderer,
                              backgroundColor.r,
                              backgroundColor.g,
                              backgroundColor.b,
                              backgroundColor.a)) {
    SDL_LogError(
        SDL_LOG_CATEGORY_RENDER, "Couldn't render color: %s", SDL_GetError());
  }

  SDL_RenderClear(state->renderer);

  render_object(state->ground, state->palette, state->renderer);
  render_object(state->bird, state->palette, state->renderer);

  SDL_RenderPresent(state->renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = appstate;
  SDL_DestroyPalette(state->palette);
  SDL_free(state->ground);
  SDL_free(state->bird);
  SDL_free(state);
}
