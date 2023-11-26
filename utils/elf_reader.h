#pragma once

#include "type_def.h"
#include <functional>
#include <memory>
#include "elfio/elfio.hpp"

namespace difftest {

class Elf {
public:
	explicit Elf(const std::string &path);
	void visit_alloc_sections(
			const std::function<void(difftest::address, difftest::msize,
                               		const difftest::u8 *)> &vistor) const;


private:
	ELFIO::elfio reader;

};


} // namespace difftest
