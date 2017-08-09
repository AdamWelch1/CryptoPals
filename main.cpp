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

bool paddingCheck(uint8_t *aesKey, uint8_t *encData, uint8_t encSize)
{
	uint8_t *decData = new uint8_t[encSize];
	AES::AES128_cbc_decrypt((char*) aesKey, encData, decData, encSize);

	bool retVal = AES::checkPadding(decData, encSize);

	delete[] decData;

	return retVal;
}

int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	// Just generate a random key on each invocation
	uint8_t aesKey[16] = {0};
	randomBytes(aesKey, 16);

	// Here we're encrypting our plaintext into a buffer
	// to be used during the "padding oracle" attack

	// I added all the A's to act like an IV to the CBC encryption function
	char plainText[] = "AAAAAAAAAAAAAAAAHow much wood would a wood chuck chuck if a wood chuck could chuck wood?";
	uint8_t encData[100] = {0};


	// Encrypt the data, and set up on working buffers...
	uint32_t encSize = AES::AES128_cbc_encrypt((char*) aesKey, (uint8_t*) plainText, encData, strlen(plainText));
	uint8_t dataCopy[100] = {0};
	uint8_t decodedBytes[100] = {0};

	/*
	 * Alright, here we are. The padding oracle attack works upon the idea
	 * that flipping a bit in Block #1 will duplicate the bit error in Block #2.
	 * By this logic, we can iterate through bytes 0-255, XORing byte X in Block #1
	 * until we produce a valid padding byte at byte X in Block #2. Since we know
	 * what the proper padding byte value is, we can then deduce the plaintext value
	 * of the corresponding byte based on the output of a CBC padding check function.
	 *
	 */

	for(uint32_t cBlock = 0; cBlock < (encSize / 16 - 1); cBlock++)
	{
		for(uint32_t j = 0; j < 16; j++)
		{
			memcpy(dataCopy, encData, encSize);

			for(uint32_t i = 0; i < j; i++)
			{
				uint8_t xByte = j + 1;
				dataCopy[encSize - (cBlock * 16) - (i+1) - 16] ^= (decodedBytes[encSize - (cBlock * 16) - (i+1) - 16] ^ xByte);
			}

			for(uint32_t i = ((j == 0) ? 1 : 0); i < 256; i++)
			{
				uint32_t index = encSize - (cBlock * 16) - (j+1) - 16;

				dataCopy[index] = encData[index];

				uint8_t xorByte = i;
				dataCopy[index] ^= xorByte;

				bool pcVal = paddingCheck(aesKey, dataCopy, (encSize - (cBlock * 16)));

				if(pcVal)
				{
					decodedBytes[index] = xorByte ^ (j + 1);
					break;
				}
			}
		}
	}

	AES::stripPadding(decodedBytes, encSize);
	printf("Recovered plaintext:\n%s", decodedBytes);
	printf("\n\n");
	return 0;
}

