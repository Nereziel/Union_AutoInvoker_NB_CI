# Union AutoInvoker NB — source and Windows x86 CI build

**Status: GitHub Actions Windows x86 build passed (run #2, commit `6eebf15`); not yet tested in New Balance.**

[Download the verified build artifact from run #2](https://github.com/Nereziel/Union_AutoInvoker_NB_CI/actions/runs/44107881661/artifacts/14339068024). DLL: PE32 x86 (61,440 bytes).
Independent implementation, not the original `Union_AutoInvoker.dll` source.

## Recommended for New Balance: Daedalus-only (no third-party DLL)

Current New Balance builds may reject unknown Union DLL plugins even when they compile and link successfully. If the game reports **Not Compatible**, do not force plugin loading by renaming DLLs or modifying Union core settings.

Use [`scripts/ManaRegen_NoDLL.d`](scripts/ManaRegen_NoDLL.d) instead. Place this **one file** in `Gothic II/System/Autorun/`, and remove old `Union_AutoInvoker.dll`, `Union_AutoInvoker_NB.dll` and any older `ManaRegen.d` from active locations. Keep other working mods (such as your XP multiplier).

This version uses zParserExtender's built-in `GameLoop()` callback only to register a persistent `AI_StartTriggerScript("NB_ManaRegen_Tick", 1000)` timer. It detects existing timers via `AI_GetNextTriggerByFuncName` to avoid duplicates after loading a save. No FPS-dependent mana calculations and no additional DLL or INI edits.

Regeneration: 1% of maximum mana per second, minimum 1 point, only if `Npc_GetTalentSkill(hero, NPC_TALENT_MAGE) >= 1`; full HP is **not** required. It deliberately does **not** enable regeneration for every class with mana.

**Status:** script API verified in zParserExtender documentation/source; **not tested inside the user's New Balance build**. If it fails to load, capture the exact parser error and disable only this file.

References:
- https://gothic-modding-community.github.io/gmc/zengin/scripts/extenders/zparserextender/externals/events_vars/
- https://auronen.cokoliv.eu/gmc/cs/zengin/scripts/extenders/zparserextender/classes/c_trigger/

---

## Build through GitHub Actions (Windows x86)
1. Open **Actions → Build Union AutoInvoker NB (Win32)** in this repository.
2. Download the successful **Union_AutoInvoker_NB-Win32** artifact from run #2, or click **Run workflow** to rebuild.
3. Extract the ZIP containing the DLL and `scripts/ManaRegen.d`.
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
