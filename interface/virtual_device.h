#pragma once

#include <memory>
#include <optional>
#include <type_def.h>

namespace difftest {

struct VirtualDeviceConfig {
	address start;
	address size;

};

class VirtualUart {
public:
	// read a byte
	virtual std::optional<u8> host_read_byte() = 0;
	// write a byte, throw exception if failed
	virtual bool host_write_byte(u8 byte) = 0;

	virtual ~VirtualUart() = default;
};

struct VirtualDeviceHandlers {
	std::shared_ptr<VirtualUart> uart;
};

} // namespace difftest

