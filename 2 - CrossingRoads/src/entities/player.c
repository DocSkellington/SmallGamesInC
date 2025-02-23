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

#include "Entities.h"
#include "Level.h"
#include <assert.h>
#include <math.h>

#define ANIMATION_LENGTH 250
#define MOVEMENT_SPEED_X (1. / ANIMATION_LENGTH)
#define MOVEMENT_SPEED_Y (1. / ANIMATION_LENGTH)

typedef enum {
  IDLE = 0,
  MOVING_UP,
  MOVING_DOWN,
  MOVING_LEFT,
  MOVING_RIGHT,
} AnimationType;

typedef struct {
  AnimationType type;
  Uint64 duration;
} Animation;

typedef struct {
  Animation animation;
  SDL_Palette *palette;
  SDL_Texture *texture;
} Memory;

static void cleanup(Entity *entity) {
  Memory *memory = entity->memory;
  SDL_DestroyPalette(memory->palette);
  SDL_DestroyTexture(memory->texture);
  SDL_free(memory);
}

static void update(Entity *entity, Uint64 deltaMS) {
  Memory *memory = entity->memory;

  if (memory->animation.duration >= ANIMATION_LENGTH) {
    entity->position.x = round(entity->position.x);
    entity->position.y = round(entity->position.y);
    memory->animation.type = IDLE;
    memory->animation.duration = 0;
  } else {
    memory->animation.duration += deltaMS;

    switch (memory->animation.type) {
    case IDLE:
      break;
    case MOVING_UP:
      entity->position.y -= deltaMS * MOVEMENT_SPEED_Y;
      break;
    case MOVING_DOWN:
      entity->position.y += deltaMS * MOVEMENT_SPEED_Y;
      break;
    case MOVING_LEFT:
      entity->position.x -= deltaMS * MOVEMENT_SPEED_X;
      break;
    case MOVING_RIGHT:
      entity->position.x += deltaMS * MOVEMENT_SPEED_X;
      break;
    }
  }
}

static SDL_Texture *render(const Entity *entity) {
  const Memory *memory = entity->memory;
  SDL_Color color = memory->palette->colors[memory->animation.type];
  SDL_Surface *surface = nullptr;

  if (SDL_LockTextureToSurface(memory->texture, nullptr, &surface)) {
    const SDL_PixelFormatDetails *formatDetails =
        SDL_GetPixelFormatDetails(surface->format);

    SDL_FillSurfaceRect(surface, nullptr, SDL_MapRGB(formatDetails, nullptr, 0, 0, 0));

    SDL_FillSurfaceRect(surface, nullptr,
      SDL_MapRGBA(formatDetails, nullptr, color.r, color.g, color.b, color.a));

    SDL_UnlockTexture(memory->texture);
  }

  return memory->texture;
}

Entity *createPlayerEntity(Level *level, Position start, SDL_Renderer *renderer) {
  Entity *entity = SDL_malloc(sizeof(Entity));
  entity->memory = SDL_malloc(sizeof(Memory));
  entity->level = level;
  entity->position = start;
  entity->cleanup = cleanup;
  entity->render = render;
  entity->update = update;

  Memory *memory = entity->memory;
  memory->animation.type = IDLE;
  memory->animation.duration = 0;
  memory->palette = SDL_CreatePalette(5);
  memory->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, CELL_WIDTH, CELL_HEIGHT);

  SDL_Color colors[5];
  colors[IDLE].r = 255;
  colors[IDLE].g = 255;
  colors[IDLE].b = 255;
  colors[IDLE].a = SDL_ALPHA_OPAQUE;
  colors[MOVING_UP].r = 255;
  colors[MOVING_UP].g = 0;
  colors[MOVING_UP].b = 0;
  colors[MOVING_UP].a = SDL_ALPHA_OPAQUE;
  colors[MOVING_DOWN].r = 0;
  colors[MOVING_DOWN].g = 255;
  colors[MOVING_DOWN].b = 0;
  colors[MOVING_DOWN].a = SDL_ALPHA_OPAQUE;
  colors[MOVING_LEFT].r = 0;
  colors[MOVING_LEFT].g = 255;
  colors[MOVING_LEFT].b = 255;
  colors[MOVING_LEFT].a = SDL_ALPHA_OPAQUE;
  colors[MOVING_RIGHT].r = 255;
  colors[MOVING_RIGHT].g = 0;
  colors[MOVING_RIGHT].b = 255;
  colors[MOVING_RIGHT].a = SDL_ALPHA_OPAQUE;
  SDL_SetPaletteColors(memory->palette, colors, 0, 5);
  return entity;
}

void Player_move(Entity *entity, Direction direction) {
  assert(entity != nullptr);
  Memory *memory = entity->memory;

  if (memory->animation.type != IDLE) {
    return;
  }

  unsigned int width = getLevelWidth(entity->level);
  unsigned int height = getLevelHeight(entity->level);

  switch (direction) {
  case UP:
    if (entity->position.y > 0) {
      memory->animation.type = MOVING_UP;
    }
    break;
  case DOWN:
    if (entity->position.y + 1 < height) {
      memory->animation.type = MOVING_DOWN;
    }
    break;
  case LEFT:
    if (entity->position.x > 0) {
      memory->animation.type = MOVING_LEFT;
    }
    break;
  case RIGHT:
    if (entity->position.x + 1 < width) {
      memory->animation.type = MOVING_RIGHT;
    }
    break;
  }
  memory->animation.duration = 0;
}
