# macrOS

> macOS macros (and utility functions).

- Avail.h: Extended macOS version macros,
  guaranteed to define every known macOS version constant.
  Additionally includes defines for macOS version ranges which a program will never run on,
  and version ranges older than what a program will always have access to.
  These ranges are based on the deployment target and architecture at compile time.
- FindVersion.h: Runtime macOS version availability macros,
  with compile time optimisation based on deployment target and architecture.
  Includes implementations of runtime macOS version checks
  for compilers which don't support \_\_builtin\_available.
- macrOSObjC.h: Objective-C helper macros, useful for calling common macOS APIs
  from non-Objective-C programs. Specialised Objective-C methods which are likely
  to be faster are used based on the deployment target.

Documentation for these macros and functions can be found in each file.

Tested across macOS 10.4 to 12.6,
with theoretical support for macOS 10.0 through to the present day.
Known to work on: ppc, i386, x86\_64, x86\_64h, arm64.

## Usage

macrOS uses [Boost's preprocessor library](https://www.boost.org/doc/libs/1_81_0/libs/preprocessor/doc/index.html),
to remove version checks at compile time.
It can be installed through the Homebrew package manager:
```bash
brew install boost
```
Or through MacPorts:
```bash
sudo port install boost
```

Java is used to generate the availability headers,
but it is not required to build with macrOS,
as the generated headers are part of the repository.
When the macOS SDK is updated to support new version of macOS,
the generator script will be edited to reflect any new constants.

macrOS can then be used as a subdirectory in a CMake project:
```cmake
# libraries/macrOS should be changed to where macrOS is.
add_subdirectory("libraries/macrOS")
target_link_libraries(${PROJECT_NAME} PRIVATE macrOS)
```

The [examples folder](examples) contains examples of how to use macrOS,
including calling Objective-C functions.
Build the examples (and this library) by running these commands:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j $(sysctl -n hw.logicalcpu)
```

## Why?

To allow writing C programs that natively target every version of macOS,
without technically using any Objective-C.

## Future work

- Foundation wrappers
- a header-only build
- a CMake install target
- better documentation / doxygen
- detecting and optimising against future version ranges
- testing on macOS pre-10.4 and ppc64

Issues and PRs are welcome.

## Credits

The Objective-C macros are based on
[tigr_objc.h](https://github.com/erkkah/tigr/blob/29f936484e7ac43bf79356bf62a86d28568b45bb/src/tigr_objc.h)
from [TIGR](https://github.com/erkkah/tigr),
a public domain cross-platform graphics library.

## License

macrOS is licensed under the BSD Zero Clause License.
