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
#include "SDL3/SDL.h"

#define CELL_WIDTH 32
#define CELL_HEIGHT 32

typedef struct {
  double x, y;
} Position;

typedef struct Level Level;

Level *createLevel(unsigned int speed, unsigned int carLanes, unsigned int riverLanes, bool safeZones, SDL_Rect *windowSize, SDL_Renderer *renderer);
void freeLevel(Level *level);
void updateLevel(Level *level, Uint64 deltaMS);
void renderLevel(const Level *level, SDL_Renderer *renderer);
void moveEventLevel(Level *level, Direction direction);

unsigned int getLevelWidth(const Level *level);
unsigned int getLevelHeight(const Level *level);
