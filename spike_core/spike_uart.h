
#pragma once

#include "abstract_device.h"
#include "virtual_device.h"
#include <queue>

using namespace difftest;

class SpikeUart : public VirtualUart, public abstract_device_t {
public:
	explicit SpikeUart(const VirtualDeviceConfig& config);

	// VirtualUart
	std::optional<u8> host_read_byte() override;
	void host_write_byte(u8 byte) override;

	// abstract_device_t 
	bool load(reg_t offset, size_t len, uint8_t* bytes) override;
  	bool store(reg_t offset, size_t len, const uint8_t* bytes) override;
  	void tick(reg_t UNUSED rtc_ticks) override;

private:
	VirtualDeviceConfig config;
	std::queue<u8> recv_buffer;
	std::queue<u8> send_buffer;
};