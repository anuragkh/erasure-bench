#ifndef ENCODER_H
#define ENCODER_H

#include <cauchy_256.h>
#include <Commons.hpp>

class Encoder {
public:
	Encoder(std::string input, uint32_t data_blocks, uint32_t parity_blocks, uint32_t block_size);

	void encode();

private:
	uint32_t data_blocks;
	uint32_t parity_blocks;
	uint32_t block_size;
	
	std::string input;
	uint8_t *data;
	size_t len;
};

#endif