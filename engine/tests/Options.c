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
#include "Engine/Pair.h"
#include "EngineTest.h"
#include <check.h>

typedef enum {
  OPTION_TEST = OPTION_CUSTOM,
} OptionNameTest;

START_TEST(create_and_free) {
  Options *options = Options_Create();
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));

  Options_Free(options);
}
END_TEST

START_TEST(set_get) {
  Options *options = Options_Create();
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));

  PairInt *windowSize = SDL_malloc(sizeof(PairInt));
  windowSize->first = 5;
  windowSize->second = 10;
  double value = 7.5;
  Options_Set(options, OPTION_WINDOWSIZE, windowSize, sizeof(PairInt));
  Options_Set(options, (OptionName)OPTION_TEST, &value, sizeof(double));

  ck_assert(Options_Has(options, OPTION_WINDOWSIZE));
  PairInt *sizeInOptions = Options_Get(options, OPTION_WINDOWSIZE);
  ck_assert_int_eq(sizeInOptions->first, windowSize->first);
  ck_assert_int_eq(sizeInOptions->second, windowSize->second);

  // Since Options hold a copy of windowSize, freeing it does not change
  // anything
  SDL_free(windowSize);
  ck_assert_int_eq(sizeInOptions->first, 5);
  ck_assert_int_eq(sizeInOptions->second, 10);

  ck_assert(Options_Has(options, (OptionName)OPTION_TEST));
  double *valueInOptions = Options_Get(options, (OptionName)OPTION_TEST);
  ck_assert_double_eq(*valueInOptions, value);

  Options_Free(options);
}
END_TEST

START_TEST(setNoCopy_get) {
  Options *options = Options_Create();
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));

  PairInt *windowSize = SDL_malloc(sizeof(PairInt));
  windowSize->first = 5;
  windowSize->second = 10;
  double value = 7.5;
  Options_SetNoCopy(options, OPTION_WINDOWSIZE, windowSize);
  Options_SetNoCopy(options, (OptionName)OPTION_TEST, &value);

  ck_assert(Options_Has(options, OPTION_WINDOWSIZE));
  PairInt *sizeInOptions = Options_Get(options, OPTION_WINDOWSIZE);
  ck_assert_int_eq(sizeInOptions->first, windowSize->first);
  ck_assert_int_eq(sizeInOptions->second, windowSize->second);

  ck_assert(Options_Has(options, (OptionName)OPTION_TEST));
  double *valueInOptions = Options_Get(options, (OptionName)OPTION_TEST);
  ck_assert_double_eq(*valueInOptions, value);

  SDL_free(windowSize);

  Options_Free(options);
}
END_TEST

static bool destroyed = false;
static void onDestroy(void *value) {
  destroyed = true;
  SDL_free(value);
}

START_TEST(setWithDestroy_get) {
  Options *options = Options_Create();
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));

  PairInt *windowSize = SDL_malloc(sizeof(PairInt));
  windowSize->first = 5;
  windowSize->second = 10;
  double value = 7.5;
  Options_SetWithDestroyFunction(
      options, OPTION_WINDOWSIZE, windowSize, sizeof(PairInt), onDestroy);
  Options_SetWithDestroyFunction(
      options, (OptionName)OPTION_TEST, &value, sizeof(double), onDestroy);

  ck_assert(Options_Has(options, OPTION_WINDOWSIZE));
  PairInt *sizeInOptions = Options_Get(options, OPTION_WINDOWSIZE);
  ck_assert_int_eq(sizeInOptions->first, windowSize->first);
  ck_assert_int_eq(sizeInOptions->second, windowSize->second);

  // Since Options hold a copy of windowSize, freeing it does not change
  // anything
  SDL_free(windowSize);
  ck_assert_int_eq(sizeInOptions->first, 5);
  ck_assert_int_eq(sizeInOptions->second, 10);

  ck_assert(Options_Has(options, (OptionName)OPTION_TEST));
  double *valueInOptions = Options_Get(options, (OptionName)OPTION_TEST);
  ck_assert_double_eq(*valueInOptions, value);

  Options_Free(options);
}
END_TEST

START_TEST(clear) {
  Options *options = Options_Create();
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));

  PairInt *windowSize = SDL_malloc(sizeof(PairInt));
  windowSize->first = 5;
  windowSize->second = 10;
  double value = 7.5;
  Options_Set(options, OPTION_WINDOWSIZE, windowSize, sizeof(PairInt));
  Options_SetWithDestroyFunction(
      options, (OptionName)OPTION_TEST, &value, sizeof(double), onDestroy);

  ck_assert(Options_Has(options, OPTION_WINDOWSIZE));
  Options_Clear(options, OPTION_WINDOWSIZE);
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));

  ck_assert(Options_Has(options, (OptionName)OPTION_TEST));
  destroyed = false;
  Options_Clear(options, (OptionName)OPTION_TEST);
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));
  ck_assert(destroyed);

  SDL_free(windowSize);
  Options_Free(options);
}
END_TEST

START_TEST(clearAll) {
  Options *options = Options_Create();
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));

  PairInt *windowSize = SDL_malloc(sizeof(PairInt));
  windowSize->first = 5;
  windowSize->second = 10;
  double value = 7.5;
  Options_Set(options, OPTION_WINDOWSIZE, windowSize, sizeof(PairInt));
  Options_SetWithDestroyFunction(
      options, (OptionName)OPTION_TEST, &value, sizeof(double), onDestroy);

  ck_assert(Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(Options_Has(options, (OptionName)OPTION_TEST));
  destroyed = false;
  Options_ClearAll(options);
  ck_assert(!Options_Has(options, OPTION_WINDOWSIZE));
  ck_assert(!Options_Has(options, (OptionName)OPTION_TEST));
  ck_assert(destroyed);

  SDL_free(windowSize);
  Options_Free(options);
}
END_TEST

Suite *makeOptionsSuite() {
  Suite *suite = suite_create("Options");
  TCase *tc_core = tcase_create("Data structure");
  suite_add_tcase(suite, tc_core);

  tcase_add_test(tc_core, create_and_free);
  tcase_add_test(tc_core, set_get);
  tcase_add_test(tc_core, setNoCopy_get);
  tcase_add_test(tc_core, setWithDestroy_get);
  tcase_add_test(tc_core, clear);
  tcase_add_test(tc_core, clearAll);

  return suite;
}
