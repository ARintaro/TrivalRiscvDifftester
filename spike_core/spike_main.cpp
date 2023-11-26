#include "processor.h"
#include "elf_reader.h"
#include "spike_core.h"
#include "test_core.h"
#include <memory>

#include "dut_lib.h"

using namespace difftest;

extern "C" EXPORT TestCoreInterface* difftest_dut_init(const TestProgramConfig* config) {
	puts("[Spike Main] Init Start");

	auto* core = new SpikeCore(*config);

	puts("[Spike Main] Success");


	return core;
}

