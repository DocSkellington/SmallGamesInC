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

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_video.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include "Objects.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Palette *palette;

  Uint64 lastFrameEndNS;
  double targetTickTimeNS;

  GameState *gameState;
} AppState;

void setTargetTick(AppState *state, Uint64 FPS) {
  state->targetTickTimeNS = SDL_NS_PER_SECOND * 1. / FPS;
}

void initPalette(SDL_Palette **palette) {
  SDL_Color background = {170, 85, 30, SDL_ALPHA_OPAQUE};
  SDL_Color bird = {255, 240, 0, SDL_ALPHA_OPAQUE};
  SDL_Color pipe = {40, 54, 24, SDL_ALPHA_OPAQUE};
  SDL_Color ground = {96, 108, 56, SDL_ALPHA_OPAQUE};

  SDL_Color colors[4];
  colors[BACKGROUND] = background;
  colors[BIRD] = bird;
  colors[PIPE] = pipe;
  colors[GROUND] = ground;

  *palette = SDL_CreatePalette(4);
  SDL_SetPaletteColors(*palette, colors, 0, 4);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_SetAppMetadata("Flappy Bird", "0.1", "com.gaetanstaquet.flappybird");

  srand(time(NULL));

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(
        SDL_LOG_CATEGORY_SYSTEM, "Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppState *state = SDL_malloc(sizeof(AppState));
  state->lastFrameEndNS = 0;
  setTargetTick(state, 60);
  initPalette(&state->palette);
  Game_Init(&state->gameState);
  *appstate = state;

  state->window =
      SDL_CreateWindow("Flappy Bird", 640, 480, SDL_WINDOW_OPENGL);
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

void drawApp(AppState *state) {
  SDL_Color backgroundColor = state->palette->colors[BACKGROUND];

  SDL_SetRenderDrawColor(state->renderer,
                         backgroundColor.r,
                         backgroundColor.g,
                         backgroundColor.b,
                         backgroundColor.a);

  SDL_RenderClear(state->renderer);

  Game_Render(state->gameState, state->palette, state->renderer);

  SDL_RenderPresent(state->renderer);
}

void physicsStep(AppState *state, Uint64 deltaNS) {
  const float delta = deltaNS * 1. / SDL_NS_PER_SECOND;
  Game_Update(state->gameState, delta);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = appstate;

  auto startFrame = SDL_GetTicksNS();
  auto deltaNS = startFrame - state->lastFrameEndNS;

  if (deltaNS >= state->targetTickTimeNS) {
    physicsStep(state, deltaNS);
    drawApp(state);

    auto endFrame = SDL_GetTicksNS();
    auto frameTime = endFrame - startFrame;
    auto elapsedSinceLast = endFrame - state->lastFrameEndNS;
    double fps = (SDL_NS_PER_SECOND * 1. / elapsedSinceLast);

    SDL_Log("Frame time: %lu ns", frameTime);
    SDL_Log("Time since previous frame: %lu ns", elapsedSinceLast);
    SDL_Log("Current FPS: %f", fps);

    state->lastFrameEndNS = endFrame;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  } else {
    Game_Event(((AppState *)appstate)->gameState, event);
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = appstate;
  SDL_DestroyPalette(state->palette);
  Game_Free(state->gameState);
  SDL_free(state);
}
