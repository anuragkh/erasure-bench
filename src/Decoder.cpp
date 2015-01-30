#include <Decoder.hpp>

Decoder::Decoder(std::string input, uint32_t data_blocks, uint32_t parity_blocks, uint32_t block_size) {
    this->input = input;

    // Read input data
    FILE *file = fopen(input.c_str(), "r");

    // Ensure file can be read
    assert(file != NULL);
    
    fseek(file, 0, SEEK_END);
    this->len = ftell(file);
    
    fseek(file, 0, SEEK_SET);
    this->data = new uint8_t[this->len];
    
    size_t bytes_read = fread(this->data, sizeof(uint8_t), this->len, file);

    // Ensure all bytes are read
    assert(bytes_read == this->len);

    // Populate data blocks array
    this->d_blocks = new uint8_t*[data_blocks];
    for (uint32_t i = 0; i < data_blocks; ++i) {
        this->d_blocks[i] = this->data + i * block_size;
    }

    // Read parity data
    FILE *pfile = fopen((input + ".parity").c_str(), "r");

    // Ensure file can be read
    assert(pfile != NULL);
    
    fseek(pfile, 0, SEEK_END);
    size_t plen = ftell(pfile);

    // Assert parity data size conforms to provided configuration
    assert(plen == parity_blocks * block_size);
    
    fseek(pfile, 0, SEEK_SET);
    this->p_blocks = new uint8_t[plen];
    
    size_t pbytes_read = fread(p_blocks, sizeof(uint8_t), plen, pfile);

    // Ensure all bytes are read
    assert(pbytes_read == plen);

    // Assume data needs to be reconstructed from all parity blocks, and minimum number of data blocks
    blocks = new Block[data_blocks];
    for(uint32_t i = 0; i < parity_blocks; i++) {
        uint32_t erasure_index = parity_blocks - i - 1;
        blocks[i].data = p_blocks + erasure_index * block_size;
        blocks[i].row = data_blocks + erasure_index;
    }

    for(uint32_t i = parity_blocks; i < data_blocks; i++) {
        blocks[i].data = data + i * block_size;
        blocks[i].row = i;
    }

	this->data_blocks = data_blocks;
	this->parity_blocks = parity_blocks;
	this->block_size = block_size;

	// Ensure the library is linked
	assert(!cauchy_256_init());
}

void Decoder::decode() {
    assert(!cauchy_256_decode(data_blocks, parity_blocks, blocks, block_size));
}

void print_usage(char *exec) {
    fprintf(stderr, "Usage: %s [-d data_blocks] [-p parity_blocks] [-b block_size] input\n", exec);
}

int main(int argc, char **argv) {

	if(argc < 2 || argc > 8) {
		print_usage(argv[0]);
		return -1;
	}

	int c;
    uint32_t data_blocks = 10;					// 10 data blocks
    uint32_t parity_blocks = 4;					// 4 parity blocks
    uint32_t block_size = 128 * 1024 * 1024;	// 128MB blocks
    while((c = getopt(argc, argv, "d:p:b:")) != -1) {
        switch(c) {
        case 'd':
            data_blocks = atoi(optarg);
            break;
        case 'p':
            parity_blocks = atoi(optarg);
            break;
        case 'b':
            block_size = atoi(optarg);
            break;
        default:
            data_blocks = 10;
            parity_blocks = 4;
            block_size = 128 * 1024 * 1024;
        }
    }

    if(optind == argc) {
        print_usage(argv[0]);
        return -1;
    }

    std::string input = std::string(argv[optind]);

    fprintf(stderr, "Working with parameters: data_blocks=%u, parity_blocks=%u, block_size=%u, input=%s\n", 
        data_blocks, parity_blocks, block_size, input.c_str());

    Decoder de(input, data_blocks, parity_blocks, block_size);

    fprintf(stderr, "Initialization successful!\n");

    timestamp_t start = get_timestamp();
    de.decode();
    timestamp_t tottime = get_timestamp() - start;

    fprintf(stderr, "Time to decode = %lf s\n", (double)tottime / (1000.0 * 1000.0));

    return 0;
}