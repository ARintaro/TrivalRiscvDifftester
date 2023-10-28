#pragma once

#include <array>
#include <cstdint>

namespace difftest {

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

using reg = u32;
using address = u32;
using msize = u32;


constexpr int INT_REGS_NUM = 32;

constexpr std::array<const char*, INT_REGS_NUM> REGS_NAME = {
	"$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
	"s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
	"a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
	"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

} // namespace difftest