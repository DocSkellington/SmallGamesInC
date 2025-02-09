#include "Objects.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"
#include <stdlib.h>

#define GRAVITY (9.8 * 30)
#define MAX_VELOCITY_BIRD 200
#define BIRD_FLAP_VELOCITY -120
#define BIRD_POSITION_X 50
#define BIRD_SIZE 20

#define NUMBER_PIPES 5
#define PIPE_WIDTH 50

void resetBird(Bird *bird) {
  bird->positionY = 200;
  bird->velocityY = BIRD_FLAP_VELOCITY;
}

void initBird(Bird **bird) {
  *bird = SDL_malloc(sizeof(Bird));
  resetBird(*bird);
}

void resetPipe(Pipe *pipe, const GameState *state) {
  float startX = state->windowSize.x;
  for (int i = 0; i < NUMBER_PIPES; i++) {
    if (state->pipes[i].position.x > startX) {
      startX = state->pipes[i].position.x;
    }
  }
  pipe->position.x = startX + state->gapPipes + rand() % 200;
  pipe->position.y = 0;
  pipe->gap = 70 + rand() % 100;
  pipe->size.x = PIPE_WIDTH;
  pipe->size.y = 10 + rand() % (state->groundY - pipe->gap - 10);
  pipe->scored = false;
}

void initPipes(Pipe **pipes, const GameState *state) {
  Pipe *p = SDL_calloc(NUMBER_PIPES, sizeof(Pipe));
  *pipes = p;
  for (int i = 0; i < NUMBER_PIPES; i++) {
    p[i].position.x = state->windowSize.x;
    p[i].position.y = 0;
  }
}

void resetGame(GameState *state) {
  state->running = false;
  state->lost = false;
  state->speedPipes = 100;
  state->gapPipes = 200;
  state->score = 0;
}

void Game_Init(GameState **state) {
  GameState *g = SDL_malloc(sizeof(GameState));
  *state = g;
  g->windowSize.x = WINDOW_DEFAULT_WIDTH;
  g->windowSize.y = WINDOW_DEFAULT_HEIGHT;
  g->groundY = WINDOW_DEFAULT_HEIGHT - 150;
  resetGame(g);
  initBird(&g->bird);
  initPipes(&g->pipes, g);
}

void Game_Free(GameState *state) {
  SDL_free(state->bird);
  SDL_free(state->pipes);
  SDL_free(state);
}

void pipeToRects(const Pipe *pipe,
                 SDL_FRect *pipeAbove,
                 SDL_FRect *pipeBelow,
                 const GameState *state) {
  pipeAbove->x = pipe->position.x;
  pipeAbove->y = 0;
  pipeAbove->w = pipe->size.x;
  pipeAbove->h = pipe->size.y;

  pipeBelow->x = pipe->position.x;
  pipeBelow->y = pipe->size.y + pipe->gap;
  pipeBelow->w = pipe->size.x;
  pipeBelow->h = state->windowSize.y - pipeBelow->y;
}

void renderFillRect(const SDL_FRect *rect,
                    const SDL_Color *color,
                    SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
  SDL_RenderFillRect(renderer, rect);
}

void renderGround(const GameState *state,
                  const SDL_Palette *palette,
                  SDL_Renderer *renderer) {
  SDL_Color col = palette->colors[GROUND];
  SDL_FRect rect = {
      0, state->groundY, state->windowSize.x, state->windowSize.y};
  renderFillRect(&rect, &col, renderer);
}

void renderBird(const Bird *bird,
                const SDL_Palette *palette,
                SDL_Renderer *renderer) {
  SDL_Color col = palette->colors[BIRD];
  SDL_FRect rect = {BIRD_POSITION_X, bird->positionY, BIRD_SIZE, BIRD_SIZE};
  renderFillRect(&rect, &col, renderer);
}

void renderPipe(const Pipe *pipe,
                const SDL_Palette *palette,
                SDL_Renderer *renderer,
                const GameState *state) {
  SDL_Color col = palette->colors[PIPE];
  SDL_FRect pipeAbove, pipeBelow;
  pipeToRects(pipe, &pipeAbove, &pipeBelow, state);

  renderFillRect(&pipeAbove, &col, renderer);
  renderFillRect(&pipeBelow, &col, renderer);
}

void Game_Render(const GameState *state,
                 const SDL_Palette *palette,
                 SDL_Renderer *renderer) {
  renderGround(state, palette, renderer);
  renderBird(state->bird, palette, renderer);
  for (int i = 0; i < NUMBER_PIPES; i++) {
    renderPipe(&state->pipes[i], palette, renderer, state);
  }

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderDebugTextFormat(renderer, 10, 10, "Score: %d", state->score);

  if (state->lost) {
    SDL_RenderDebugText(renderer, 100, 100, "GAME OVER");
  }
  if (!state->running) {
    SDL_RenderDebugText(renderer,
                        state->windowSize.x / 2,
                        state->windowSize.y / 2,
                        "Press space, up, Z, or W.");
  }
}

bool hasOverlap(const SDL_FRect *rectangle1, const SDL_FRect *rectangle2) {
  return !(rectangle1->x + rectangle1->w < rectangle2->x ||
           rectangle2->x + rectangle2->w < rectangle1->x ||
           rectangle1->y + rectangle1->h < rectangle2->y ||
           rectangle2->y + rectangle2->h < rectangle1->y);
}

bool hasCollisionWithPipe(Bird *bird, Pipe *pipe, const GameState *state) {
  SDL_FRect b = {BIRD_POSITION_X, bird->positionY, BIRD_SIZE, BIRD_SIZE};
  SDL_FRect pipeAbove, pipeBelow;
  pipeToRects(pipe, &pipeAbove, &pipeBelow, state);

  return hasOverlap(&b, &pipeAbove) || hasOverlap(&b, &pipeBelow);
}

bool hasCollisionWithGround(Bird *bird, float groundY) {
  return (bird->positionY + BIRD_SIZE >= groundY);
}

bool birdHasCollision(Bird *bird, GameState *state) {
  if (hasCollisionWithGround(bird, state->groundY)) {
    return true;
  }
  for (int i = 0; i < NUMBER_PIPES; i++) {
    if (hasCollisionWithPipe(bird, &state->pipes[i], state)) {
      return true;
    }
  }
  return false;
}

bool updateBird(GameState *state, float delta) {
  Bird *bird = state->bird;
  bird->positionY += bird->velocityY * delta;
  if (bird->positionY < 0) {
    bird->positionY = 0;
    bird->velocityY = 0;
  }

  bird->velocityY += GRAVITY * delta;
  if (bird->velocityY > MAX_VELOCITY_BIRD) {
    bird->velocityY = MAX_VELOCITY_BIRD;
  }

  return birdHasCollision(bird, state);
}

void updatePipe(Pipe *pipe, GameState *state, float delta) {
  pipe->position.x -= state->speedPipes * delta;

  if (pipe->position.x + pipe->size.x < BIRD_POSITION_X && !pipe->scored) {
    pipe->scored = true;
    state->score++;
  }

  if (pipe->position.x + pipe->size.x < 0) {
    resetPipe(pipe, state);
  }
}

void Game_Update(GameState *state, float delta) {
  if (state->lost || !state->running) {
    return;
  }

  int oldScore = state->score;

  bool end = updateBird(state, delta);
  for (int i = 0; i < NUMBER_PIPES; i++) {
    updatePipe(&state->pipes[i], state, delta);
  }

  if (state->score % 5 == 0 && oldScore != state->score) {
    state->speedPipes += 5;
    state->gapPipes -= 20;
    if (state->gapPipes < 50) {
      state->gapPipes = 50;
    }
  }

  if (end) {
    state->lost = true;
    state->running = false;
  }
}

void Game_Event(GameState *state, const SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_UP) {
    if (event->key.scancode == SDL_SCANCODE_SPACE ||
        event->key.scancode == SDL_SCANCODE_UP ||
        event->key.scancode == SDL_SCANCODE_W) {
      if (state->running) {
        state->bird->velocityY = BIRD_FLAP_VELOCITY;
      } else {
        resetBird(state->bird);
        resetGame(state);
        for (int i = 0; i < NUMBER_PIPES; i++) {
          state->pipes[i].position.x = state->windowSize.x;
        }
        for (int i = 0; i < NUMBER_PIPES; i++) {
          resetPipe(&state->pipes[i], state);
        }
        state->running = true;
      }
    }
  } else if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    state->windowSize.x = event->window.data1;
    state->windowSize.y = event->window.data2;
  }
}
