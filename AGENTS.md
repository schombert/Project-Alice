# Build and deployment notes

This repository produces the game executable used by `B:\VictoriaModernAges`.
The release must always contain three matching CPU variants:

| Build preset | Installed filename | CPU requirement |
| --- | --- | --- |
| `x64-release-sse-windows` | `AliceSSE.exe` | SSE4.2 |
| `x64-release-avx2-windows` | `Alice.exe` | AVX2 |
| `x64-release-avx512-windows` | `Alice512.exe` | AVX-512F |

The stock `launch_alice.exe` detects the processor and chooses these names in
that order. Build the launcher from the SSE4.2 preset and deploy it together
with the game executables whenever scenario data structures or parsers change.
The launcher serializes scenarios, so an old launcher is not compatible with a
new executable after a change to `src/gamestate/dcon_generated.txt`.

## Windows build

Configure and build each game variant:

```powershell
cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --preset x64-release-sse-windows'
cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build out/build/x64-release-sse-windows --config Release --target Alice --parallel 6'

cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --preset x64-release-avx2-windows'
cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build out/build/x64-release-avx2-windows --config Release --target Alice --parallel 6'

cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --preset x64-release-avx512-windows'
cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build out/build/x64-release-avx512-windows --config Release --target Alice --parallel 6'
```

Build the launcher once from the SSE4.2 tree:

```powershell
cmd /d /s /c 'call "C:\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build out/build/x64-release-sse-windows --config Release --target launch_alice --parallel 6'
```

The parser and data-container generators are intentionally compiled for
SSE4.2. This allows the build to run on a machine without AVX2 while still
producing the AVX2 and AVX-512 game variants.

The presets deliberately set `ALICE_ENABLE_LTCG=OFF` so all three compatible
executables can be produced promptly. Set it to `ON` only for a slower,
performance-focused rebuild; it does not affect format compatibility.

## Deployment and verification

1. Back up the four installed executables before replacing them.
2. Copy each generated `Release/Alice.exe` to the matching installed filename
   above, and copy the SSE-built `Launcher/Release/launch_alice.exe` to the
   game root.
3. Do not delete existing scenario cache files. Use the launcher to create a
   fresh scenario after a change to scenario-tagged data. The cache is at
   `%USERPROFILE%\Documents\Project Alice\scenarios`.
