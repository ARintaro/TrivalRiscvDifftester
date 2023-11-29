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

  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <spike_file> <tested_file> <elf_file>" << std::endl;
    return 1;
  }

  auto ext = get_path_extension(argv[3]);
  Dut golden(argv[1], config);
  Dut tested(argv[2], config);
  if (ext == "elf") {
    Elf elf(argv[3]);
    golden.write_memory(elf);
    tested.write_memory(elf);
  } else if (ext == "bin") {
    BinFile bin(argv[3]);
    golden.write_memory(config.ram_base, bin);
    tested.write_memory(config.ram_base, bin);
  } else {
    std::cerr << "Unknown file extension: " << ext << std::endl;
    return 1;
  }

  auto isa = isa_parser_t("RV32IMAFDC", "MSU");

  auto disasm = std::make_unique<disassembler_t>(&isa);

  unsigned instr_step = 0;

  bool failed = false;
  for (int i = 0; i < 10000; i++) {
    unsigned stepped_num = tested.core->step(1);

    if (failed) {
      return 1;
    }

    // std::cerr<<"step_num:" << stepped_num<<std::endl;
    for(int j = 0;j < stepped_num;j++) {
      golden.core->step(1);
      std::cerr << disasm->disassemble(golden.core->get_state().last_inst) << std::endl;
    }
    instr_step += stepped_num;
    //auto opt = golden.devices.uart->host_read_byte();

    // if (opt.has_value()) {
    //   std::cout << static_cast<char>(opt.value());
    // }
    // 
    
    const auto& state = golden.core->get_state();
    const auto& state_tested = tested.core->get_state();

    if(state.int_regs != state_tested.int_regs || state.last_pc != state_tested.last_pc) {
      std::cerr<<"not matched at"<<instr_step<<std::endl;
      state.print();
      state_tested.print();
      failed = true;
    } else {
      // state_tested.print();
    }
    
    
  }
  std::cout<<"赢"<<std::endl;
  std::cout<<"total steps"<<instr_step <<std::endl;
  return 0;
}
