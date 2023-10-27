#pragma once

#include "type_def.h"
#include <array>

namespace Difftest {

class RiscvCoreState {
public:
	std::array<reg, INT_REGS_NUM> intRegs;
	reg pc;

	
};

class TestCore {};

} // namespace Difftest
