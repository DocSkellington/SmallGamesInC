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

#include "Direction.h"
#include "Level.h"
#include "SDL3/SDL.h"

typedef struct Entity Entity;

struct Entity {
  void *memory;
  Level *level;
  Position position;
  void (*cleanup)(Entity *entity);
  void (*render)(const Entity *entity, SDL_Renderer *renderer, Position shift);
  void (*update)(Entity *entity, Uint64 deltaMS);
};

void freeEntity(Entity *entity);
void renderEntity(const Entity *entity, SDL_Renderer *renderer, Position shift);
void updateEntity(Entity *entity, Uint64 deltaMS);

Entity *createPlayerEntity(Level *level, Position start);
void Player_move(Entity *entity, Direction direction);
