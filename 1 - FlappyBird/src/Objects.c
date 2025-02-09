#include "Objects.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_scancode.h"

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
  g->lost = false;

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

bool has_collision(Object *object1, Object *object2) {
  return !(object1->rectangle.x + object1->rectangle.w < object2->rectangle.x ||
           object2->rectangle.x + object2->rectangle.w < object1->rectangle.x ||
           object1->rectangle.y + object1->rectangle.h < object2->rectangle.y ||
           object2->rectangle.y + object2->rectangle.h < object2->rectangle.y);
}

bool update_bird(Object *bird, GameState *state, float delta) {
  bird->rectangle.y += bird->velocityY * delta;
  if (bird->rectangle.y < 0) {
    bird->rectangle.y = 0;
    bird->velocityY = 0;
  }

  bird->velocityY += GRAVITY * delta;

  return has_collision(bird, state->ground);
}

void Game_Update(GameState *state, float delta) {
  if (state->lost) {
    return;
  }

  if (update_bird(state->bird, state, delta)) {
    state->lost = true;
  }
}

void Game_Event(GameState *state, const SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_UP) {
    if (event->key.scancode == SDL_SCANCODE_SPACE ||
        event->key.scancode == SDL_SCANCODE_UP ||
        event->key.scancode == SDL_SCANCODE_W) {
      state->bird->velocityY = -120;
    }
  }
}
