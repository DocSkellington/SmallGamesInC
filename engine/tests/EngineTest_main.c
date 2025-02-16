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
