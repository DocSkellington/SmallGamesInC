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
#pragma once

#include "SDL3/SDL.h"

#define CELL_WIDTH 20
#define CELL_HEIGHT 20

typedef struct {
  float x, y;
} Position;

typedef struct Entity Entity;

struct Entity {
  void *memory;
  Position position;
  void (*cleanup)(Entity *entity);
  void (*render)(const Entity *entity, SDL_Renderer *renderer);
  void (*update)(Entity *entity, Uint64 deltaMS);
};

void freeEntity(Entity *entity);
void renderEntity(const Entity *entity, SDL_Renderer *renderer);
void updateEntity(Entity *entity, Uint64 deltaMS);

typedef enum {
  UP = 0,
  DOWN,
  LEFT,
  RIGHT,
} Direction;

Entity *createPlayerEntity();
void Player_move(Entity *entity, Direction direction);
