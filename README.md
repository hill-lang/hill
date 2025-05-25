# Hill language [![License](https://img.shields.io/github/license/hill-lang/hill.svg)](https://github.com/hill-lang/hill/blob/master/LICENSE)

[![Build Ubuntu](https://github.com/hill-lang/hill/actions/workflows/build-ubuntu.yml/badge.svg)](https://github.com/hill-lang/hill/actions/workflows/build-ubuntu.yml) [![Build MacOS](https://github.com/hill-lang/hill/actions/workflows/build-macos.yml/badge.svg)](https://github.com/hill-lang/hill/actions/workflows/build-macos.yml) [![Build Windows](https://github.com/hill-lang/hill/actions/workflows/build-windows.yml/badge.svg)](https://github.com/hill-lang/hill/actions/workflows/build-windows.yml)
[![Test Unit](https://github.com/hill-lang/hill/actions/workflows/test-unit.yml/badge.svg)](https://github.com/hill-lang/hill/actions/workflows/test-unit.yml)

A static typed scripting language in its very early stage

## Getting started

### Project generation

This project uses [Premake](https://premake.github.io) to generate build files. Here is an example of how to generate a Visual Studio solution:

```bash
premake5 vs2022
```

## GCC

This project depends on functionality that is not available in GCC versions prior to 13.
