#pragma once

#include <memory>
#include <optional>
#include <type_def.h>

namespace difftest {

struct VirtualDeviceConfig {
	address start;
	address end;

	[[nodiscard]] inline bool check_in(address addr) const {
		return addr >= start && addr <= end;
	}
};

class VirtualUart {
	// read a byte
	virtual std::optional<u8> read_byte();
	// write a byte, throw exception if failed
	virtual void write_byte(u8 byte);
};

struct VirtualDeviceHandlers {
	std::shared_ptr<VirtualUart> uart;
};

} // namespace difftest

