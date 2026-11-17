// Union AutoInvoker NB – independent reimplementation (NOT original DLL source)
// Designed for the Patrix9999/union-plugin-template Gothic II Addon build.
// Plugin.ipp is included by the template for each selected game version.
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>
#include "AutoInvokerCore.hpp"

namespace GOTHIC_NAMESPACE
{
    namespace
    {
        std::vector<nb_auto::Task> tasks;
        zCParser* registeredParser = nullptr;
        int registeredSymbolCount = -1;
        bool callingDaedalus = false;

        void ResetAutoInvoker()
        {
            tasks.clear();
            registeredParser = nullptr;
            registeredSymbolCount = -1;
            callingDaedalus = false;
        }

        void RegisterAutoFunctions()
        {
            tasks.clear();
            if (!parser) return;

            registeredParser = parser;
            registeredSymbolCount = parser->symtab.GetNumInList();
            const auto now = static_cast<std::uint32_t>(::GetTickCount());
            for (int index = 0; index < registeredSymbolCount; ++index)
            {
                zCPar_Symbol* symbol = parser->GetSymbol(index);
                if (!symbol || symbol->type != zPAR_TYPE_FUNC) continue;

                std::uint32_t intervalMs = 0;
                if (!nb_auto::ParseAutoName(symbol->name.ToChar(), intervalMs)) continue;
                // AutoInvoker only calls functions without script parameters.
                if (symbol->ele != 0) continue;

                tasks.push_back({index, intervalMs, now});
            }
        }

        void TickAutoInvoker()
        {
            if (callingDaedalus || !parser || !ogame || !oCNpc::player)
                return;

            const int symbolCount = parser->symtab.GetNumInList();
            if (registeredParser != parser || registeredSymbolCount != symbolCount)
                RegisterAutoFunctions();

            const auto now = static_cast<std::uint32_t>(::GetTickCount());
            for (auto& task : tasks)
            {
                if (!nb_auto::IsDue(task, now)) continue;
                task.lastTickMs = now; // skip catch-up calls after pause/lag/loading
                callingDaedalus = true;
                // Daedalus executes on the Gothic main thread. The script is
                // parameterless and refers to HERO; no NPC context rewriting.
                parser->CallFunc(task.index);
                callingDaedalus = false;
            }
        }
    }

    void Game_Init()
    {
        ResetAutoInvoker();
    }

    void Game_Exit()
    {
        ResetAutoInvoker();
    }

    void Game_Loop()
    {
        TickAutoInvoker();
    }

    // Official Union event hooks from the current Gothic Union template.
    // The world render partial hook is once per rendered frame; avoid the old
    // oCAniCtrl_Human::Loop hook, which may run per animation/NPC.
    void __fastcall oCGame_Init(oCGame* self, void* vtable);
    auto Hook_NB_Init = ::Union::CreateHook(
        SIGNATURE_OF(&oCGame::Init),
        &oCGame_Init,
        ::Union::HookType::Hook_Detours);
    void __fastcall oCGame_Init(oCGame* self, void* vtable)
    {
        Hook_NB_Init(self, vtable);
        Game_Init();
    }

    void __fastcall CGameManager_Done(CGameManager* self, void* vtable);
    auto Hook_NB_Exit = ::Union::CreateHook(
        SIGNATURE_OF(&CGameManager::Done),
        &CGameManager_Done,
        ::Union::HookType::Hook_Detours);
    void __fastcall CGameManager_Done(CGameManager* self, void* vtable)
    {
        Game_Exit();
        Hook_NB_Exit(self, vtable);
    }

    void __fastcall oCGame_MainWorld_Render(::Union::Registers& reg);
    auto Partial_NB_MainWorldRender = ::Union::CreatePartialHook(
        reinterpret_cast<void*>(zSwitch(0, 0, 0, 0x006C87EB)),
        &oCGame_MainWorld_Render);
    void __fastcall oCGame_MainWorld_Render(::Union::Registers& reg)
    {
        Game_Loop();
    }
}
