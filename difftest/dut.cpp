#include "dut.h"
#include "test_core.h"
#include <dlfcn.h>
#include <memory>


Dut::Dut(const std::string& path, const TestProgramConfig& config, const Elf& elf) : lib(path) {
	core = std::unique_ptr<TestCoreInterface>(lib.init_func(&config, &elf));
	devices = core->get_device_handlers();
}

DutLib::~DutLib() {
	dlclose(lib_handle);
}

DutLib::DutLib(const std::string& path) {
	lib_handle = dlopen(path.c_str(), RTLD_NOW);

	if (lib_handle == nullptr) {
		throw std::runtime_error("Cannot open library: " + std::string(dlerror()));
	}

	dlerror();

	init_func = reinterpret_cast<DifftestDutInitFunc>(dlsym(lib_handle, "difftest_dut_init"));

	const char* dlsym_error = dlerror();
	if (dlsym_error != nullptr) {
		dlclose(lib_handle);
		throw std::runtime_error("Cannot load symbol 'difftest_dut_init ': " + std::string(dlsym_error));
	}
}