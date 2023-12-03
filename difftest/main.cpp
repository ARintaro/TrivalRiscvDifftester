#include "difftest.h"
#include "dut.h"
#include "elf_reader.h"
#include "test_core.h"
#include "virtual_device.h"
#include <cstdio>
#include <iostream>
#include <optional>
#include <ostream>

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

  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <spike_file> <tested_file> <Print> <data_file>" << std::endl;
    return 1;
  }

  auto print = std::string(argv[3]) == "true";

  std::cout << "Print: " << print << std::endl;

  auto ext = get_path_extension(argv[4]);
  Dut golden(argv[1], config);
  Dut tested(argv[2], config);
  if (ext == "elf") {
    Elf elf(argv[4]);
    golden.write_memory(elf);
    tested.write_memory(elf);
  } else if (ext == "bin") {
    BinFile bin(argv[4]);
    golden.write_memory(config.ram_base, bin);
    tested.write_memory(config.ram_base, bin);
  } else {
    std::cerr << "Unknown file extension: " << ext << std::endl;
    return 1;
  }

  auto isa = isa_parser_t("RV32IMAFDC", "MSU");

  auto disasm = std::make_unique<disassembler_t>(&isa);

  unsigned instr_step = 0;

  const unsigned max_step = 100000;

  const u32 ebreak = 0x100073;

  bool failed = false;
  for (int i = 0; i < max_step; i++) {

    unsigned stepped_num = tested.core->step(1);

    if (failed) {
      return 1;
    }

    if (print) {
      std::cerr<< "step_num : " << stepped_num<<std::endl;
    }
    for (int j = 0; j < stepped_num; j++) {
      golden.core->step(1);

      auto last_inst = golden.core->get_state().last_inst;

      if (print) {
        std::cerr << disasm->disassemble(last_inst) << std::endl;
      }

      if (last_inst == ebreak) {
        return 0;
      }

    }
    instr_step += stepped_num;

    const auto &state = golden.core->get_state();
    const auto &state_tested = tested.core->get_state();

    if (state.int_regs != state_tested.int_regs ||
        state.last_pc != state_tested.last_pc) {
      std::cerr << "not matched at" << instr_step << std::endl;
      state.print();
      state_tested.print();
      failed = true;
    } else {
      if (print) {
        state_tested.print();

        auto start = 0x80001000;

        for (int i = 0; i < 10; i++) {
          auto addr = start + i * 4;
          auto val = tested.core->read_memory_word(addr);
          std::cerr << "addr: " << std::hex << addr << " val: " << val
                    << std::endl;
        }

      }
    }
  }
  std::cout << "Difftest Over " << std::endl;
  std::cout << "total steps: " << instr_step << std::endl;
  return 0;
}
