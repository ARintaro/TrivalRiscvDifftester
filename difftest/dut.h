#pragma once

#include "bin.h"
#include "difftest.h"
#include "dut_lib.h"
#include "virtual_device.h"


class DutLib {
	void* lib_handle;

public:
	DifftestDutInitFunc init_func;

	DutLib(const DutLib &) = delete;
	DutLib(DutLib &&) = delete;
	DutLib &operator=(const DutLib &) = delete;
	DutLib &operator=(DutLib &&) = delete;

	explicit DutLib(const std::string& path);
	~DutLib();
};

class Dut {
public:
	// 动态库句柄放在最上面，最后被析构
	DutLib lib;

	std::unique_ptr<TestCoreInterface> core;
	VirtualDeviceHandlers devices;

	Dut(const std::string &path, const TestProgramConfig &config);

	void write_memory(const Elf& elf);

	void write_memory(address addr, const BinFile& bin);
};