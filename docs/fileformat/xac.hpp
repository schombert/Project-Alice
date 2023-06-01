#include <iostream>
#include <cstdint>

typedef struct {
	//char magic[3];
	int32_t version;
	int32_t unknown_data;
	char junk[29];
} xac_header;
