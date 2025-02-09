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

#define WINDOW_DEFAULT_WIDTH 640
#define WINDOW_DEFAULT_HEIGHT 480

typedef enum {
  BACKGROUND = 0,
  BIRD,
  PIPE,
  GROUND
} ObjectType;

typedef struct {
  float x, y;
} Vector2f;

typedef struct {
  float positionY;
  float velocityY;
} Bird;

typedef struct {
  Vector2f position;
  Vector2f size;
  int gap;
  bool scored;
} Pipe;

typedef struct {
  bool running, lost;
  int groundY;
  float speedPipes, gapPipes;
  Vector2f windowSize;
  Bird *bird;
  Pipe *pipes;
  int score;
} GameState;

void Game_Init(GameState **state);

void Game_Free(GameState *state);

void Game_Render(const GameState *state,
                 const SDL_Palette *palette,
                 SDL_Renderer *renderer);

void Game_Update(GameState *state, float delta);

void Game_Event(GameState *state, const SDL_Event *event);
