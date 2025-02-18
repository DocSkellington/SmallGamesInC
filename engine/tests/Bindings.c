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
#include "EngineTest.h"
#include "SDL3/SDL.h"
#include <check.h>

typedef enum {
  ACTION_TEST = ACTION_CUSTOM
} ActionsTest;

START_TEST(create_and_free) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));
  Bindings_Free(bindings);
}
END_TEST

START_TEST(set) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Set(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_Set(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5);

  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Free(bindings);
}
END_TEST

START_TEST(set_alias) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Set(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_SetAlias(bindings, (Action)ACTION_TEST, ACTION_MOVE_FORWARD);

  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Free(bindings);
}
END_TEST

START_TEST(add) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));

  Bindings_Add(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_Add(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_W);

  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));

  Bindings_Free(bindings);
}
END_TEST

START_TEST(add_alias) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Add(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_AddAlias(bindings, ACTION_MOVE_FORWARD, ACTION_MOVE_FORWARD);
  Bindings_AddAlias(bindings, (Action)ACTION_TEST, ACTION_MOVE_FORWARD);

  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Free(bindings);
}
END_TEST

START_TEST(clear) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));

  Bindings_Set(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_SetAlias(bindings, (Action)ACTION_TEST, ACTION_MOVE_FORWARD);
  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_ClearAlias(bindings, ACTION_MOVE_FORWARD);
  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));
  Bindings_Clear(bindings, ACTION_MOVE_FORWARD);
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Clear(bindings, (Action)ACTION_TEST);
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));
  Bindings_ClearAlias(bindings, (Action)ACTION_TEST);
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Free(bindings);
}
END_TEST

START_TEST(remove) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Add(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5);
  Bindings_Add(bindings, (Action)ACTION_TEST, SDL_SCANCODE_SPACE);

  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  SDL_Scancode *scancodes;
  unsigned int size;
  Bindings_Get(bindings, (Action)ACTION_TEST, &scancodes, &size);
  ck_assert_int_eq(size, 2);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_KP_5);
  ck_assert_int_eq(scancodes[1], SDL_SCANCODE_SPACE);
  SDL_free(scancodes);

  Bindings_Remove(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5);
  Bindings_Get(bindings, (Action)ACTION_TEST, &scancodes, &size);
  ck_assert_int_eq(size, 1);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_SPACE);
  SDL_free(scancodes);

  Bindings_Remove(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5);
  Bindings_Get(bindings, (Action)ACTION_TEST, &scancodes, &size);
  ck_assert_int_eq(size, 1);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_SPACE);
  SDL_free(scancodes);

  Bindings_Remove(bindings, (Action)ACTION_TEST, SDL_SCANCODE_SPACE);
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Free(bindings);
}
END_TEST

START_TEST(get) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  SDL_Scancode *scancodes;
  unsigned int size;
  Bindings_Get(bindings, ACTION_MOVE_FORWARD, &scancodes, &size);
  ck_assert_int_eq(size, 0);
  ck_assert(scancodes == nullptr);

  Bindings_Set(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_Add(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5);
  Bindings_Add(bindings, (Action)ACTION_TEST, SDL_SCANCODE_SPACE);

  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Get(bindings, ACTION_MOVE_FORWARD, &scancodes, &size);
  ck_assert_int_eq(size, 1);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_UP);
  SDL_free(scancodes);

  Bindings_Get(bindings, (Action)ACTION_TEST, &scancodes, &size);
  ck_assert_int_eq(size, 2);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_KP_5);
  ck_assert_int_eq(scancodes[1], SDL_SCANCODE_SPACE);
  SDL_free(scancodes);

  Bindings_AddAlias(bindings, (Action)ACTION_TEST, ACTION_MOVE_FORWARD);
  Bindings_Get(bindings, (Action)ACTION_TEST, &scancodes, &size);
  ck_assert_int_eq(size, 2);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_KP_5);
  ck_assert_int_eq(scancodes[1], SDL_SCANCODE_SPACE);
  SDL_free(scancodes);

  Bindings_SetAlias(bindings, (Action)ACTION_TEST, ACTION_MOVE_FORWARD);
  Bindings_Get(bindings, (Action)ACTION_TEST, &scancodes, &size);
  ck_assert_int_eq(size, 2);
  ck_assert_int_eq(scancodes[0], SDL_SCANCODE_KP_5);
  ck_assert_int_eq(scancodes[1], SDL_SCANCODE_SPACE);
  SDL_free(scancodes);

  Bindings_Free(bindings);
}
END_TEST

START_TEST(matches) {
  Bindings *bindings = Bindings_Create();
  ck_assert(!Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(!Bindings_Has(bindings, (Action)ACTION_TEST));

  Bindings_Set(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP);
  Bindings_Add(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5);

  ck_assert(Bindings_Has(bindings, ACTION_MOVE_FORWARD));
  ck_assert(Bindings_Has(bindings, (Action)ACTION_TEST));

  ck_assert(Bindings_Matches(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP));
  ck_assert(Bindings_Matches(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5));
  ck_assert(!Bindings_Matches(bindings, (Action)ACTION_TEST, SDL_SCANCODE_UP));

  Bindings_AddAlias(bindings, (Action)ACTION_TEST, ACTION_MOVE_FORWARD);
  ck_assert(Bindings_Matches(bindings, ACTION_MOVE_FORWARD, SDL_SCANCODE_UP));
  ck_assert(Bindings_Matches(bindings, (Action)ACTION_TEST, SDL_SCANCODE_KP_5));
  ck_assert(Bindings_Matches(bindings, (Action)ACTION_TEST, SDL_SCANCODE_UP));

  Bindings_Free(bindings);
}
END_TEST

Suite *makeBindingsSuite(void) {
  Suite *suite = suite_create("Bindings manager");
  TCase *tc_core = tcase_create("Data structure");
  suite_add_tcase(suite, tc_core);

  tcase_add_test(tc_core, create_and_free);
  tcase_add_test(tc_core, set);
  tcase_add_test(tc_core, set_alias);
  tcase_add_test(tc_core, add);
  tcase_add_test(tc_core, add_alias);
  tcase_add_test(tc_core, clear);
  tcase_add_test(tc_core, remove);
  tcase_add_test(tc_core, get);
  tcase_add_test(tc_core, matches);

  return suite;
}
