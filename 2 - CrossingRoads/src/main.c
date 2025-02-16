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
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include "Engine/StateManager.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "States.h"
#include <stdlib.h>
#include <time.h>

#define WINDOW_DEFAULT_WIDTH 640
#define WINDOW_DEFAULT_HEIGHT 480
#define STATEMANAGER_CAPACITY 3

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  Uint64 lastFrameEndNS;
  double targetTickTimeNS;
  double fps;
  double frameTime;

  StateManager *stateManager;
} AppState;

void setTargetTick(AppState *state, Uint64 FPS) {
  state->targetTickTimeNS = SDL_NS_PER_SECOND * 1. / FPS;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_SetAppMetadata("Crossing Roads", "1.0", "com.gaetanstaquet.crossing");

  srand(time(NULL));

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(
        SDL_LOG_CATEGORY_SYSTEM, "Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if(!TTF_Init()) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "Couldn't initialize TTF: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppState *state = SDL_malloc(sizeof(AppState));
  state->lastFrameEndNS = 0;
  setTargetTick(state, 60);
  state->stateManager = StateManager_Create(STATEMANAGER_CAPACITY);
  StateManager_Push(state->stateManager, createStartState());
  *appstate = state;

  state->window = SDL_CreateWindow("Crossing Roads",
                                   WINDOW_DEFAULT_WIDTH,
                                   WINDOW_DEFAULT_HEIGHT,
                                   SDL_WINDOW_OPENGL);
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
  SDL_Color black = {0, 0, 0, SDL_ALPHA_OPAQUE};
  SDL_Color white = {255, 255, 255, SDL_ALPHA_OPAQUE};

  SDL_SetRenderDrawColor(state->renderer,
                         black.r,
                         black.g,
                         black.b,
                         black.a);

  SDL_RenderClear(state->renderer);

  StateManager_Render(state->stateManager, state->renderer);
  SDL_SetRenderDrawColor(state->renderer, white.r, white.g, white.b, white.a);
  SDL_RenderDebugTextFormat(state->renderer, 0, 0, "FPS: %f (%fms)", state->fps, state->frameTime);

  SDL_RenderPresent(state->renderer);
}

void physicsStep(AppState *state, Uint64 deltaNS) {
  StateManager_Update(state->stateManager, SDL_NS_TO_MS((double)deltaNS));
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
    state->fps = (SDL_NS_PER_SECOND * 1. / elapsedSinceLast);
    state->frameTime = frameTime * 1. / SDL_NS_PER_MS;
    state->lastFrameEndNS = endFrame;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  } else {
    StateManager_ProcessEvent(((AppState *)appstate)->stateManager, event);
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = appstate;
  StateManager_Free(state->stateManager);
  SDL_free(state);
}
