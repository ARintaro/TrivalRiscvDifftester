#include "difftest.h"
#include "dut.h"
#include "elf_reader.h"
#include "test_core.h"
#include "virtual_device.h"
#include <cstdio>
#include <iostream>
#include <optional>

#include "disasm.h"

std::string get_path_extension(const std::string &path) {
  auto pos = path.find_last_of('.');
  if (pos != std::string::npos || pos == path.length() - 1) {
    return path.substr(pos + 1);
  }
  return "";
}

int main(int argc, char **argv) {
  TestProgramConfig config{
      .debug = false,
      .start_pc = 0x80000000,
      .ram_base = 0x80000000,
      .memory_size = 1024 * 1024 * 8,
      .uart_config = VirtualDeviceConfig{.start = 0x10000000, .size = 8}};

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <so_file> <elf_file>" << std::endl;
    return 1;
  }

  auto ext = get_path_extension(argv[2]);

  Dut golden(argv[1], config);

  if (ext == "elf") {
    Elf elf(argv[2]);
    golden.write_memory(elf);
  } else if (ext == "bin") {
    BinFile bin(argv[2]);
    golden.write_memory(config.ram_base, bin);
  } else {
    std::cerr << "Unknown file extension: " << ext << std::endl;
    return 1;
  }

  auto isa = isa_parser_t("RV32IMAFDC", "MSU");

  auto disasm = std::make_unique<disassembler_t>(&isa);

  for (int i = 0; i < 20; i++) {
    golden.core->step(1);

    auto opt = golden.devices.uart->host_read_byte();

    if (opt.has_value()) {
      std::cout << static_cast<char>(opt.value());
    }
    std::cerr << disasm->disassemble(golden.core->get_state().last_inst) << std::endl;

    const auto& state = golden.core->get_state();
    state.print();
  }

  return 0;
}
