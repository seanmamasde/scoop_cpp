# Search

> **Acknowledgement: This project is inspired by [scoop-search](https://github.com/shilangyu/scoop-search)**

## Features

- Multithreaded search.
- Does the exact same thing as `scoop search`, but faster. The output format is a bit different, though.
  > [!IMPORTANT]
  > The output will be the same as the original `scoop search` command in the future.

## Installation

Manually download the latest release from the release section or build it yourself, and add it to `PATH`.

> [!NOTE]
> Still working on configuring the CI/CD pipeline and the release process.

## PowerShell hook

You do not have to use the `scoop_cpp.exe search ...` to look for a package/binary, instead put

```powershell
Invoke-Expression (&scoop_search_cpp.exe --hook)
```

in your PowerShell profile, and it will run whenever you use `scop search ...`.

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
> hyperfine --warmup 1 'scoop_search_cpp.exe search google' 'scoop search google'
Benchmark 1: C:\Users\seanma\source\projects\scoop_cpp\x64\Release\scoop_search_cpp.exe search google
  Time (mean ± σ):     111.8 ms ±   5.8 ms    [User: 360.8 ms, System: 402.6 ms]
  Range (min … max):   103.8 ms … 127.1 ms    14 runs

Benchmark 2: scoop search google
  Time (mean ± σ):      3.243 s ±  0.050 s    [User: 0.060 s, System: 0.095 s]
  Range (min … max):    3.195 s …  3.359 s    10 runs

Summary
  C:\Users\seanma\source\projects\scoop_cpp\x64\Release\scoop_search_cpp.exe search google ran
   28.99 ± 1.58 times faster than scoop search google
```

Testing against the [scoop-search](https://github.com/shilangyu/scoop-search) project:

```PowerShell
> hyperfine --warmup 1 'scoop_search_cpp.exe search google' 'scoop-search google'
Benchmark 1: C:\Users\seanma\source\projects\scoop_cpp\x64\Release\scoop_search_cpp.exe search google
  Time (mean ± σ):     102.4 ms ±   2.9 ms    [User: 394.0 ms, System: 436.0 ms]
  Range (min … max):    99.7 ms … 110.0 ms    14 runs

Benchmark 2: scoop-search google
  Time (mean ± σ):     143.6 ms ±   9.4 ms    [User: 375.4 ms, System: 469.3 ms]
  Range (min … max):   134.0 ms … 161.1 ms    12 runs

Summary
  C:\Users\seanma\source\projects\scoop_cpp\x64\Release\scoop_search_cpp.exe search google ran
    1.40 ± 0.10 times faster than scoop-search google
```
