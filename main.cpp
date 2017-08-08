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

struct kvpData {

	kvpData(uint8_t *p, uint8_t *c)
	{

		memcpy(plainText, p, 16);
		memcpy(cypherText, c, 16);


		plainText[16] = 0;
		cypherText[16] = 0;
	}

	bool find(uint8_t *cText)
	{
		bool found = true;
		for(uint32_t i = 0; i < 16; i++)
		{
			if(cypherText[i] != cText[i])
			{
				found = false;
				break;
			}
		}

		return found;
	}

	uint8_t plainText[17];
	uint8_t cypherText[17];
};

int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	// Get the block size of the encrypted data
	uint32_t unkBlockCount;
	uint8_t ignoreme[1] = {0};
	uint8_t *blah;
	breakME(ignoreme, 0, blah, unkBlockCount);

	unkBlockCount /= 16;

	// Initialize our dictionary variables
	kvpData *dictionary[256] = {0};
	uint8_t data[17] = "AAAAAAAAAAAAAAA";
	data[16] = 0;

	// Loop through each block of encrypted data
	for(uint32_t cBlock = 0; cBlock < unkBlockCount; cBlock++)
	{
		uint8_t testData[17] = "AAAAAAAAAAAAAAA";

		// Loop through each byte of the current block
		for(uint32_t x = 0; x < 16; x++)
		{
			// Build up our dictionary
			for(uint32_t i = 0; i < 256; i++)
			{
				uint8_t byte = i;
				data[15] = i;
		
				uint8_t *result = 0;
				uint32_t resSize = 0;
		
				breakME(data, 16, result, resSize);
		
				dictionary[i] = new kvpData(data, result);
			}
		
			// Send our testData block to be encrypted by the algorithm
			uint8_t *unknownData = 0;
			uint32_t unkDataSize = 0;

			testData[15 - x] = 0;
			//printf("testData: %s\n", testData);
			breakME(testData, (15 - x), unknownData, unkDataSize);
			
			// Compary unknownData+(cBlock * 16) to our dictionary to find our next
			// unencrypted byte of data
			#pragma omp parallel for
			for(uint32_t i = 0; i < 256; i++)
			{
				if(dictionary[i]->find(unknownData+(cBlock*16)) == true)
				{
				//	printf("Found data in dictionary: %s\n", dictionary[i]->plainText);

					data[15] = dictionary[i]->plainText[15];
					printf("%c", data[15]); // Print out the unencrypted byte that we just found
					for(uint32_t j = 0; j < 15; j++)
					{
						data[j] = data[j + 1];
					}
				}

				delete dictionary[i];
			}

			delete[] unknownData;
		}

	}
	printf("\n\n");
	return 0;
}