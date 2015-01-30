#ifndef DECODER_H
#define DECODER_H

#include <cauchy_256.h>
#include <Commons.hpp>

class Decoder {
public:
	Decoder(std::string input, uint32_t data_blocks, uint32_t parity_blocks, uint32_t block_size);

	void decode();

private:
	uint32_t data_blocks;
	uint32_t parity_blocks;
	uint32_t block_size;

	std::string input;
	uint8_t *data;
	size_t len;

	uint8_t **d_blocks;
	uint8_t *p_blocks;

	Block *blocks;
};

#endif