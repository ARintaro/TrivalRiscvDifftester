#include "spike_core.h"
#include "cfg.h"
#include "devices.h"
#include "mmu.h"
#include "sim.h"
#include "type_def.h"
#include "spike_uart.h"

#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

SpikeCore::SpikeCore(const TestProgramConfig& config) {
	const char *isa = "RV32IMAFDC";

	cfg_t cfg(
		std::make_pair(static_cast<reg_t>(0), static_cast<reg_t>(0)),
		nullptr,
		isa,
		"MSU",
		"vlen:128,elen:64",
		false,
		endianness_little,
		16,
		std::vector<mem_cfg_t>(),
		std::vector<size_t>(1),
		false,
		4
	);

	args.emplace_back("");
	debug_config = std::make_unique<debug_module_config_t>(debug_module_config_t {
		.progbufsize = 2,
		.max_sba_data_width = 0,
		.require_authentication = false,
		.abstract_rti = 0,
		.support_hasel = true,
		.support_abstract_csr_access = true,
		.support_abstract_fpr_access = true,
		.support_haltgroups = true,
		.support_impebreak = true
	});

	mem = std::make_unique<mem_t>(config.memory_size);

	sim = std::make_unique<sim_t>(&cfg,
		false,
		std::vector<std::pair<reg_t, abstract_mem_t*>>(
			1, std::make_pair(static_cast<reg_t>(config.ram_base), mem.get())),
		std::vector<const device_factory_t*>(),
		args,
		*debug_config,
		nullptr,
		false,
		nullptr,
		false,
		nullptr);
	
	proc = sim->get_core(0);
	state = proc->get_state();

	state->pc = config.start_pc;	
	proc->set_privilege(3, false);

	sync_extern_state();

	init_device_handlers(config);
}

void SpikeCore::step(u64 n) {
	proc->step(n);
	sync_extern_state();
}

const RiscvCoreState& SpikeCore::get_state() const {
	return extern_state;
}

void SpikeCore::write_memory(address addr, msize size, const u8 *data){
	for (msize i = 0; i < size; i++) {
		proc->get_mmu()->store<u8>(addr + i, data[i]);
	}
}

u32 SpikeCore::read_memory_word(address addr) {
	return proc->get_mmu()->load<u32>(addr);
}

void SpikeCore::send_interupt(reg cause) {
	
}

void SpikeCore::init_device_handlers(const TestProgramConfig& config) {
	if (config.uart_config.has_value()) {
		auto uart = std::make_unique<SpikeUart>(config.uart_config.value());
		device_handlers.uart = std::move(uart);
	}
}

VirtualDeviceHandlers SpikeCore::get_device_handlers() {
	return device_handlers;
}

void SpikeCore::sync_extern_state() {
	for (int i = 0; i < INT_REGS_NUM; i++) {
		extern_state.int_regs[i] = state->XPR[i];
	}

	extern_state.pc = state->pc;
	extern_state.cur_prv = state->prv;

	extern_state.mstatus = state->mstatus->read();
	extern_state.mcause = state->mcause->read();
	extern_state.mtval = state->mtval->read();
	extern_state.mepc = state->mepc->read();
}





