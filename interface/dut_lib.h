#pragma once

// difftest dynamic link library interface

#include <elf_reader.h>
#include <memory>

namespace difftest {

#ifndef difftest_main

#define EXPORT __attribute__ ((visibility("default")))

class TestCoreInterface;
class TestProgramConfig;

#ifdef __cplusplus
extern "C" {
#endif

TestCoreInterface* difftest_dut_init(const TestProgramConfig* config);

#ifdef __cplusplus
}
#endif


#else

using DifftestDutInitFunc = TestCoreInterface* (*)(const TestProgramConfig* config);

#endif

} // namespace difftest
