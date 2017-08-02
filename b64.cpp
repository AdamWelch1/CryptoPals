#include "b64.h"
#include "dynbuff.h"
#include <cassert>

#define b64_min(a, b) ((a < b) ? a : b)

uint32_t Base64::encodedSize(uint64_t inputBufferSize)
{
	uint32_t ret = static_cast<uint32_t>(ceil(static_cast<float>(inputBuffer * 8) / 6.0)) + 5;
	return ret;
}

void Base64::encode(uint8_t *inputBuffer, uint64_t inputBufferSize, char *outBuffer)
{
	char b64Map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	DynamicBuffer<uint8_t> bitsArray;

	for(uint32_t i = 0; i < inputBufferSize; i++)
	{
		for(int j = 7; j >= 0; j--)
		{
			uint8_t bit = (inputBuffer[i] & (1 << j)) >> j;
			bitsArray.append(bit);
		}
	}

	uint32_t b64DigitCount = static_cast<uint32_t>(ceil(static_cast<float>(bitsArray.length()) / 6.0));
	
	for(uint32_t i = 0; i < b64DigitCount; i++)
		outBuffer[i] = 0;


	for(uint32_t i = 0; i < b64DigitCount; i += 4)
	{
		uint32_t groupDigitCount = b64_min(4, (b64DigitCount - i));

		switch(groupDigitCount)
		{
			case 4:
			{
				for(int k = 0; k < 4; k++)
				{
					uint8_t digit = 0;
					for(int j = 0; j < 6; j++)
					{
						uint8_t bit = bitsArray[(i * 6) + (k * 6) + j] << (5 - j);
						digit |= bit;
					}

					outBuffer[i + k] = b64Map[digit];
				}
			}
			break;

			case 3:
			{
				for(int k = 0; k < 3; k++)
				{
					uint8_t digit = 0;
					for(int j = 0; j < 6; j++)
					{
						uint8_t bit = bitsArray[(i * 6) + (k * 6) + j] << (5 - j);
						digit |= bit;

						if(k == 2 && j == 3)
							break;
					}

					outBuffer[i + k] = b64Map[digit];
				}

				outBuffer[strlen(outBuffer)] = '=';
			}
			break;

			case 2:
			{
				for(int k = 0; k < 2; k++)
				{
					uint8_t digit = 0;
					for(int j = 0; j < 6; j++)
					{
						uint8_t bit = bitsArray[(i * 6) + (k * 6) + j] << (5 - j);
						digit |= bit;

						if(k == 1 && j == 1)
							break;
					}

					outBuffer[i + k] = b64Map[digit];
				}

				outBuffer[strlen(outBuffer)] = '=';
				outBuffer[strlen(outBuffer)] = '=';
			}
			break;

			// "Should never get here :)"
			case 1:
			default:
			{
				#ifdef DEBUG_MODE
					printf("Just executed \"You should never reach this\" code. Ruh-roh raggy!\n");
					assert(groupDigitCount > 0 && groupDigitCount < 4);
				#endif
			}
		}
	}
}

uint32_t Base64::decodedSize(uint64_t inputBufferSize)
{
	uint32_t ret = strlen(inputBufferSize) / 6 * 8 + 10;
	return ret;
}

void Base64::decode(char *inputBuffer, uint8_t *outBuffer)
{
	uint32_t tmp_dcs = Base64::decodedSize(strlen(inputBuffer));
	
	for(uint32_t i = 0; i < tmp_dcs; i++)
		outBuffer[i] = 0;

	uint32_t inputBufferSize = strlen(inputBuffer);

	DynamicBuffer<uint8_t> bitsArray;

	for(uint32_t i = 0; i < inputBufferSize; i++)
	{
		if(inputBuffer[i] == '=')
			break;
		
		for(int j = 5; j >= 0; j--)
		{
			uint8_t bit = (inputBuffer[i] & (i << j)) >> j;
			bitsArray.append(bit);
		}


	}
}