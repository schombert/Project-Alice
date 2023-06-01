#include <cstdio>

#include "xac.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 2) {
		return -1;
	}
	xac_header header;
	FILE* fp;
	fp = fopen(argv[1], "rb");

	fgetc(fp); fgetc(fp); fgetc(fp);
	fread(&header, sizeof(header), 1, fp);

	//printf("Magic: %s\n",header.magic);
	printf("Version: %d\n",header.version);
	printf("Unknown: %d\n",header.unknown_data);

	fclose(fp);
	return 0;
}
