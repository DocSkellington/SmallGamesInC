#include "Objects.h"
#include "SDL3/SDL.h"
#include <stdlib.h>

#define GRAVITY (9.8 * 30)
#define MAX_VELOCITY_BIRD 200
#define BIRD_FLAP_VELOCITY -120
#define BIRD_POSITION_X 50
#define BIRD_SIZE 20

#define NUMBER_PIPES 3
#define PIPE_WIDTH 50

void initBird(Bird **bird) {
  Bird *b = SDL_malloc(sizeof(Bird));
  b->positionY = 200;
  b->velocityY = 5;
  *bird = b;
}

void resetPipe(Pipe *pipe, const GameState *state) {
  pipe->position.x = 640 + rand() % 100; // TODO: window size
  pipe->position.y = 0;
  pipe->gap = 70 + rand() % 100;
  pipe->size.x = PIPE_WIDTH;
  pipe->size.y = 10 + rand() % (state->groundY - pipe->gap - 10);
}

void initPipes(Pipe **pipes, const GameState *state) {
  Pipe *p = SDL_malloc(sizeof(Pipe) * NUMBER_PIPES);
  for (int i = 0; i < NUMBER_PIPES; i++) {
    resetPipe(&p[i], state);
    p[i].position.x = 500 + i * (PIPE_WIDTH + state->gapPipes);
  }
  *pipes = p;
}

void Game_Init(GameState **state) {
  GameState *g = SDL_malloc(sizeof(GameState));
  g->lost = false;
  g->groundY =
      400; // TODO: position of ground must depend on the size of the window
  g->speedPipes = 100;
  g->gapPipes = 200;
  initBird(&g->bird);
  initPipes(&g->pipes, g);

  *state = g;
}

void Game_Free(GameState *state) {
  SDL_free(state->bird);
  SDL_free(state->pipes);
  SDL_free(state);
}

void pipeToRects(const Pipe *pipe, SDL_FRect *pipeAbove, SDL_FRect *pipeBelow) {
  pipeAbove->x = pipe->position.x;
  pipeAbove->y = 0;
  pipeAbove->w = pipe->size.x;
  pipeAbove->h = pipe->size.y;

  pipeBelow->x = pipe->position.x;
  pipeBelow->y = pipe->size.y + pipe->gap;
  pipeBelow->w = pipe->size.x;
  pipeBelow->h = 600; // TODO: height must depend on the size of the window
}

void renderFillRect(const SDL_FRect *rect,
                    const SDL_Color *color,
                    SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
  SDL_RenderFillRect(renderer, rect);
}

void renderGround(float groundY,
                  const SDL_Palette *palette,
                  SDL_Renderer *renderer) {
  SDL_Color col = palette->colors[GROUND];
  SDL_FRect rect = {0, groundY, 800, 600}; // TODO: rectangle fills window
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
                SDL_Renderer *renderer) {
  SDL_Color col = palette->colors[PIPE];
  SDL_FRect pipeAbove, pipeBelow;
  pipeToRects(pipe, &pipeAbove, &pipeBelow);

  renderFillRect(&pipeAbove, &col, renderer);
  renderFillRect(&pipeBelow, &col, renderer);
}

void Game_Render(const GameState *state,
                 const SDL_Palette *palette,
                 SDL_Renderer *renderer) {
  renderGround(state->groundY, palette, renderer);
  renderBird(state->bird, palette, renderer);
  for (int i = 0; i < NUMBER_PIPES; i++) {
    renderPipe(&state->pipes[i], palette, renderer);
  }
}

bool hasOverlap(const SDL_FRect *rectangle1, const SDL_FRect *rectangle2) {
  return !(rectangle1->x + rectangle1->w < rectangle2->x ||
           rectangle2->x + rectangle2->w < rectangle1->x ||
           rectangle1->y + rectangle1->h < rectangle2->y ||
           rectangle2->y + rectangle2->h < rectangle1->y);
}

bool hasCollisionWithPipe(Bird *bird, Pipe *pipe) {
  SDL_FRect b = {BIRD_POSITION_X, bird->positionY, BIRD_SIZE, BIRD_SIZE};
  SDL_FRect pipeAbove, pipeBelow;
  pipeToRects(pipe, &pipeAbove, &pipeBelow);

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
    if (hasCollisionWithPipe(bird, &state->pipes[i])) {
      return true;
    }
  }
  return false;
}

bool updateBird(Bird *bird, GameState *state, float delta) {
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

  if (pipe->position.x + pipe->size.x < 0) {
    resetPipe(pipe, state);
  }
}

void Game_Update(GameState *state, float delta) {
  if (state->lost) {
    return;
  }

  if (updateBird(state->bird, state, delta)) {
    state->lost = true;
  }
  for (int i = 0; i < NUMBER_PIPES; i++) {
    updatePipe(&state->pipes[i], state, delta);
  }
}

void Game_Event(GameState *state, const SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_UP) {
    if (event->key.scancode == SDL_SCANCODE_SPACE ||
        event->key.scancode == SDL_SCANCODE_UP ||
        event->key.scancode == SDL_SCANCODE_W) {
      state->bird->velocityY = BIRD_FLAP_VELOCITY;
    }
  }
}
