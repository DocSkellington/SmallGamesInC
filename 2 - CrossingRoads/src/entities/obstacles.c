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
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

typedef struct {
  SDL_Texture *texture;
  Direction direction;
  double speed;
} Memory;

static void cleanup(Entity *entity) {
  Memory *memory = entity->memory;
  SDL_DestroyTexture(memory->texture);
  SDL_free(memory);
}

static SDL_Texture *render(const Entity *entity, const Level *) {
  const Memory *memory = entity->memory;
  return memory->texture;
}

inline static Entity *createGeneric(Position start,
                                    Direction direction,
                                    unsigned int size,
                                    double speed,
                                    SDL_Renderer *renderer) {
  Entity *entity = SDL_malloc(sizeof(Entity));
  entity->memory = SDL_malloc(sizeof(Memory));
  entity->position = start;
  entity->size.x = size;
  entity->size.y = 1;
  entity->cleanup = cleanup;
  entity->render = render;

  Memory *memory = entity->memory;
  memory->direction = direction;
  memory->speed = speed;
  memory->texture = SDL_CreateTexture(renderer,
                                      SDL_PIXELFORMAT_RGBA32,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      size * CELL_WIDTH,
                                      CELL_HEIGHT);

  return entity;
}

inline static void fillTexture(SDL_Texture *texture, SDL_Color *color) {
  SDL_Surface *surface;
  if (SDL_LockTextureToSurface(texture, nullptr, &surface)) {
    const SDL_PixelFormatDetails *formatDetails =
        SDL_GetPixelFormatDetails(surface->format);

    SDL_FillSurfaceRect(
        surface,
        nullptr,
        SDL_MapRGBA(
            formatDetails, nullptr, color->r, color->g, color->b, color->a));

    SDL_UnlockTexture(texture);
  }
}

Entity *createCarEntity(Level *,
                        Position start,
                        Direction direction,
                        unsigned int size,
                        double speed,
                        SDL_Renderer *renderer) {
  Entity *car = createGeneric(start, direction, size, speed, renderer);
  Memory *memory = car->memory;
  SDL_Color color = {.r = 160, .g = 25, .b = 25, .a = SDL_ALPHA_OPAQUE};
  fillTexture(memory->texture, &color);

  // entity->update = update;

  return car;
}

Entity *createTurtleEntity(Level *,
                           Position start,
                           Direction direction,
                           unsigned int size,
                           double speed,
                           SDL_Renderer *renderer) {
  Entity *turtle = createGeneric(start, direction, size, speed, renderer);
  Memory *memory = turtle->memory;
  SDL_Color color = {.r = 25, .g = 150, .b = 50, .a = SDL_ALPHA_OPAQUE};
  fillTexture(memory->texture, &color);
  return turtle;
}

Entity *createLogEntity(Level *,
                        Position start,
                        Direction direction,
                        unsigned int size,
                        double speed,
                        SDL_Renderer *renderer) {
  Entity *log = createGeneric(start, direction, size, speed, renderer);
  Memory *memory = log->memory;
  SDL_Color color = {.r = 153, .g = 88, .b = 42, .a = SDL_ALPHA_OPAQUE};
  fillTexture(memory->texture, &color);
  return log;
}
