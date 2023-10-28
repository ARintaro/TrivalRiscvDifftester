#include "spike_uart.h"

void SpikeUart::tick(reg_t UNUSED rtc_ticks) {
	
}

bool SpikeUart::store(reg_t addr, size_t len, const uint8_t* bytes) {
	
}

bool SpikeUart::load(reg_t addr, size_t len, uint8_t* bytes) {
	if (!config.check_in(addr)) return false;

	u8 offset = addr - config.start;

	if (offset == 0) {
		
	}

}
