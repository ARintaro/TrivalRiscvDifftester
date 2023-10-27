#include "spike_core.h"
#include "devices.h"

#include <cstdio>

int main() {
	
	abstract_mem_t* mem_p = new mem_t(4096 * 341);

	std::cout << mem_p->size() << std::endl;

	return 0;
}