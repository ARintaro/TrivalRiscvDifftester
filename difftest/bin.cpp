#include "bin.h"
#include <fstream>
#include <ios>
#include <stdexcept>

BinFile::BinFile(const std::string &path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file || !file.is_open()) {
		throw std::runtime_error("Cannot open bin file: " + path);
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	buffer.resize(size);

	if (!file.read(reinterpret_cast<char *>(buffer.data()), size)) {
		throw std::runtime_error("Cannot read bin file: " + path);
	}
}
