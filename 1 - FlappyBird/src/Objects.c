#include "Objects.h"
#include "SDL3/SDL_render.h"

void render_object(const Object *object,
                   const SDL_Palette *palette,
                   SDL_Renderer *renderer) {
  SDL_Color col = palette->colors[object->type];
  SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

  SDL_RenderFillRect(renderer, &object->rectangle);
}
