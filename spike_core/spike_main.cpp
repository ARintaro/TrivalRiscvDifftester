#include "processor.h"
#include "elf_reader.h"
#include "spike_core.h"
#include "test_core.h"
#include <memory>

#include "dut_lib.h"

using namespace difftest;

extern "C" EXPORT TestCoreInterface* difftest_dut_init(const TestProgramConfig* config, const Elf* elf) {
	puts("[Spike Main] Init Start");

	auto* core = new SpikeCore(*config);
	elf->visit_alloc_sections([&](difftest::address addr, difftest::msize size, const difftest::u8* data) {
		core->write_memory(addr, size, data);
	});

	puts("[Spike Main] Success");


	return core;
}

