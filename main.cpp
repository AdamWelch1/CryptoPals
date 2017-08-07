#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <tomcrypt.h>


#include "utility.h"
#include "b64.h"
#include "hex.h"
#include "xor.h"
#include "dynbuff.h"
#include "fileio.h"
#include "aes.h"

int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	FileIO file("7.txt", "rb");
	uint8_t *b64Buffer = new uint8_t[file.getSize()+1];
	memcpy(b64Buffer, file.getContents(), file.getSize());
	b64Buffer[file.getSize()] = 0;

	uint32_t rawSize = Base64::decodedSize((char*) b64Buffer);
	uint8_t *rawBytes = new uint8_t[rawSize+1];

	Base64::decode((char*) b64Buffer, rawBytes);

	char aesKey[] = "YELLOW SUBMARINE";
	uint32_t blockCount = rawSize / 16;

	AES::AES128_ecb_decrypt(aesKey, rawBytes, rawBytes, blockCount);

	rawBytes[rawSize] = 0;
	printf("%s", rawBytes);


	delete[] b64Buffer;
	delete[] rawBytes;
	printf("\n\n");
	return 0;
}