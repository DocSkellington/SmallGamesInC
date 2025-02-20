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

#include "Engine/Options.h"
#include "SDL3/SDL_stdinc.h"
#include <glib.h>
#include <stddef.h>

typedef struct {
  void *value;
  void (*onDestroy)(void *value);
} ValueDestroy;

struct Options {
  Bindings *bindings;

  GHashTable *associations;
};

static void onKeyDestroy(gpointer data) {
  SDL_free(data);
}

static void onValueDestroy(gpointer data) {
  ValueDestroy *value = data;
  if (value->onDestroy != nullptr) {
    value->onDestroy(value->value);
  } else {
    SDL_free(value->value);
  }
  SDL_free(value);
}

static void noDestroy(gpointer) {}

static inline OptionName *makeOptionPointer(OptionName name) {
  OptionName *p = SDL_malloc(sizeof(OptionName));
  SDL_memcpy(p, &name, sizeof(OptionName));
  return p;
}

inline static ValueDestroy *createCopy(void *value, size_t count) {
  ValueDestroy *valueDestroy = SDL_malloc(sizeof(ValueDestroy));
  valueDestroy->value = SDL_malloc(count);
  SDL_memcpy(valueDestroy->value, value, count);
  valueDestroy->onDestroy = nullptr;
  return valueDestroy;
}

Options *Options_Create() {
  Options *options = SDL_malloc(sizeof(Options));
  options->bindings = Bindings_Create();
  options->associations = g_hash_table_new_full(
      g_int_hash, g_int_equal, onKeyDestroy, onValueDestroy);
  return options;
}

void Options_Free(Options *options) {
  Options_ClearAll(options);
  g_hash_table_destroy(options->associations);
  Bindings_Free(options->bindings);
  SDL_free(options);
}

Bindings *Options_GetBindings(Options *options) {
  return options->bindings;
}

void Options_Set(Options *options, OptionName name, void *value, size_t count) {
  ValueDestroy *valueDestroy = createCopy(value, count);
  valueDestroy->onDestroy = nullptr;
  g_hash_table_insert(
      options->associations, makeOptionPointer(name), valueDestroy);
}

void Options_SetWithDestroyFunction(Options *options,
                                    OptionName name,
                                    void *value,
                                    size_t count,
                                    void (*onDestroy)(void *value)) {
  ValueDestroy *valueDestroy = createCopy(value, count);
  valueDestroy->onDestroy = onDestroy;
  g_hash_table_insert(
      options->associations, makeOptionPointer(name), valueDestroy);
}

void Options_SetNoCopy(Options *options, OptionName name, void *value) {
  ValueDestroy *valueDestroy = SDL_malloc(sizeof(ValueDestroy));
  valueDestroy->value = value;
  valueDestroy->onDestroy = noDestroy;
  g_hash_table_insert(
      options->associations, makeOptionPointer(name), valueDestroy);
}

bool Options_Has(const Options *options, OptionName name) {
  return g_hash_table_lookup(options->associations, &name) != nullptr;
}

void *Options_Get(const Options *options, OptionName name) {
  ValueDestroy *value = g_hash_table_lookup(options->associations, &name);
  if (value == nullptr) {
    return nullptr;
  } else {
    return value->value;
  }
}

void Options_Clear(Options *options, OptionName name) {
  g_hash_table_remove(options->associations, &name);
}

void Options_ClearAll(Options *options) {
  g_hash_table_remove_all(options->associations);
}
