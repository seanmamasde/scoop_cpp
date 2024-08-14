# scoop_cpp

Fast replacement for `scoop` commands, written in C++.

> This is my first time doing a normal C++ project (not a competitive programming one), so I'm still learning.
> Please feel free to open an issue or a PR if you see something wrong or could be improved.

## Features

> [!CAUTION]
> This project is still under active development, not all commands are implemented.
> **Use it at your own risk.**

|            Command             | Implemented?  |
| :----------------------------: | :-----------: |
| [`search`](./manual/search.md) |      Yes      |
|             `list`             | Working on it |

> [!NOTE]
> Unimplemented commands will be show a `Not implemented yet` error.
> It will **not** be forwarded to the original `scoop` executable.

## Building

You will need Visual Studio 2022 and MSVC installed, with `vcpkg` configured in manifest mode.

## Todos

- [ ] Add CI/CD pipeline (idk how to do this properly).
- [ ] Add releases on github (still figuring out).
- [ ] Add to scoop buckets  (when CI/CD pipelines are configured).
