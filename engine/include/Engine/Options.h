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

#include "Bindings.h"

/**
 * The predefined actions.
 *
 * Create an enumeration whose first value is ACTION_CUSTOM to add new actions.
 */
typedef enum {
  OPTION_WINDOWSIZE = 0,

  OPTION_CUSTOM
} OptionName;

/**
 * The Options struct handles the global options of the game.
 *
 * It stores key-value pairs, where key must be an element of \ref OptionName
 * (or of an extension of the enumeration), and value can be a pointer to
 * anything.
 *
 * Use \ref Options_Create to create a new options manager.
 * To add a new pair, multiple functions are defined: \ref Options_Set that
 * copies the value (i.e., the stored value belongs to Options), \ref
 * Options_SetWithDestroyFunction that copies the value and defines a specific
 * function to destroy it when needed (which is useful for non-trivial objects),
 * and \ref Options_SetNoCopy that stores the pointer as-is, without copying the
 * value (i.e., the stored value belongs to the caller of the function).
 *
 * The functions \ref Options_Clear and \ref Options_ClearAll respectively
 * removes one pair and all pairs.
 *
 * \todo Save and load.
 */
typedef struct Options Options;

Options *Options_Create();
void Options_Free(Options *options);
Bindings *Options_GetBindings(Options *options);
void Options_Set(Options *options, OptionName name, void *value, size_t count);
void Options_SetWithDestroyFunction(Options *options,
                                    OptionName name,
                                    void *value,
                                    size_t count,
                                    void (*onDestroy)(void *value));
void Options_SetNoCopy(Options *options, OptionName name, void *value);
bool Options_Has(const Options *options, OptionName name);
void *Options_Get(const Options *options, OptionName name);
void Options_Clear(Options *options, OptionName name);
void Options_ClearAll(Options *options);
