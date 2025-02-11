#include "Engine/StateManager.h"
#include "EngineTest.h"
#include <check.h>

START_TEST(create_and_free) {
  StateManager *manager = StateManager_Create(3);
  ck_assert_ptr_nonnull(manager);

  ck_assert_int_eq(manager->top, -1);
  ck_assert_int_eq(manager->capacity, 3);

  StateManager_Free(manager);

  manager = StateManager_Create(0);
  ck_assert_ptr_null(manager);
}

END_TEST

START_TEST(pop_empty) {
  StateManager *manager = StateManager_Create(2);
  ck_assert_ptr_nonnull(manager);

  ck_assert_int_eq(StateManager_Pop(manager), STATEMANAGER_EMPTY);

  StateManager_Free(manager);
}

END_TEST

START_TEST(push_1) {
  StateManager *manager = StateManager_Create(2);
  ck_assert_ptr_nonnull(manager);

  State *s1 = State_Create();
  ck_assert_int_eq(StateManager_Push(manager, s1), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);

  StateManager_Free(manager);
}

END_TEST

START_TEST(push_2) {
  StateManager *manager = StateManager_Create(3);
  ck_assert_ptr_nonnull(manager);

  State *s1 = State_Create();
  State *s2 = State_Create();
  ck_assert_int_eq(StateManager_Push(manager, s1), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Push(manager, s2), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 1);

  StateManager_Free(manager);
}

END_TEST

START_TEST(push_3) {
  StateManager *manager = StateManager_Create(3);
  ck_assert_ptr_nonnull(manager);

  State *s1 = State_Create();
  State *s2 = State_Create();
  State *s3 = State_Create();
  ck_assert_int_eq(StateManager_Push(manager, s1), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Push(manager, s2), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 1);
  ck_assert_int_eq(StateManager_Push(manager, s3), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 2);

  StateManager_Free(manager);
}

END_TEST

START_TEST(push_exceeds_capacity) {
  StateManager *manager = StateManager_Create(2);
  ck_assert_ptr_nonnull(manager);

  State *s1 = State_Create();
  State *s2 = State_Create();
  State *s3 = State_Create();
  ck_assert_int_eq(StateManager_Push(manager, s1), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Push(manager, s2), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 1);
  ck_assert_int_eq(StateManager_Push(manager, s3), STATEMANAGER_FULL);
  ck_assert_int_eq(manager->top, 1);

  StateManager_Free(manager);
}

END_TEST

START_TEST(push_pop_1) {
  StateManager *manager = StateManager_Create(3);
  ck_assert_ptr_nonnull(manager);

  State *s1 = State_Create();
  ck_assert_int_eq(StateManager_Push(manager, s1), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Pop(manager), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, -1);

  StateManager_Free(manager);
}

END_TEST

START_TEST(push_pop_2) {
  StateManager *manager = StateManager_Create(3);
  ck_assert_ptr_nonnull(manager);

  State *s1 = State_Create();
  State *s2 = State_Create();
  State *s3 = State_Create();
  ck_assert_int_eq(StateManager_Push(manager, s1), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Push(manager, s2), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 1);
  ck_assert_int_eq(StateManager_Pop(manager), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Push(manager, s3), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 1);
  ck_assert_int_eq(StateManager_Pop(manager), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, 0);
  ck_assert_int_eq(StateManager_Pop(manager), STATEMANAGER_OK);
  ck_assert_int_eq(manager->top, -1);
  ck_assert_int_eq(StateManager_Pop(manager), STATEMANAGER_EMPTY);
  ck_assert_int_eq(manager->top, -1);

  StateManager_Free(manager);
}

END_TEST

Suite *makeStateManagerSuite(void) {
  Suite *suite = suite_create("State manager");
  TCase *tc_core = tcase_create("Stack");

  suite_add_tcase(suite, tc_core);

  tcase_add_test(tc_core, create_and_free);
  tcase_add_test(tc_core, pop_empty);
  tcase_add_test(tc_core, push_1);
  tcase_add_test(tc_core, push_2);
  tcase_add_test(tc_core, push_3);
  tcase_add_test(tc_core, push_exceeds_capacity);
  tcase_add_test(tc_core, push_pop_1);
  tcase_add_test(tc_core, push_pop_2);

  return suite;
}
