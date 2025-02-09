#include "Objects.h"
#include "SDL3/SDL.h"

#define GRAVITY (9.8 * 30)

void initGround(Object **ground) {
  Object *g = SDL_malloc(sizeof(Object));
  g->type = GROUND;
  g->rectangle.x = 0;
  g->rectangle.y = 400;
  g->rectangle.w = 640;
  g->rectangle.h = 100;
  g->velocityX = 0;
  g->velocityY = 0;
  *ground = g;
}

void initBird(Object **bird) {
  Object *b = SDL_malloc(sizeof(Object));
  b->type = BIRD;
  b->rectangle.x = 50;
  b->rectangle.y = 200;
  b->rectangle.w = 20;
  b->rectangle.h = 20;
  b->velocityX = 0;
  b->velocityY = 5;
  *bird = b;
}

void Game_Init(GameState **state) {
  GameState *g = SDL_malloc(sizeof(GameState));
  initGround(&g->ground);
  initBird(&g->bird);

  *state = g;
}

void Game_Free(GameState *state) {
  SDL_free(state->ground);
  SDL_free(state->bird);
  SDL_free(state);
}

void render_object(const Object *object,
                   const SDL_Palette *palette,
                   SDL_Renderer *renderer) {
  SDL_Color col = palette->colors[object->type];
  SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

  SDL_RenderFillRect(renderer, &object->rectangle);
}

void Game_Render(const GameState *state,
                 const SDL_Palette *palette,
                 SDL_Renderer *renderer) {
  render_object(state->ground, palette, renderer);
  render_object(state->bird, palette, renderer);
}

void update_bird(Object *bird, float delta) {
  bird->rectangle.y += bird->velocityY * delta;

  bird->velocityY += GRAVITY * delta;
}

void update_object(Object *object, float delta) {
  switch (object->type) {
  case BIRD:
    update_bird(object, delta);
  case BACKGROUND:
  case PIPE:
  case GROUND:
    break;
  }
}

void Game_Update(GameState *state, float delta) {
  update_object(state->bird, delta);
}
