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

#include "Level.h"
#include "Engine/Pair.h"
#include "Entities.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_stdinc.h"
#include <math.h>

#define COLUMNS 15
#define MAX_CARS_PER_LANE 5
#define MAX_TURTLES_PER_LANE 5
#define MAX_LOGS_PER_LANE 5
#define ENTITY_MARGIN_X (2. / CELL_WIDTH)
#define ENTITY_MARGIN_Y (2. / CELL_HEIGHT)

enum InPalette {
  OUTSIDE = 0,
  SAFE,
  CAR_LANE,
  RIVER_LANE,
  TARGET,
  SIZE_IN_PALETTE,
};

typedef struct {
  Entity **obstacles;
  unsigned int size;
} Obstacles;

struct Level {
  Entity *player;
  Obstacles cars;
  Obstacles logs;
  Obstacles turtles;
  unsigned int speed;
  unsigned int carLanes;
  unsigned int riverLanes;
  bool safeZones;
  SDL_Rect boundaries;
  SDL_Rect windowSize;
  SDL_Palette *palette;
};

static void gridToGlobalPosition(const Level *level,
                                 const Position *grid,
                                 float *x,
                                 float *y) {
  *x = grid->x * CELL_WIDTH + level->boundaries.x;
  *y = grid->y * CELL_HEIGHT + level->boundaries.y;
}

static void createObstacles(Level *level, SDL_Renderer *renderer) {
  unsigned int nCars = MAX_CARS_PER_LANE * level->carLanes;
  level->cars.size = nCars;
  level->cars.obstacles = SDL_malloc(nCars * sizeof(Entity *));
  for (unsigned int i = 0; i < nCars; i++) {
    level->cars.obstacles[i] = nullptr;
  }

  unsigned int nTurtles = MAX_TURTLES_PER_LANE * level->riverLanes;
  level->turtles.size = nTurtles;
  level->turtles.obstacles = SDL_malloc(nTurtles * sizeof(Entity *));
  for (unsigned int i = 0; i < nTurtles; i++) {
    level->turtles.obstacles[i] = nullptr;
  }

  unsigned int nLogs = MAX_LOGS_PER_LANE * level->riverLanes;
  level->logs.size = nLogs;
  level->logs.obstacles = SDL_malloc(nLogs * sizeof(Entity *));
  for (unsigned int i = 0; i < nLogs; i++) {
    level->logs.obstacles[i] = nullptr;
  }

  for (unsigned int lane = 0; lane < level->carLanes; lane++) {
    double speed = level->speed;
    unsigned int size = 2;
    Direction direction;
    unsigned int gap;

    if (lane % 2 == 0) {
      direction = RIGHT;
      gap = 3;
    } else {
      direction = LEFT;
      speed *= 2;
      gap = 5;
    }

    for (unsigned int car = 0; car < 4 - (2 * (lane % 2)); car++) {
      Position start = {.x = (size + gap) * car + (lane % 3),
                        .y = 1 + level->riverLanes + 1 + level->carLanes -
                             lane - 1};
      level->cars.obstacles[MAX_CARS_PER_LANE * lane + car] =
          createCarEntity(level, start, direction, size, speed, renderer);
    }
  }

  for (unsigned int lane = 0; lane < level->riverLanes; lane++) {
    double speed = level->speed;
    unsigned int size = 3;
    Direction direction;
    unsigned int gap;

    if (lane % 3 == 0) { // Turtles
      if (lane % 2 == 0) {
        direction = RIGHT;
        gap = 4;
      } else {
        direction = LEFT;
        speed *= 1.5;
        gap = 5;
      }

      for (unsigned int turtle = 0; turtle < 3; turtle++) {
        Position start = {.x = (size + gap) * turtle + 2 * (lane % 4),
                          .y = 1 + level->riverLanes - lane - 1};
        level->turtles.obstacles[MAX_TURTLES_PER_LANE * lane + turtle] =
            createTurtleEntity(level, start, direction, size, speed, renderer);
      }
    } else { // Logs
      if (lane % 2 == 1) {
        size = 5;
        direction = LEFT;
        gap = 2;
      } else {
        size = 3;
        direction = RIGHT;
        speed *= 2;
        gap = 3;
      }

      for (unsigned int log = 0; log < 3; log++) {
        Position start = {.x = (size + gap) * log + (lane % 4),
                          .y = 1 + level->riverLanes - lane - 1};
        level->logs.obstacles[MAX_LOGS_PER_LANE * lane + log] =
            createLogEntity(level, start, direction, size, speed, renderer);
      }
    }
  }
}

Level *createLevel(unsigned int speed,
                   unsigned int carLanes,
                   unsigned int riverLanes,
                   bool safeZones,
                   SDL_Rect *windowSize,
                   SDL_Renderer *renderer) {
  Level *level = SDL_malloc(sizeof(Level));
  level->speed = speed;
  level->carLanes = carLanes;
  level->riverLanes = riverLanes;
  level->safeZones = safeZones;
  level->windowSize = *windowSize;
  unsigned int nLines = getLevelHeight(level);

  level->boundaries.w = COLUMNS * CELL_WIDTH;
  level->boundaries.h = nLines * CELL_HEIGHT;
  level->boundaries.x = (windowSize->w - level->boundaries.w) / 2.;
  level->boundaries.y = (windowSize->h - level->boundaries.h) / 2.;

  level->palette = SDL_CreatePalette(SIZE_IN_PALETTE);
  SDL_Color colors[SIZE_IN_PALETTE];
  colors[OUTSIDE].r = 120;
  colors[OUTSIDE].g = 10;
  colors[OUTSIDE].b = 10;
  colors[OUTSIDE].a = SDL_ALPHA_OPAQUE;
  colors[SAFE].r = 10;
  colors[SAFE].g = 120;
  colors[SAFE].b = 10;
  colors[SAFE].a = SDL_ALPHA_OPAQUE;
  colors[CAR_LANE].r = 30;
  colors[CAR_LANE].g = 30;
  colors[CAR_LANE].b = 30;
  colors[CAR_LANE].a = SDL_ALPHA_OPAQUE;
  colors[RIVER_LANE].r = 10;
  colors[RIVER_LANE].g = 10;
  colors[RIVER_LANE].b = 120;
  colors[RIVER_LANE].a = SDL_ALPHA_OPAQUE;
  colors[TARGET].r = 120;
  colors[TARGET].g = 120;
  colors[TARGET].b = 10;
  colors[TARGET].a = SDL_ALPHA_OPAQUE;

  SDL_SetPaletteColors(level->palette, colors, 0, SIZE_IN_PALETTE);

  Position start = {.x = floor(COLUMNS / 2.), .y = nLines - 1};
  level->player = createPlayerEntity(level, start, renderer);

  createObstacles(level, renderer);

  return level;
}

void freeLevel(Level *level) {
  freeEntity(level->player);

  for (unsigned int i = 0; i < level->cars.size; i++) {
    if (level->cars.obstacles[i] != nullptr) {
      freeEntity(level->cars.obstacles[i]);
    }
  }
  SDL_free(level->cars.obstacles);

  for (unsigned int i = 0; i < level->turtles.size; i++) {
    if (level->turtles.obstacles[i] != nullptr) {
      freeEntity(level->turtles.obstacles[i]);
    }
  }
  SDL_free(level->turtles.obstacles);

  for (unsigned int i = 0; i < level->logs.size; i++) {
    if (level->logs.obstacles[i] != nullptr) {
      freeEntity(level->logs.obstacles[i]);
    }
  }
  SDL_free(level->logs.obstacles);

  SDL_DestroyPalette(level->palette);
  SDL_free(level);
}

static void
updateObstacles(Level *level, Uint64 deltaMS, Obstacles *obstacles) {
  for (unsigned int i = 0; i < obstacles->size; i++) {
    if (obstacles->obstacles[i] != nullptr) {
      updateEntity(obstacles->obstacles[i], deltaMS, level);
    }
  }
}

static bool isHitByCar(const Level *level) {
  const Entity *player = level->player;
  for (unsigned int i = 0; i < level->cars.size; i++) {
    const Entity *car = level->cars.obstacles[i];
    if (car != nullptr) {
      SDL_FRect playerRect = {.x = player->position.x + ENTITY_MARGIN_X,
                              .y = player->position.y + ENTITY_MARGIN_Y,
                              .w = player->size.x - ENTITY_MARGIN_X,
                              .h = player->size.y - ENTITY_MARGIN_Y};
      SDL_FRect carRect = {.x = car->position.x + ENTITY_MARGIN_X,
                           .y = car->position.y + ENTITY_MARGIN_Y,
                           .w = car->size.x - ENTITY_MARGIN_X,
                           .h = car->size.y - ENTITY_MARGIN_Y};
      SDL_FRect intersection;
      if (SDL_GetRectIntersectionFloat(&playerRect, &carRect, &intersection)) {
        return true;
      }
    }
  }
  return false;
}

static bool isInWaterOrMoveWithObstacle(Level *level, Uint64 deltaMS) {
  Entity *player = level->player;
  if (isPlayerJumping(player) || 1 > player->position.y || floor(player->position.y) >= level->riverLanes + 1) {
    return false;
  }

  for (unsigned int i = 0; i < level->logs.size; i++) {
    const Entity *log = level->logs.obstacles[i];
    if (log != nullptr) {
      SDL_FRect playerRect = {.x = player->position.x + ENTITY_MARGIN_X,
                              .y = player->position.y + ENTITY_MARGIN_Y,
                              .w = player->size.x - ENTITY_MARGIN_X,
                              .h = player->size.y - ENTITY_MARGIN_Y};
      SDL_FRect logRect = {.x = log->position.x + ENTITY_MARGIN_X,
                           .y = log->position.y + ENTITY_MARGIN_Y,
                           .w = log->size.x - ENTITY_MARGIN_X,
                           .h = log->size.y - ENTITY_MARGIN_Y};
      SDL_FRect intersection;
      if (SDL_GetRectIntersectionFloat(&playerRect, &logRect, &intersection)) {
        movePlayerWithObstacle(log, player, deltaMS);
        return false;
      }
    }
  }
  for (unsigned int i = 0; i < level->turtles.size; i++) {
    const Entity *turtle = level->turtles.obstacles[i];
    if (turtle != nullptr) {
      SDL_FRect playerRect = {.x = player->position.x + ENTITY_MARGIN_X,
                              .y = player->position.y + ENTITY_MARGIN_Y,
                              .w = player->size.x - ENTITY_MARGIN_X,
                              .h = player->size.y - ENTITY_MARGIN_Y};
      SDL_FRect turtleRect = {.x = turtle->position.x + ENTITY_MARGIN_X,
                           .y = turtle->position.y + ENTITY_MARGIN_Y,
                           .w = turtle->size.x - ENTITY_MARGIN_X,
                           .h = turtle->size.y - ENTITY_MARGIN_Y};
      SDL_FRect intersection;
      if (SDL_GetRectIntersectionFloat(&playerRect, &turtleRect, &intersection)) {
        movePlayerWithObstacle(turtle, player, deltaMS);
        return false;
      }
    }
  }
  return true;
}

bool updateLevel(Level *level, Uint64 deltaMS) {
  updateObstacles(level, deltaMS, &level->cars);
  updateObstacles(level, deltaMS, &level->turtles);
  updateObstacles(level, deltaMS, &level->logs);
  updateEntity(level->player, deltaMS, level);

  if (isHitByCar(level) || isInWaterOrMoveWithObstacle(level, deltaMS)) {
    return true;
  }

  return false;
}

inline static void renderOutside(const Level *level, SDL_Renderer *renderer) {
  SDL_Color color = level->palette->colors[OUTSIDE];
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_FRect rect = {
      .x = 0, .y = 0, .w = level->boundaries.x, .h = level->windowSize.h};
  SDL_RenderFillRect(renderer, &rect);

  rect.x = level->boundaries.x + level->boundaries.w;
  SDL_RenderFillRect(renderer, &rect);

  rect.x = level->boundaries.x;
  rect.y = 0;
  rect.w = level->boundaries.w;
  rect.h = level->boundaries.y;
  SDL_RenderFillRect(renderer, &rect);

  rect.x = level->boundaries.x;
  rect.y = level->boundaries.y + level->boundaries.h;
  rect.w = level->boundaries.w;
  rect.h = level->boundaries.y;
  SDL_RenderFillRect(renderer, &rect);
}

inline static void renderSafeLanes(const Level *level, SDL_Renderer *renderer) {
  SDL_Color color = level->palette->colors[SAFE];
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  SDL_FRect rect = {.x = level->boundaries.x,
                    .y = level->boundaries.y +
                         (1 + level->riverLanes) * CELL_HEIGHT,
                    .w = level->boundaries.w,
                    .h = CELL_HEIGHT};
  SDL_RenderFillRect(renderer, &rect);

  rect.y = level->boundaries.y +
           (1 + level->riverLanes + 1 + level->carLanes) * CELL_HEIGHT;
  SDL_RenderFillRect(renderer, &rect);
}

inline static void renderTargetLane(const Level *level,
                                    SDL_Renderer *renderer) {
  SDL_Color color = level->palette->colors[TARGET];
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  SDL_FRect rect = {.x = level->boundaries.x,
                    .y = level->boundaries.y,
                    .w = level->boundaries.w,
                    .h = CELL_HEIGHT};
  SDL_RenderFillRect(renderer, &rect);
}

inline static void renderCarLanes(const Level *level, SDL_Renderer *renderer) {
  SDL_Color color = level->palette->colors[CAR_LANE];
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  SDL_FRect rect = {.x = level->boundaries.x,
                    .y = level->boundaries.y +
                         (1 + level->riverLanes + 1) * CELL_HEIGHT,
                    .w = level->boundaries.w,
                    .h = level->carLanes * CELL_HEIGHT};
  SDL_RenderFillRect(renderer, &rect);
}

inline static void renderRiverLanes(const Level *level,
                                    SDL_Renderer *renderer) {
  SDL_Color color = level->palette->colors[RIVER_LANE];
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  SDL_FRect rect = {.x = level->boundaries.x,
                    .y = level->boundaries.y + 1 * CELL_HEIGHT,
                    .w = level->boundaries.w,
                    .h = level->riverLanes * CELL_HEIGHT};
  SDL_RenderFillRect(renderer, &rect);
}

static void renderOneEntity(const Level *level,
                            const Entity *entity,
                            SDL_Renderer *renderer) {
  SDL_Texture *texture = renderEntity(entity, level);
  SDL_FRect dstrect = {.x = 0,
                       .y = 0,
                       .w = entity->size.x * CELL_WIDTH,
                       .h = entity->size.y * CELL_HEIGHT};
  gridToGlobalPosition(level, &entity->position, &dstrect.x, &dstrect.y);

  SDL_RenderTexture(renderer, texture, nullptr, &dstrect);
}

static void renderObstacles(const Level *level,
                            const Obstacles *obstacles,
                            SDL_Renderer *renderer) {
  for (unsigned int i = 0; i < obstacles->size; i++) {
    if (obstacles->obstacles[i] != nullptr) {
      renderOneEntity(level, obstacles->obstacles[i], renderer);
    }
  }
}

void renderLevel(const Level *level, SDL_Renderer *renderer) {
  // Since the background never changes, it would be best to prepare a texture
  // once and reuse it. Optimizing this project is not a priority, though.
  renderSafeLanes(level, renderer);
  renderTargetLane(level, renderer);
  renderCarLanes(level, renderer);
  renderRiverLanes(level, renderer);

  renderObstacles(level, &level->turtles, renderer);
  renderObstacles(level, &level->logs, renderer);
  renderOneEntity(level, level->player, renderer);
  renderObstacles(level, &level->cars, renderer);

  // The outside is drawn last to hide the obstacles that go offscreen.
  renderOutside(level, renderer);
}

void moveEventLevel(Level *level, Direction direction) {
  Player_move(level->player, direction, level);
}

unsigned int getLevelWidth(const Level *) {
  return COLUMNS;
}

unsigned int getLevelHeight(const Level *level) {
  return level->carLanes + level->riverLanes + 3;
}
