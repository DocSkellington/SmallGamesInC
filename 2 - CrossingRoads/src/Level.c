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
#include "Entities.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

#define COLUMNS 14

struct Level {
  Entity *player;
  unsigned int speed;
  unsigned int carLanes;
  unsigned int riverLanes;
  bool safeZones;
  SDL_Rect boundaries;
  SDL_Rect windowSize;
};

Level *createLevel(unsigned int speed, unsigned int carLanes, unsigned int riverLanes, bool safeZones, SDL_Rect *windowSize) {
  Level *level = SDL_malloc(sizeof(Level));
  level->speed = speed;
  // TODO
  level->carLanes = carLanes;
  level->riverLanes = riverLanes;
  level->safeZones = safeZones;
  level->boundaries.w = COLUMNS * CELL_WIDTH;
  level->boundaries.h = (carLanes + riverLanes + 3) * CELL_HEIGHT;
  level->boundaries.x = (windowSize->w - level->boundaries.w) / 2.;
  level->boundaries.y = (windowSize->h - level->boundaries.h) / 2.;

  Position start = {.x = level->boundaries.w / 2., .y = level->boundaries.h};
  level->player = createPlayerEntity(level, start);
  level->windowSize = *windowSize;
  return level;
}

void freeLevel(Level *level) {
  freeEntity(level->player);
  SDL_free(level);
}

void updateLevel(Level *level, Uint64 deltaMS) {
  updateEntity(level->player, deltaMS);
}

static void renderAroundBoundary(const Level *level, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_FRect rect = {.x = 0, .y = 0, .w = level->boundaries.x, .h = level->windowSize.h};
  SDL_RenderFillRect(renderer, &rect);

  rect.x = level->boundaries.x + level->boundaries.w;
  SDL_RenderFillRect(renderer, &rect);
}

void renderLevel(const Level *level, SDL_Renderer *renderer) {
  renderAroundBoundary(level, renderer);

  Position shift = {.x = level->boundaries.x, .y = level->boundaries.y};
  renderEntity(level->player, renderer, shift);
}

void moveEventLevel(Level *level, Direction direction) {
  Player_move(level->player, direction);
}
