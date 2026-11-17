#pragma once
#include <cstdint>
extern std::uint32_t mockClock;
inline std::uint32_t GetTickCount() { return mockClock; }
