#include <cstdio>
#include <iostream>
#include <optional>
#include "difftest.h"
#include "test_core.h"
#include "elf_reader.h"
#include "dut.h"

int main(int argc, char **argv) {
  TestProgramConfig config {
    .start_pc = 0x80000000,
    .ram_base = 0x80000000,
    .memory_size = 4096,
    .uart_config = std::nullopt
  };

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <so_file> <elf_file>" << std::endl;
    return 1;
  }

  Elf elf(argv[2]);

  Dut dut(argv[1], config, elf);

  dut.core->step(10000);

  dut.core->get_state().print();
  


  return 0;
}
