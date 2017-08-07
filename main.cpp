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

	FileIO file("10.txt", "rb");
	
	uint8_t *b64Buffer = new uint8_t[file.getSize()+1];
	memcpy(b64Buffer, file.getContents(), file.getSize());
	b64Buffer[file.getSize()] = 0;

	uint32_t rawSize = Base64::decodedSize((char*) b64Buffer);
	uint8_t *rawBytes = new uint8_t[rawSize+1];
	
	Base64::decode((char*) b64Buffer, rawBytes);

	char aesKey[] = "YELLOW SUBMARINE";
	uint8_t *decryptedBytes = new uint8_t[rawSize];

	uint32_t decSize = AES::AES128_cbc_decrypt(aesKey, rawBytes, decryptedBytes, rawSize);

	printf("rawSize: %lu\ndecSize: %lu\n\n", rawSize, decSize);
	//maskNonPrintableChars(decryptedBytes, decSize);
	printf("Decrypted: %s", decryptedBytes);

	delete[] decryptedBytes;
	delete[] b64Buffer;
	delete[] rawBytes;
	printf("\n\n");
	return 0;
}