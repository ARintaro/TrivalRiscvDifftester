#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include "difftest.h"

class BinFile {
public:
	std::vector<u8> buffer; 

	explicit BinFile(const std::string &path);
	
	BinFile(const BinFile &) = delete;
	BinFile(BinFile &&) = delete;
	BinFile &operator=(const BinFile &) = delete;
	BinFile &operator=(BinFile &&) = delete;
};
