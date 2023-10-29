#pragma once

#include "type_def.h"
#include "virtual_device.h"
#include <array>
#include <cstdio>
#include <optional>

namespace difftest {

// All interface won't be called while dut is stepping.

struct RiscvCoreState {
	std::array<reg, INT_REGS_NUM> int_regs;
	address pc;
	reg cur_prv;

	reg mstatus;
	reg mcause;
	reg mtval;
	reg mepc;	

	address last_pc;
	reg last_inst;

	inline void print() const {
		puts("===========");
		printf("next_pc: 0x%08x\n");
		for (int i = 0; i < INT_REGS_NUM; ++i) {
			printf("%s: 0x%08x | ", REGS_NAME[i], int_regs[i]);
			if ((i + 1) % 8 == 0) puts("");
		}
	}
};

struct TestProgramConfig {
	bool debug;
	address start_pc;
	address ram_base;
	msize memory_size;
	

	std::optional<VirtualDeviceConfig> uart_config;
};


class TestCoreInterface {
public:
  virtual void step(u64 n) = 0;

  [[nodiscard]] virtual const RiscvCoreState &get_state() const = 0;

  virtual void write_memory(address addr, msize size, const u8 *data) = 0;

  virtual u32 read_memory_word(address addr) = 0;

  virtual void send_interupt(reg cause) = 0;

  virtual VirtualDeviceHandlers get_device_handlers() = 0;

  virtual ~TestCoreInterface() = default;

  TestCoreInterface() = default;
  TestCoreInterface(const TestCoreInterface &) = delete;
  TestCoreInterface(TestCoreInterface &&) = default;
  TestCoreInterface &operator=(const TestCoreInterface &) = delete;
  TestCoreInterface &operator=(TestCoreInterface &&) = default;
};

} // namespace difftest

