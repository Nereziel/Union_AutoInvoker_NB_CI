#pragma once
// Independent scheduling helpers; no ZenGin/Union dependencies.
// Requires C++11.
#include <cstdint>
#include <string>
#include <vector>
#include <cctype>

namespace nb_auto {
    struct Task {
        int index;
        std::uint32_t intervalMs;
        std::uint32_t lastTickMs;
    };

    // Format: AUTO_<positive milliseconds>_<name>. 1..86,400,000 ms.
    inline bool ParseAutoName(const std::string& name, std::uint32_t& intervalMs) {
        if (name.size() < 8) return false;
        const char* prefix = "AUTO_";
        for (unsigned i = 0; i != 5; ++i) {
            if (std::toupper(static_cast<unsigned char>(name[i])) != prefix[i]) return false;
        }
        std::uint64_t parsed = 0;
        std::size_t pos = 5;
        if (pos >= name.size() || !std::isdigit(static_cast<unsigned char>(name[pos]))) return false;
        while (pos < name.size() && std::isdigit(static_cast<unsigned char>(name[pos]))) {
            parsed = parsed * 10 + (name[pos] - '0');
            if (parsed > 86400000ULL) return false;
            ++pos;
        }
        if (parsed == 0 || pos + 1 >= name.size() || name[pos] != '_') return false;
        intervalMs = static_cast<std::uint32_t>(parsed);
        return true;
    }

    // GetTickCount() is a 32-bit monotonic counter with wrap-around.
    // Unsigned subtraction safely handles wrapping for our <=24h periods.
    inline bool IsDue(const Task& task, std::uint32_t now) {
        return static_cast<std::uint32_t>(now - task.lastTickMs) >= task.intervalMs;
    }
}
