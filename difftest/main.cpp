#include <cstdio>
#include <iostream>
#include <optional>
#include "difftest.h"
#include "test_core.h"
#include "elf_reader.h"
#include "dut.h"
#include "virtual_device.h"

#include "disasm.h"

int main(int argc, char **argv) {
  TestProgramConfig config {
    .debug = false,
    .start_pc = 0x80000000,
    .ram_base = 0x80000000,
    .memory_size = 1024 * 1024 * 8,
    .uart_config = VirtualDeviceConfig {
      .start = 0x10000000,
      .size = 8
    }
  };

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <so_file> <elf_file>" << std::endl;
    return 1;
  }

  Elf elf(argv[2]);



  {
    Dut dut(argv[1], config, elf);

    auto isa = isa_parser_t("RV32IMAFDC", "MSU");

    auto disasm = std::make_unique<disassembler_t>(&isa);
  
    for (int i = 0; i < 100000; i++) {
      dut.core->step(1);

      auto opt = dut.devices.uart->host_read_byte();

      if (opt.has_value()) {
        std::cout << static_cast<char>(opt.value());
      }
      disasm->disassemble(dut.core->get_state().last_inst);

    }
  }
  

  return 0;
}
