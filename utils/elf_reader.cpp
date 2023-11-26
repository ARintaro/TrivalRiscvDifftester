#include "elf_reader.h"
#include "elfio/elfio_segment.hpp"
#include <iostream>


using namespace ELFIO;

namespace difftest {

void Elf::visit_alloc_sections(const std::function<void(address, msize, const u8*)>& vistor) const {
	Elf_Half seg_num = reader.segments.size();
    for (int i = 0; i < seg_num; ++i) {
        segment *pseg = reader.segments[i];
        Elf_Xword flags = pseg->get_type();    
        msize size = pseg->get_file_size();
        if((flags & PT_LOAD) != 0 && size > 0){
            const auto* data = reinterpret_cast<const u8*>(pseg->get_data());
            u64 addr = pseg->get_virtual_address();
            vistor(addr, size, data);
        }
    }
}


Elf::Elf(const std::string &path) {
    if (!reader.load(path)) {
        std::cerr << "elf load failed\n";
        std::exit(1);
    }
}

} // namespace difftest