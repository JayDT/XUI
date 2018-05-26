# ACMake

**A**nsweror' **CMake**. Just some useful snippets.

## Installation

```cmake
list(APPEND CMAKE_MODULE_PATH PATH_TO_ACMAKE)
```

Then include necessary `.cmake` files.


## Useful snippets

### acmake\_boost\_support

* Include header files;
* link against header only boost or shared library version;
* define macros to avoid hardcoded library related `pragma`;
* add C++0x `decltype` support for VS2010.

### acmake\_qt\_support

* Include header files;
* find libs;
* auto moc `.h`, `.hpp`, `.rc`, `.ui`;
