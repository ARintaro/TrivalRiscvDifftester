#pragma once

#include "csrs.h"
#include "debug_module.h"
#include "test_core.h"
#include "sim.h"
#include "virtual_device.h"
#include <memory>

using namespace difftest;

class SpikeCore : public TestCoreInterface {
public:
	explicit SpikeCore(const TestProgramConfig& config);

	void step(u64 n) override;

	[[nodiscard]] const RiscvCoreState& get_state() const override;

	void write_memory(address addr, msize size, const u8 *data) override;

	u32 read_memory_word(address addr) override;

	void send_interupt(reg cause) override;

	VirtualDeviceHandlers get_device_handlers() override;

	~SpikeCore() override = default;


private:
	std::unique_ptr<sim_t> sim;

	processor_t* proc;
	state_t* state;

	std::vector<std::string> args;
	std::unique_ptr<debug_module_config_t> debug_config;
	std::unique_ptr<mem_t> mem;

	RiscvCoreState extern_state;
	VirtualDeviceHandlers device_handlers;


	void sync_extern_state();

	void init_device_handlers(const TestProgramConfig& config);
};