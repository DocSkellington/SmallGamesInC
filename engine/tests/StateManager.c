#include "Engine/StateManager.h"
#include "EngineTest.h"
#include <check.h>

START_TEST(create_and_free) {
  StateManager *manager = StateManager_Create();

  ck_assert_int_eq(manager->top, -1);
  ck_assert_int_gt(manager->capacity, 0);

  StateManager_Free(manager);
}

START_TEST(push_to_empty) {
  StateManager *manager = StateManager_Create();

  State *state = State_Create();
  StateManager_Push(manager, state);

  ck_assert_int_eq(manager->top, 0);

  StateManager_Free(manager);
}

END_TEST

Suite *makeStateManagerSuite(void) {
  Suite *suite = suite_create("State manager");
  TCase *tc_core = tcase_create("Core");

  suite_add_tcase(suite, tc_core);

  tcase_add_test(tc_core, create_and_free);
  tcase_add_test(tc_core, push_to_empty);

  return suite;
}
