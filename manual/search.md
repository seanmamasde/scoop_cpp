# Search

> **Acknowledgement: This project is inspired by [scoop-search](https://github.com/shilangyu/scoop-search)**

## Features

- Multithreaded search.
- Does the exact same thing as `scoop search`, but faster. 
- The output format is a bit different from the original command, but queried word is colorized.
- Added `--regex` flag to enable search with regex.

## Installation

Manually download the latest release from the release section or build it yourself, and add it to `PATH`.

> [!NOTE]
> Still working on configuring the CI/CD pipeline and the release process.

## PowerShell hook

You do not have to use the `scoop_cpp.exe search ...` to look for a package/binary, instead put

```powershell
Invoke-Expression (&scoop_search_cpp.exe --hook)
```

in your PowerShell profile, and it will run whenever you use `scoop search ...`.

## Command Prompt wrapper

Still working on this.
<!-- ```cmd
@echo off

if "%1" == "search" (
    call :search_subroutine %*
) else (
    powershell scoop.ps1 %*
)
goto :eof

:search_subroutine
set "args=%*"
set "newargs=%args:* =%"
scoop_search_cpp.exe %newargs%
goto :eof
``` -->

## Benchmark

Tested with:

- Windows 24H2, build 26100.1297
- AMD R9 5900HS @3.30GHz, 32Gb RAM
- `scoop` commit `be56faf2`
- PowerShell 7.5.0-preview.3
- hyperfine v1.18.0

Testing against the built-in `scoop search` command:

```PowerShell
> hyperfine --warmup 1 ".\scoop_cpp.exe search google" "scoop search google"
Benchmark 1: .\scoop_cpp.exe search google
  Time (mean ± σ):      78.4 ms ±   2.3 ms    [User: 289.7 ms, System: 340.6 ms]
  Range (min … max):    74.4 ms …  82.6 ms    20 runs

Benchmark 2: scoop search google
  Time (mean ± σ):      2.329 s ±  0.025 s    [User: 0.057 s, System: 0.052 s]
  Range (min … max):    2.294 s …  2.370 s    10 runs

Summary
  .\scoop_cpp.exe search google ran
   29.71 ± 0.93 times faster than scoop search google
```

Testing against the [scoop-search](https://github.com/shilangyu/scoop-search) project:

```PowerShell
> hyperfine --warmup 1 ".\scoop_cpp.exe search google" "scoop-search google"
Benchmark 1: .\scoop_cpp.exe search google
  Time (mean ± σ):      77.0 ms ±   3.3 ms    [User: 312.1 ms, System: 341.3 ms]
  Range (min … max):    74.3 ms …  88.0 ms    19 runs

Benchmark 2: scoop-search google
  Time (mean ± σ):      93.8 ms ±   5.6 ms    [User: 250.1 ms, System: 287.3 ms]
  Range (min … max):    87.6 ms … 104.5 ms    17 runs

Summary
  .\scoop_cpp.exe search google ran
    1.22 ± 0.09 times faster than scoop-search google
```
