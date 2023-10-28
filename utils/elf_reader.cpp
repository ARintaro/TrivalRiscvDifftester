#include "elf_reader.h"
#include <iostream>


using namespace ELFIO;

namespace difftest {

void Elf::visit_alloc_sections(const std::function<void(address, msize, const u8*)>& vistor) const {
	Elf_Half sec_num = reader.sections.size();
    for (int i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        Elf_Xword flags = psec->get_flags();        
        if((flags & SHF_ALLOC) != 0){
            msize size = psec->get_size();
            const auto* data = reinterpret_cast<const u8*>(reader.sections[i]->get_data());
            u64 addr = psec->get_address();
            vistor(addr, size, data);
        }
    }
}


Elf::Elf(const char* path) {
    if (!reader.load(path)) {
        std::cerr << "elf load failed\n";
        std::exit(1);
    }
}

} // namespace difftest