#include "Engine/StateManager.h"
#include "EngineTest.h"
#include <check.h>
#include <stdlib.h>

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

typedef struct {
  int n;
} Memory;

void init_state(void **memory, StateManager *) {
  *memory = malloc(sizeof(memory));
  ((Memory*)*memory)->n = 5;
}

void destroy_state(void *memory) {
  free(memory);
}

bool update_state_passthrough(void *memory, float, StateManager *) {
  ((Memory*)memory)->n *= 2;
  return true;
}

bool update_state_no_passthrough(void *memory, float, StateManager *) {
  ((Memory*)memory)->n *= 2;
  return false;
}

START_TEST(init_destroy_memory) {
  StateManager *manager = StateManager_Create(1);
  State *state = State_Create();
  State_SetInit(state, init_state);
  State_SetDestroy(state, destroy_state);

  StateManager_Push(manager, state);
  ck_assert_int_eq(((Memory*)state->memory)->n, 5);

  StateManager_Free(manager);
}

START_TEST(update_passthrough) {
  StateManager *manager = StateManager_Create(2);
  State *bottom = State_Create();
  State *top = State_Create();

  State_SetInit(bottom, init_state);
  State_SetInit(top, init_state);
  State_SetDestroy(bottom, destroy_state);
  State_SetDestroy(top, destroy_state);
  State_SetUpdate(bottom, update_state_passthrough);
  State_SetUpdate(top, update_state_passthrough);

  StateManager_Push(manager, bottom);
  StateManager_Push(manager, top);
  Memory *mBottom = bottom->memory;
  Memory *mTop = top->memory;
  ck_assert_int_eq(mBottom->n, 5);
  ck_assert_int_eq(mTop->n, 5);

  StateManager_Update(manager, 0);
  ck_assert_int_eq(mBottom->n, 10);
  ck_assert_int_eq(mTop->n, 10);

  StateManager_Free(manager);
}

START_TEST(update_no_passthrough) {
  StateManager *manager = StateManager_Create(2);
  State *bottom = State_Create();
  State *top = State_Create();

  State_SetInit(bottom, init_state);
  State_SetInit(top, init_state);
  State_SetDestroy(bottom, destroy_state);
  State_SetDestroy(top, destroy_state);
  State_SetUpdate(bottom, update_state_no_passthrough);
  State_SetUpdate(top, update_state_no_passthrough);

  StateManager_Push(manager, bottom);
  StateManager_Push(manager, top);
  Memory *mBottom = bottom->memory;
  Memory *mTop = top->memory;
  ck_assert_int_eq(mBottom->n, 5);
  ck_assert_int_eq(mTop->n, 5);

  StateManager_Update(manager, 0);
  ck_assert_int_eq(mBottom->n, 5);
  ck_assert_int_eq(mTop->n, 10);

  StateManager_Free(manager);
}

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

  TCase *tc_state = tcase_create("State manipulation");
  suite_add_tcase(suite, tc_state);

  tcase_add_test(tc_state, init_destroy_memory);
  tcase_add_test(tc_state, update_passthrough);
  tcase_add_test(tc_state, update_no_passthrough);

  return suite;
}
