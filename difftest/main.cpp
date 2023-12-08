#include "difftest.h"
#include "dut.h"
#include "elf_reader.h"
#include "test_core.h"
#include "virtual_device.h"
#include <asm-generic/socket.h>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <ostream>
#include <queue>

#include "disasm.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

const unsigned max_step = 100000;

const u32 ebreak = 0x100073;

bool received_term = false;
int ret_value = 0;

std::string get_path_extension(const std::string &path) {
  auto pos = path.find_last_of('.');
  if (pos != std::string::npos || pos == path.length() - 1) {
    return path.substr(pos + 1);
  }
  return "";
}

void signal_handler(int signum) {
  puts("Receive Signal, exit...");
  received_term = true;
  ret_value = signum;
}

int main(int argc, char **argv) {
  TestProgramConfig config{
      .debug = false,
      .start_pc = 0x80000000,
      .ram_base = 0x80000000,
      .memory_size = 1024 * 1024 * 8,
      .uart_config = VirtualDeviceConfig{.start = 0x10000000, .size = 8}};


  
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0]
              << " <spike_file> <tested_file> <print> <data_file> <uart_server>" << std::endl;
    return 1;
  }

  auto print = std::string(argv[3]) == "true";
  bool uart_server = std::string(argv[5]) == "true";

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

  int bind_port = 8080;

  std::queue<u8> uart_buffer;
  int listenfd = 0;
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(bind_port);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int clientfd = 0;

  if (uart_server) {
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    // setsockopt(listenfd, SOL_SOCKET , SO_REUSEADDR, &opt, sizeof(opt));
    if (listenfd == -1) {
      std::cout << "socket error" << std::endl;
      return 1;
    }
    if (bind(listenfd, (sockaddr *)&addr, sizeof(addr)) == -1) {
      std::cout << "bind error on port " << bind_port << std::endl;
      return 1;
    }

    if (listen(listenfd, 1) == -1) {
      std::cout << "listen error" << std::endl;
      return 1;
    }

    std::cout << "running on server mode, waiting connect, watching " << bind_port << std::endl;

    clientfd = accept(listenfd, (sockaddr *)&client_addr, &client_addr_len);

    if (received_term) {
      std::cout << "client not connected" << std::endl;
      goto EXIT;
    }

    std::cout << "client connected" << std::endl;
  } else {

    std::cout << "running on no server mode, max step: " << max_step  << std::endl;
  }

  
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  u8 recvBuffer[1024];


  try {
    while (!received_term && (uart_server || instr_step < max_step)) {

      if (uart_server) {
        int recvLen =
            recv(clientfd, recvBuffer, sizeof(recvBuffer), MSG_DONTWAIT);
        if (recvLen > 0) {
          for (int j = 0; j < recvLen; j++) {
            uart_buffer.push(recvBuffer[j] & 0xff);
            printf("[difftest] send 0x%x\n", recvBuffer[j] & 0xff);
          }
        }

        if (uart_buffer.size()) {
          if (tested.devices.uart->host_write_byte(uart_buffer.front())) {
            uart_buffer.pop();
          }
        }
      }

      auto result = tested.core->step();
      // std::cout << "tested step" << instr_step << "finished " << std::endl;

      if (print) {
        std::cerr << "step_num : " << result.step_num << std::endl;
      }

      auto &test_state = tested.core->get_state();

      if (result.uncertern) {
        // 非确定转移，同步状态
        golden.core->set_state(test_state);
        // std::cerr<< "uncertern sync state" << result.step_num << std::endl;
        // test_state.print();
        continue;
      }

      for (int j = 0; j < result.step_num; j++) {
        golden.core->step();
        // std::cout << "golden step" << instr_step + j << "finished " << std::endl;
        auto last_inst = golden.core->get_state().last_inst;

        if (print) {
          std::cerr << disasm->disassemble(last_inst) << std::endl;
        }

        // if (last_inst == ebreak) {
        //   ret_value = 0;
        //   received_term = true;
        // }
      }

      auto uart_out = tested.devices.uart->host_read_byte();
      if (uart_out.has_value()) {
        send(clientfd, &uart_out.value(), 1, 0);
      }

      instr_step += result.step_num;

      const auto &state = golden.core->get_state();
      const auto &state_tested = tested.core->get_state();

      if (state != state_tested && !received_term) {
        std::cerr << "not matched" << std::endl;
        std::cerr << "== golden state ==" << std::endl;
        state.print();
        std::cerr << "== test core state ==" << std::endl;
        state_tested.print();

        ret_value = 1;
        received_term = true;
      }
      else if (print) {
        test_state.print();
      }
    }

  } catch (...) {
    
  }

EXIT:

  puts("==============");
  std::cout << "Difftest Over with " << ret_value << " " << std::endl;
  std::cout << "total steps: " << instr_step << std::endl;

  if (uart_server) {
    close(clientfd);
    close(listenfd);
  }

  return ret_value;
}
