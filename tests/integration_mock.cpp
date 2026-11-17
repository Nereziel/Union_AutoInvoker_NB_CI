// C++ integration mock: not a real Union SDK compilation.
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#define GOTHIC_NAMESPACE MockGothic
#define __fastcall
#define zSwitch(a,b,c,d) (d)
#define SIGNATURE_OF(method) ((void*)0x1234)
namespace Union {
    enum class HookType { Hook_Detours };
    struct Registers {};
    template<typename T> struct FakeHook {
        template<class ...A> void operator()(A&& ...) {}
    };
    template<typename Fn> FakeHook<Fn> CreateHook(void*, Fn, HookType) { return {}; }
    template<typename Fn> FakeHook<Fn> CreatePartialHook(void*, Fn) { return {}; }
}
namespace MockGothic {
    enum { zPAR_TYPE_FUNC=5, zPAR_TYPE_INT=2 };
    struct zSTRING { std::string raw; const char* ToChar() { return raw.c_str(); } };
    struct zCPar_Symbol { zSTRING name; int type; int ele; };
    struct zCParser {
        std::vector<zCPar_Symbol> syms;
        std::vector<int> calls;
        struct SymbolTable { std::vector<zCPar_Symbol>* owner; int GetNumInList() { return (int)owner->size(); } } symtab;
        zCParser() : symtab{&syms} {}
        zCPar_Symbol* GetSymbol(int n) { return n>=0 && n<(int)syms.size() ? &syms[n] : nullptr; }
        void CallFunc(int n) { calls.push_back(n); }
    };
    struct oCNpc { static oCNpc* player; };
    oCNpc* oCNpc::player=nullptr;
    struct oCGame { void Init(){} };
    struct CGameManager { void Done(){} };
    oCGame* ogame=nullptr;
    zCParser* parser=nullptr;
}
std::uint32_t mockClock=0;
#include "../src/Plugin.ipp"
int main() {
    using namespace MockGothic;
    zCParser p;
    p.syms.push_back({ {"Auto_1000_ManaRegen"}, zPAR_TYPE_FUNC, 0 });
    p.syms.push_back({ {"AUTO_500_OTHER"}, zPAR_TYPE_FUNC, 0 });
    p.syms.push_back({ {"AUTO_20_NEEDSARG"}, zPAR_TYPE_FUNC, 1 });
    parser=&p; oCGame game; ogame=&game; oCNpc player; oCNpc::player=&player;
    Game_Init();
    Game_Loop(); assert(p.calls.size()==0);
    mockClock=500; Game_Loop(); Game_Loop(); assert(p.calls.size()==1 && p.calls[0]==1);
    mockClock=1000; Game_Loop(); assert(p.calls.size()==3 && p.calls[1]==0 && p.calls[2]==1);
    mockClock=1100; p.syms.push_back({ {"AUTO_200_NEW"}, zPAR_TYPE_FUNC, 0 }); Game_Loop();
    mockClock=1300; Game_Loop(); assert(p.calls.back()==3);
    auto n = p.calls.size(); oCNpc::player=nullptr; mockClock=2500; Game_Loop(); assert(p.calls.size()==n);
    Game_Exit();
}
