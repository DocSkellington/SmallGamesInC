# Collection of small games written in C

The point of this project was to re-familiarize myself with the C programming language (using SDL3 to handle the rendering and the window). While the games work correctly, they are not refined. In particular, the graphics are simple colored rectangles, and lack animations and juice. I do not intend on improving them in a near future (more games may appear if I feel like it).

This repository is split into three parts:

  - A clone of Flappy Bird.
  - A small game engine that lacks many functionalities.
  - A clone of Frogger built upon the engine, called Crossing Roads.

## Dependencies, Building, and Running

The project has the following dependencies:

  - SDL3, which is included in this repository as a git submodule.
  - [GLib](https://docs.gtk.org/glib/index.html).
  - The [Check](https://libcheck.github.io/check/) unit test framework.
  - Optionally, Doxygen with dot to build the documentation of the engine.

The project relies on CMake to handle the build process.
For instance, to build the Crossing Roads game.

```bash
mkdir build && cd build && cmake .. && make CrossingRoads
```

While the clone of Flappy Bird can be played from its build directory, Crossing Roads require external resources. Either copy the folder `2 - CrossingRoads/resources/` into the build folder, or start the executable from `2 - CrossingRoads/resources/`.

## License

This project is distributed under GPLv3.
See [LICENSE] for more details.
