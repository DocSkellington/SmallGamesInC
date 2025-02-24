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
  Position position;
  Position size;
  void (*cleanup)(Entity *entity);
  SDL_Texture *(*render)(const Entity *entity, const Level *level);
  void (*update)(Entity *entity, Uint64 deltaMS, Level *level);
};

void freeEntity(Entity *entity);
SDL_Texture *renderEntity(const Entity *entity, const Level *level);
void updateEntity(Entity *entity, Uint64 deltaMS, Level *level);

Entity *
createPlayerEntity(Level *level, Position start, SDL_Renderer *renderer);
void Player_move(Entity *entity, Direction direction, Level *level);
bool isPlayerJumping(const Entity *entity);

Entity *createCarEntity(Level *level,
                        Position start,
                        Direction direction,
                        unsigned int size,
                        double speed,
                        SDL_Renderer *renderer);

Entity *createTurtleEntity(Level *level,
                           Position start,
                           Direction direction,
                           unsigned int size,
                           double speed,
                           SDL_Renderer *renderer);

Entity *createLogEntity(Level *level,
                        Position start,
                        Direction direction,
                        unsigned int size,
                        double speed,
                        SDL_Renderer *renderer);

void movePlayerWithObstacle(const Entity *obstacle, Entity *player, Uint64 deltaMS);
