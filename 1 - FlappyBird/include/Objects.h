/* Flappy Bird, part of a collection of small games in C.
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

typedef enum {
  BACKGROUND = 0,
  BIRD,
  PIPE,
  GROUND
} ObjectType;

typedef struct {
  ObjectType type;
  SDL_FRect rectangle;
  float speedX, speedY;
} Object;

void render_object(const Object *object,
                   const SDL_Palette *palette,
                   SDL_Renderer *renderer);

void update_object(Object *object, float deltaTime);

bool has_collision_with(const Object *source, const Object *target);
