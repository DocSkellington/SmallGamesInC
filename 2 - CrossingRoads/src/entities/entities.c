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

#include "Entities.h"

void freeEntity(Entity *entity) {
  if (entity->cleanup != nullptr) {
    entity->cleanup(entity);
  }
  SDL_free(entity);
}

SDL_Texture* renderEntity(const Entity *entity) {
  if (entity != nullptr && entity->render != nullptr) {
    return entity->render(entity);
  }
  return nullptr;
}

void updateEntity(Entity *entity, Uint64 deltaMS) {
  if (entity != nullptr && entity->update != nullptr) {
    entity->update(entity, deltaMS);
  }
}

