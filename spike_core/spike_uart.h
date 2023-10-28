
#pragma once

#include "abstract_device.h"
#include "virtual_device.h"
#include <queue>

using namespace difftest;

class SpikeUart : public VirtualUart, public abstract_device_t {
public:
	// VirtualUart
	explicit SpikeUart(const VirtualDeviceConfig& config);
	std::optional<u8> read_byte() override;
	void write_byte(u8 byte) override;

	// abstract_device_t 
	bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  	bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  	void tick(reg_t UNUSED rtc_ticks) override;

private:
	VirtualDeviceConfig config;
	std::queue<u8> buffer;
};