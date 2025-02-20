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

#include "Engine/Bindings.h"
#include "glib.h"

typedef struct {
  GSList *list;
} Container;

struct Bindings {
  GHashTable *associations;
  GHashTable *aliases;
};

static void onKeyDestroy(gpointer data) {
  SDL_free(data);
}

static void onValueDestroy(gpointer data) {
  Container *cont = data;
  g_slist_free_full(cont->list, onKeyDestroy);
  SDL_free(cont);
}

static inline gpointer makeActionPointer(Action action) {
  Action *p = SDL_malloc(sizeof(Action));
  SDL_memcpy(p, &action, sizeof(Action));
  return p;
}

static inline gpointer makeScancodePointer(SDL_Scancode code) {
  SDL_Scancode *p = SDL_malloc(sizeof(SDL_Scancode));
  SDL_memcpy(p, &code, sizeof(SDL_Scancode));
  return p;
}

static void addCodeToContainer(Container *container, SDL_Scancode code) {
  container->list = g_slist_append(container->list, makeScancodePointer(code));
}

static void addActionToContainer(Container *container, Action action) {
  container->list = g_slist_append(container->list, makeActionPointer(action));
}

static Container *createContainerScancode(SDL_Scancode code) {
  Container *container = SDL_malloc(sizeof(Container));
  container->list = nullptr;
  addCodeToContainer(container, code);
  return container;
}

static Container *createContainerAction(Action action) {
  Container *container = SDL_malloc(sizeof(Container));
  container->list = nullptr;
  addActionToContainer(container, action);
  return container;
}

static gint compareCode(gconstpointer a, gconstpointer b) {
  const SDL_Scancode *a1 = a;
  const SDL_Scancode *a2 = b;
  if (*a1 < *a2) {
    return -1;
  } else if (*a1 == *a2) {
    return 0;
  }
  return 1;
}

static gint compareAction(gconstpointer a, gconstpointer b) {
  const Action *a1 = a;
  const Action *a2 = b;
  if (*a1 < *a2) {
    return -1;
  } else if (*a1 == *a2) {
    return 0;
  }
  return 1;
}

static void removeCodeFromContainer(Container *container, SDL_Scancode code) {
  if (container == nullptr) {
    return;
  }
  GSList *node = g_slist_find_custom(container->list, &code, compareCode);
  container->list = g_slist_remove_link(container->list, node);
  g_slist_free_1(node);
}

static void removeActionFromContainer(Container *container, Action action) {
  if (container == nullptr) {
    return;
  }
  GSList *node = g_slist_find_custom(container->list, &action, compareAction);
  container->list = g_slist_remove_link(container->list, node);
  g_slist_free_1(node);
}

static bool hasAssociation(Bindings *bindings, Action action) {
  return g_hash_table_contains(bindings->associations, &action);
}

static bool hasAlias(Bindings *bindings, Action action) {
  return g_hash_table_contains(bindings->aliases, &action);
}

static void containerToArray(Container *container,
                             SDL_Scancode **codes,
                             unsigned int *size) {
  GSList *codes_list = container->list;
  *size = g_slist_length(codes_list);
  *codes = SDL_calloc(*size, sizeof(SDL_Scancode));
  unsigned int i = 0;

  while (codes_list != nullptr) {
    SDL_Scancode *value = codes_list->data;
    (*codes)[i++] = *value;
    codes_list = codes_list->next;
  }
}

static bool codeInContainer(Container *container, SDL_Scancode code) {
  if (container == nullptr) {
    return false;
  }

  GSList *codes_list = container->list;
  while (codes_list != nullptr) {
    SDL_Scancode *value = codes_list->data;
    if (*value == code) {
      return true;
    }
    codes_list = codes_list->next;
  }
  return false;
}

static bool codeInAliases(Bindings *bindings, Container *aliases, SDL_Scancode code) {
  if (aliases == nullptr) {
    return false;
  }

  GSList *aliases_list = aliases->list;
  while (aliases_list != nullptr) {
    Action *action = aliases_list->data;
    Container *assoc = g_hash_table_lookup(bindings->associations, action);
    if (codeInContainer(assoc, code)) {
      return true;
    }
    aliases_list = aliases_list->next;
  }
  return false;
}

Bindings *Bindings_Create() {
  Bindings *bindings = SDL_malloc(sizeof(Bindings));
  bindings->associations = g_hash_table_new_full(
      g_int_hash, g_int_equal, onKeyDestroy, onValueDestroy);
  bindings->aliases = g_hash_table_new_full(
      g_int_hash, g_int_equal, onKeyDestroy, onValueDestroy);
  return bindings;
}

void Bindings_Free(Bindings *bindings) {
  g_hash_table_destroy(bindings->associations);
  g_hash_table_destroy(bindings->aliases);
  SDL_free(bindings);
}

void Bindings_Clear(Bindings *bindings, Action action) {
  g_hash_table_remove(bindings->associations, &action);
}

void Bindings_ClearAlias(Bindings *bindings, Action action) {
  g_hash_table_remove(bindings->aliases, &action);
}

void Bindings_Remove(Bindings *bindings, Action action, SDL_Scancode scancode) {
  Container *assoc = g_hash_table_lookup(bindings->associations, &action);
  removeCodeFromContainer(assoc, scancode);
  if (assoc->list == nullptr) {
    g_hash_table_remove(bindings->associations, &action);
  }
}

void Bindings_RemoveAlias(Bindings *bindings, Action action, Action alias) {
  Container *aliases = g_hash_table_lookup(bindings->aliases, &action);
  removeActionFromContainer(aliases, alias);
  if (aliases->list == nullptr) {
    g_hash_table_remove(bindings->aliases, &action);
  }
}

void Bindings_Set(Bindings *bindings, Action action, SDL_Scancode scancode) {
  g_hash_table_insert(bindings->associations,
                      makeActionPointer(action),
                      createContainerScancode(scancode));
}

void Bindings_SetAlias(Bindings *bindings, Action action, Action alias) {
  if (action == alias) {
    return;
  }
  g_hash_table_insert(bindings->aliases,
                      makeActionPointer(action),
                      createContainerAction(alias));
}

void Bindings_Add(Bindings *bindings, Action action, SDL_Scancode scancode) {
  Container *assoc = g_hash_table_lookup(bindings->associations, &action);
  if (assoc == nullptr) {
    Bindings_Set(bindings, action, scancode);
  } else {
    addCodeToContainer(assoc, scancode);
  }
}

void Bindings_AddAlias(Bindings *bindings, Action action, Action alias) {
  Container *aliases = g_hash_table_lookup(bindings->aliases, &action);
  if (aliases == nullptr) {
    Bindings_SetAlias(bindings, action, alias);
  } else {
    addActionToContainer(aliases, alias);
  }
}

bool Bindings_Has(Bindings *bindings, Action action) {
  return hasAssociation(bindings, action) || hasAlias(bindings, action);
}

void Bindings_Get(Bindings *bindings,
                  Action action,
                  SDL_Scancode **scancodes,
                  unsigned int *length) {
  Container *assoc = g_hash_table_lookup(bindings->associations, &action);
  if (assoc == nullptr) {
    *length = 0;
    *scancodes = nullptr;
    return;
  }
  containerToArray(assoc, scancodes, length);
}

bool Bindings_Matches(Bindings *bindings,
                      Action action,
                      SDL_Scancode scancode) {
  Container *assoc = g_hash_table_lookup(bindings->associations, &action);
  Container *aliases = g_hash_table_lookup(bindings->aliases, &action);
  return codeInContainer(assoc, scancode) ||
         codeInAliases(bindings, aliases, scancode);
}
