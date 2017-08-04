#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>


#include "utility.h"
#include "b64.h"
#include "hex.h"
#include "xor.h"
#include "dynbuff.h"
#include "fileio.h"

int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	FileIO file("6.txt", "rb");
	uint8_t *b64Buffer = new uint8_t[file.getSize()+1];
	memcpy(b64Buffer, file.getContents(), file.getSize());
	b64Buffer[file.getSize()] = 0;


	uint32_t rawSize = Base64::decodedSize((char*) b64Buffer);
	uint8_t *rawBytes = new uint8_t[rawSize];

	Base64::decode((char*) b64Buffer, rawBytes);

	breakVigenere(rawBytes, rawSize);

	delete[] rawBytes;

	printf("\n\n");
	return 0;
}