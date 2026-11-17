#include "../src/AutoInvokerCore.hpp"
#include <cassert>
#include <cstdint>
#include <string>
int main() {
    std::uint32_t ms = 0;
    assert(nb_auto::ParseAutoName("Auto_1000_ManaRegen", ms) && ms == 1000);
    assert(nb_auto::ParseAutoName("AUTO_500_Heal", ms) && ms == 500);
    assert(nb_auto::ParseAutoName("Auto_86400000_Long", ms) && ms == 86400000);
    assert(!nb_auto::ParseAutoName("Auto_0_Do", ms));
    assert(!nb_auto::ParseAutoName("Auto_-3_Do", ms));
    assert(!nb_auto::ParseAutoName("Auto_1000", ms));
    assert(!nb_auto::ParseAutoName("Auto_99999999999999999999_Overflow", ms));
    assert(nb_auto::ParseAutoName("Auto_100_Do", ms) && ms == 100);
    nb_auto::Task t{ 1, 1000, 100 };
    assert(!nb_auto::IsDue(t, 1099));
    assert(nb_auto::IsDue(t, 1100));
    nb_auto::Task wrap{ 1, 1000, 0xFFFFFF00u };
    assert(!nb_auto::IsDue(wrap, 0x00000100u));
    assert(nb_auto::IsDue(wrap, 0x00000400u));
}
