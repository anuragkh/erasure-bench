#include <Encoder.hpp>
#include <Commons.hpp>

Encoder::Encoder(std::string input, uint32_t data_blocks, uint32_t parity_blocks, uint32_t block_size) {
    this->input = input;

	FILE *file = fopen(input.c_str(), "r");

    // Ensure file can be read
    assert(file != NULL);
    
    fseek(file, 0, SEEK_END);
    this->len = ftell(file);
    fseek(file, 0, SEEK_SET);
	
    this->data = (uint8_t *) memory_map(input);

	this->data_blocks = data_blocks;
	this->parity_blocks = parity_blocks;
	this->block_size = block_size;

	// Ensure the library is linked
	assert(!cauchy_256_init());
}

void Encoder::encode() {
	uint8_t **d_blocks = new uint8_t*[data_blocks];
	for (uint32_t i = 0; i < data_blocks; ++i) {
		d_blocks[i] = data + i * block_size;
        fprintf(stderr, "Assigned %p to index %u...\n", d_blocks[i], i);
	}

	uint8_t *p_blocks = new uint8_t[parity_blocks * block_size];
	
	// Encode the data using library
    assert(!cauchy_256_encode(data_blocks, parity_blocks, (const uint8_t **) d_blocks, p_blocks, block_size));

    FILE *file = fopen((input + ".parity").c_str(), "w");
    size_t bytes_written = fwrite(p_blocks, sizeof(uint8_t), parity_blocks * block_size, file);

    // Ensure all the data is written
    assert(bytes_written == parity_blocks * block_size);

    // Clean up allocated memory
    delete[] d_blocks;
    delete[] p_blocks;
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
    uint32_t parity_blocks = 2;					// 2 parity blocks
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
    
    Encoder en(input, data_blocks, parity_blocks, block_size);

    fprintf(stderr, "Initialization successful!\n");

    timestamp_t start = get_timestamp();
    en.encode();
    timestamp_t tottime = get_timestamp() - start;

    fprintf(stderr, "Time to encode = %lf s\n", (double)tottime / (1000.0 * 1000.0));

    return 0;
}
