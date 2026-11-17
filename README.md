# Union AutoInvoker NB — build-ready source package, not compiled

**Status: NOT built against Union Framework or tested in New Balance.**
Independent implementation, not the original `Union_AutoInvoker.dll` source.

## Build through GitHub Actions (Windows x86)
1. Create a GitHub repository and upload all files from this ZIP, **including `.github/workflows/build-windows.yml`**.
2. Open **Actions → Build Union AutoInvoker NB (Win32) → Run workflow**.
3. If successful, download artifact **Union_AutoInvoker_NB-Win32**. Contains DLL and `ManaRegen.d`.
4. Back up the game and save files, **remove the old AutoInvoker DLL**, then install `Union_AutoInvoker_NB.dll` and `ManaRegen.d` into `Gothic II/System/Autorun/`.
5. Test a character with a learned magic circle. If the game crashes or fails to start, remove the DLL.

## Build on Windows PC
Install Visual Studio C++ workload, Windows x86 build tools, Git, CMake and Ninja.
Open **x86 Native Tools Command Prompt for VS**, then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Build script obtains the current public `Patrix9999/union-plugin-template` with its recursive Git submodules (Union API and Gothic API), patches CMake to link Union statically, and compiles **G2A x86**. The output is `Union_AutoInvoker_NB.dll`.

## Behavior
- Schedules parameterless Daedalus functions called `Auto_<milliseconds>_<Name>`.
- Checks only the Game parser. Supports multiple `Auto_` functions.
- Wall-clock timing via `GetTickCount`, 1..86,400,000 milliseconds.
- World-render hook, not per-NPC animation-loop hook.
- Rescans when the parser pointer/symbol count changes.
- Does not catch up with multiple calls after lag/load.
- Calls functions only if the world, game parser and player exist.

## Caveats
- The 2020 original DLL's incompatibility cause is unknown. This rewrite is **not** verified to resolve it.
- Current Union Framework/Gothic API may change method signatures and require C++ fixes in `src/Plugin.ipp`; this build package is a best effort, not a guarantee.
- Hook address assumes standard G2A binary. New Balance may patch engine functions.
- Do not run both AutoInvokers simultaneously.
- Callback parameters, pause/menu, save/load transitions and engine hooks need in-game testing.
- `Npc_GetTalentSkill(hero, NPC_TALENT_MAGE)` detects magic circles, not character classes.

Sources: https://github.com/Patrix9999/union-plugin-template and https://modding.sefaris.eu/docs/modding-basics-union/first-plugin
