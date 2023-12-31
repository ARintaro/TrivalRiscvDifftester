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
	reg mtvec;
	reg mepc;
	reg mie;
	reg mip;
	reg mscratch;
	reg mtval;
	reg medeleg;
	reg mideleg;
	reg mhartid;

	reg sstatus;
	reg scause;
	reg stvec;
	reg sepc;
	reg sie;
	reg sip;
	reg sscratch;
	reg stval;
	reg satp;

	address last_pc;
	reg last_inst;

	inline void print() const {
		fprintf(stderr, "last_pc: 0x%08x, pc: 0x%08x\n", last_pc, pc);
		for (int i = 0; i < INT_REGS_NUM; ++i) {
			fprintf(stderr, "%s: 0x%08x | ", REGS_NAME[i], int_regs[i]);
			if ((i + 1) % 8 == 0) fprintf(stderr, "\n");
		}
		
		// print csrs, 3 in a line
		fprintf(stderr, "CSR REGISTERS :\n");
		fprintf(stderr, "cur_prv: %d\n", cur_prv);
		fprintf(stderr, "mstatus: 0x%08x, mcause: 0x%08x, mtvec: 0x%08x\n", mstatus, mcause, mtvec);
		fprintf(stderr, "mepc: 0x%08x, mie: 0x%08x, mip: 0x%08x\n", mepc, mie, mip);
		fprintf(stderr, "mscratch: 0x%08x, mtval: 0x%08x, medeleg: 0x%08x\n", mscratch, mtval, medeleg);
		fprintf(stderr, "mideleg: 0x%08x, mhartid: 0x%08x\n", mideleg, mhartid);
		fprintf(stderr, "sstatus: 0x%08x, scause: 0x%08x, stvec: 0x%08x\n", sstatus, scause, stvec);
		fprintf(stderr, "sepc: 0x%08x, sie: 0x%08x, sip: 0x%08x\n", sepc, sie, sip);
		fprintf(stderr, "sscratch: 0x%08x, stval: 0x%08x, satp: 0x%08x\n", sscratch, stval, satp);

		fprintf(stderr, "\n");
	}

	inline bool operator != (const RiscvCoreState& rhs) const {
		if (last_pc != rhs.last_pc) return true;
		if (pc != rhs.pc) return true;
		for (int i = 0; i < INT_REGS_NUM; ++i) {
			if (int_regs[i] != rhs.int_regs[i]) return true;
		}
		if (mstatus != rhs.mstatus) return true;
		if (mcause != rhs.mcause) return true;
		if (mtvec != rhs.mtvec) return true;
		if (mepc != rhs.mepc) return true;
		if (mie != rhs.mie) return true;
		// if (mip != rhs.mip) return true;
		if (mscratch != rhs.mscratch) return true;
		// if (mtval != rhs.mtval) return true;
		if (medeleg != rhs.medeleg) return true;
		if (mideleg != rhs.mideleg) return true;
		if (mhartid != rhs.mhartid) return true;
		if (sstatus != rhs.sstatus) return true;
		if (scause != rhs.scause) return true;
		if (stvec != rhs.stvec) return true;
		if (sepc != rhs.sepc) return true;
		// if (sie != rhs.sie) return true;
		// if (sip != rhs.sip) return true;
		if (sscratch != rhs.sscratch) return true;
		// if (stval != rhs.stval) return true;
		// if (satp != rhs.satp) return true;
		if (cur_prv != rhs.cur_prv) return true;
		return false;
	}
};

struct TestProgramConfig {
	bool debug;
	address start_pc;
	address ram_base;
	msize memory_size;
	

	std::optional<VirtualDeviceConfig> uart_config;
};

struct TestCoreStepResult {
	u32 step_num;
	bool uncertern;
};

class TestCoreInterface {
public:
  virtual TestCoreStepResult step() = 0;

  [[nodiscard]] virtual const RiscvCoreState &get_state() const = 0;

  virtual void set_state(const RiscvCoreState &state) = 0;

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


