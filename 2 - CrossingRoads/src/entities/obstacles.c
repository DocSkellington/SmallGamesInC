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

#include "Direction.h"
#include "Entities.h"
#include "Level.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

#define MARGIN 2
#define TIME_CELL 600
#define MOVEMENT_SPEED(speed) (speed * 1. / TIME_CELL)

typedef struct {
  SDL_Texture *texture;
  Direction direction;
  double speed;
} Memory;

static void cleanup(Entity *entity) {
  Memory *memory = entity->memory;
  SDL_DestroyTexture(memory->texture);
  SDL_free(memory);
}

static SDL_Texture *render(const Entity *entity, const Level *) {
  const Memory *memory = entity->memory;
  return memory->texture;
}

static void warp(Entity *entity, const Level *level) {
  Memory *memory = entity->memory;
  unsigned int width = getLevelWidth(level);
  if (memory->direction == LEFT && entity->position.x + entity->size.x <= 0) {
    entity->position.x = width + MARGIN;
  } else if (memory->direction == RIGHT && entity->position.x >= width) {
    entity->position.x = -MARGIN - entity->size.x;
  }
}

static void move(Entity *entity, Direction direction, double distance) {
  switch (direction) {
  case LEFT:
    entity->position.x -= distance;
    break;
  case RIGHT:
    entity->position.x += distance;
    break;
  case UP:
  case DOWN:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Invalid direction for an obstacle");
    break;
  }
}

static void update(Entity *entity, Uint64 deltaMS, Level *level) {
  Memory *memory = entity->memory;
  move(entity, memory->direction, deltaMS * MOVEMENT_SPEED(memory->speed));
  warp(entity, level);
}

inline static Entity *createGeneric(Level *level,
                                    Position start,
                                    Direction direction,
                                    unsigned int size,
                                    double speed,
                                    SDL_Renderer *renderer) {
  Entity *entity = SDL_malloc(sizeof(Entity));
  entity->memory = SDL_malloc(sizeof(Memory));
  entity->position = start;
  entity->size.x = size;
  entity->size.y = 1;
  entity->cleanup = cleanup;
  entity->render = render;
  entity->update = update;

  Memory *memory = entity->memory;
  memory->direction = direction;
  memory->speed = speed;
  memory->texture = SDL_CreateTexture(renderer,
                                      SDL_PIXELFORMAT_RGBA32,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      size * CELL_WIDTH,
                                      CELL_HEIGHT);

  warp(entity, level);
  return entity;
}

inline static void fillTexture(SDL_Texture *texture, SDL_Color *color) {
  SDL_Surface *surface;
  if (SDL_LockTextureToSurface(texture, nullptr, &surface)) {
    const SDL_PixelFormatDetails *formatDetails =
        SDL_GetPixelFormatDetails(surface->format);

    SDL_FillSurfaceRect(
        surface,
        nullptr,
        SDL_MapRGBA(
            formatDetails, nullptr, color->r, color->g, color->b, color->a));

    SDL_UnlockTexture(texture);
  }
}

Entity *createCarEntity(Level *level,
                        Position start,
                        Direction direction,
                        unsigned int size,
                        double speed,
                        SDL_Renderer *renderer) {
  Entity *car = createGeneric(level, start, direction, size, speed, renderer);
  Memory *memory = car->memory;
  SDL_Color color = {.r = 160, .g = 25, .b = 25, .a = SDL_ALPHA_OPAQUE};
  fillTexture(memory->texture, &color);

  return car;
}

Entity *createTurtleEntity(Level *level,
                           Position start,
                           Direction direction,
                           unsigned int size,
                           double speed,
                           SDL_Renderer *renderer) {
  Entity *turtle =
      createGeneric(level, start, direction, size, speed, renderer);
  Memory *memory = turtle->memory;
  SDL_Color color = {.r = 25, .g = 150, .b = 50, .a = SDL_ALPHA_OPAQUE};
  fillTexture(memory->texture, &color);
  return turtle;
}

Entity *createLogEntity(Level *level,
                        Position start,
                        Direction direction,
                        unsigned int size,
                        double speed,
                        SDL_Renderer *renderer) {
  Entity *log = createGeneric(level, start, direction, size, speed, renderer);
  Memory *memory = log->memory;
  SDL_Color color = {.r = 153, .g = 88, .b = 42, .a = SDL_ALPHA_OPAQUE};
  fillTexture(memory->texture, &color);
  return log;
}

void movePlayerWithObstacle(const Entity *obstacle, Entity *player, Uint64 deltaMS) {
  Memory *memory = obstacle->memory;
  move(player, memory->direction, deltaMS * MOVEMENT_SPEED(memory->speed));
}
