#include "EngineTest.h"
#include <check.h>
#include <stdlib.h>

int main(void) {
  setup();
  SRunner *runner = srunner_create(makeStateManagerSuite());
  srunner_run_all(runner, CK_VERBOSE);
  clean();
  int numberFailed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (numberFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
