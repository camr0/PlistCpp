# Building

## System Dependencies

This project is setup to be built using [vcpkg](https://github.com/Microsoft/vcpkg),
make sure you have it setup as per the install instructions, mainly setting the
VCPKG_ROOT environment variable.

## Building and running the tests (i.e not as a dependency via vcpkg)

* `mkdir build && cd build`
* `cmake .. --preset osx-static-dev` (or other suitable config, see `cmake .. --list-presets`)
* `cmake --build .`
* `cd test`
* `./test/test -s`
