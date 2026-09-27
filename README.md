# AESDumpster

Cross-platform command-line tool that extracts the AES-256 encryption key from Unreal Engine 4/5 game executables (UE 4.19 – 5.3) by binary signature scanning. Candidates are ranked by Shannon entropy and the most likely key is highlighted.

## Build

Linux (g++ / make):

```bash
make
```

Windows (Visual Studio 2022, v143 toolset):

```powershell
msbuild msvc\AESDumpster.sln /p:Configuration=Release /p:Platform=x64
```

Cross-compile a Windows exe from Linux (mingw-w64): `make windows`.

## Usage

```bash
./aesdumpster <path-to-exe> [more exes...]
./aesdumpster --help
```

The tool reads the executable as raw bytes, so the Linux build analyzes Windows PE binaries exactly like the Windows build does. On Windows you can also drag and drop executables onto `AESDumpster.exe`.

## Tests

```bash
bash tests/run_tests.sh
```

Builds both the Linux and (via mingw/wine) Windows binaries and checks scanning against a synthetic fixture.

## Provenance

Unofficial cross-platform fork of [AESDumpster](https://github.com/GHFear/AESDumpster) by GHFear. The original work is © 2024–2025 GHFear and was published without an open-source license; all rights to the original code remain with the original author. See [NOTICE](NOTICE).

## License

The cross-platform modifications in this fork are MIT licensed — see [LICENSE](LICENSE).

Copyright © 2026 Nikita Radchenko for the fork modifications.
Original AESDumpster © 2024–2025 GHFear / IllusorySoftware, all rights reserved.
