/* Small game engine in C.
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

typedef struct {
  int first, second;
} PairInt;

typedef struct {
  unsigned int first, second;
} PairUInt;

typedef struct {
  Uint64 first, second;
} PairUInt64;

typedef struct {
  bool first, second;
} PairBool;

typedef struct {
  float first, second;
} PairFloat;

typedef struct {
  double first, second;
} PairDouble;
