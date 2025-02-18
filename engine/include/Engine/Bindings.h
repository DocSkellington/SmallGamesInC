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
#include <glib.h>

/**
 * The predefined actions.
 *
 * Create an enumeration whose first value is ACTION_CUSTOM to add new actions.
 */
typedef enum {
  ACTION_MOVE_FORWARD = 0,
  ACTION_MOVE_BACKWARD,
  ACTION_MOVE_LEFT,
  ACTION_MOVE_RIGHT,
  ACTION_MENU_UP,
  ACTION_MENU_DOWN,
  ACTION_MENU_LEFT,
  ACTION_MENU_RIGHT,

  ACTION_CUSTOM
} Action;

/**
 * The Bindings struct handles the bindings between an \ref Action and a
 * SDL_Scancode.
 *
 * Use \ref Bindings_Create to create a new bindings manager, which can be
 * populated via \ref Bindings_Set and \ref Bindings_Add. The function \ref
 * Bindings_Remove removes one binding, while \ref Bindings_Clear removes every
 * scancode associated to an action.
 *
 * To test if a binding exists for an action, use \ref Bindings_Has, and \ref
 * Bindings_Matches to check whether a given scancode matches the bindings of
 * the action. To get every defined binding, call \ref Bindings_Get.
 *
 * It is also possible to define aliases: if <code>Bindings_SetAlias(bindings,
 * a, b)</code> is called, then any scancode that matches <code>b</code> will
 * also match <code>a</code>. Use \ref Bindings_SetAlias, \ref
 * Bindings_AddAlias, \ref Bindings_RemoveAlias and \ref Bindings_ClearAlias to
 * manipulate the aliases.
 *
 * \ref Bindings_Get does not return, nor "expand" the aliases, i.e., the
 * function only returns the proper key bindings.
 *
 * \warning Controllers and mouse controls are not supported.
 */
typedef struct Bindings Bindings;

Bindings *Bindings_Create();
void Bindings_Free(Bindings *bindings);
void Bindings_Clear(Bindings *bindings, Action action);
void Bindings_ClearAlias(Bindings *bindings, Action action);
void Bindings_Remove(Bindings *bindings, Action action, SDL_Scancode scancode);
void Bindings_RemoveAlias(Bindings *bindings, Action action, Action alias);
void Bindings_Set(Bindings *bindings, Action action, SDL_Scancode scancode);
void Bindings_SetAlias(Bindings *bindings, Action action, Action alias);
void Bindings_Add(Bindings *bindings, Action action, SDL_Scancode scancode);
void Bindings_AddAlias(Bindings *bindings, Action action, Action alias);
bool Bindings_Has(Bindings *bindings, Action action);
void Bindings_Get(Bindings *bindings,
                  Action action,
                  SDL_Scancode **scancodes,
                  unsigned int *length);
bool Bindings_Matches(Bindings *bindings, Action action, SDL_Scancode scancode);
