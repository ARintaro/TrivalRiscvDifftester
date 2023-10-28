#pragma once

#include "difftest.h"
#include "dut_lib.h"


class Dut {
public:
	std::unique_ptr<TestCoreInterface> core;
	void* lib_handle;

	Dut(const Dut &) = delete;
	Dut(Dut &&) = delete;
	Dut &operator=(const Dut &) = delete;
	Dut &operator=(Dut &&) = delete;
	
	Dut(const std::string &path, const TestProgramConfig &config,
		const Elf &elf);
	~Dut();
};