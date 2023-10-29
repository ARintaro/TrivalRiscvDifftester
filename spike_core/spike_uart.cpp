#include "spike_uart.h"
#include "type_def.h"
#include <iostream>

void SpikeUart::tick(reg_t UNUSED rtc_ticks) {
	
}

bool SpikeUart::store(reg_t offset, size_t len, const uint8_t* bytes) {
	if (offset > config.size) return false;

	if (offset == 0) {
		send_buffer.push(bytes[0]);
	}

	return true;
}

bool SpikeUart::load(reg_t offset, size_t len, uint8_t* bytes) {
	if (offset > config.size) return false;

	u8 ret = 0;
	if (offset == 0 && !recv_buffer.empty()) {
		ret = recv_buffer.front();
		recv_buffer.pop();
	} else if (offset == 5) {
		ret = 0x20U | (!recv_buffer.empty());
	}
	bytes[0] = ret;

	return true;
}

std::optional<u8> SpikeUart::host_read_byte() {
	if (send_buffer.empty()) return std::nullopt;
	u8 ret = send_buffer.front();
	send_buffer.pop();
	return ret;
}

void SpikeUart::host_write_byte(u8 byte) {	
	recv_buffer.push(byte);
}

SpikeUart::SpikeUart(const VirtualDeviceConfig& config) {
	this->config = config;
}
