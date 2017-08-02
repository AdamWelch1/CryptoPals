#include "b64.h"
#include "dynbuff.h"
#include <cassert>

#define b64_min(a, b) ((a < b) ? a : b)

uint32_t Base64::encodedSize(uint64_t inputBufferSize)
{
	uint32_t ret = static_cast<uint32_t>(ceil(static_cast<float>(inputBufferSize * 8) / 6.0)) + 5;
	return ret;
}

/**
  * I am aware that this function is less than optimal. My intention here is
  * for readability over efficiency. Given that computers are incredibly fast,
  * this should have minimal impact unless this function is used to encode
  * very large amounts of data.
 **/
void Base64::encode(uint8_t *inputBuffer, uint64_t inputBufferSize, char *outBuffer)
{
	char b64Map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	DynamicBuffer<uint8_t> bitsArray;

	// Extract bits from input bytes and add to bitsArray
	for(uint32_t i = 0; i < inputBufferSize; i++)
	{
		for(int j = 7; j >= 0; j--)
		{
			uint8_t bit = (inputBuffer[i] & (1 << j)) >> j;
			bitsArray.append(bit);
		}
	}

	// Calculate the number of Base64 digits - effectively, the output buffer size
	uint32_t b64DigitCount = static_cast<uint32_t>(ceil(static_cast<float>(bitsArray.length()) / 6.0));
	
	// Zero the output buffer, ensures that the buffer ends with a null byte
	for(uint32_t i = 0; i < b64DigitCount + 1; i++)
		outBuffer[i] = 0;


	// Iterate over the bitsArray and map the data to Base64 digits
	for(uint32_t i = 0; i < b64DigitCount; i += 4)
	{
		// Calculate the number of base64 digits left to calculate
		uint32_t groupDigitCount = b64_min(4, (b64DigitCount - i));

		switch(groupDigitCount)
		{
			case 4: // We have a full 24 bits, generate 4 Base64 digits
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

			case 3: // We have only 16 bits left, generate 3 Base64 digits and 1 '='
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

			case 2: // We have only 8 bits left, generate 2 Base64 digits and 2 '='
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
					assert(groupDigitCount > 1 && groupDigitCount < 4);
				#endif
			}
		}
	}
}

uint32_t Base64::decodedSize(char *inputBuffer)
{
	uint32_t ret = strlen(inputBuffer);

	if(inputBuffer[ret-1] == '=')
		ret--;
	
	if(inputBuffer[ret-1] == '=')
		ret--;

	ret = ret * 6 / 8;

	return ret;
}

/*** See comment just before the encode() function above ***/
void Base64::decode(char *inputBuffer, uint8_t *outBuffer)
{
	char b64Map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	// Zero out our outBuffer
	uint32_t tmp_dcs = Base64::decodedSize(inputBuffer);
	
	for(uint32_t i = 0; i < tmp_dcs; i++)
		outBuffer[i] = 0;


	uint32_t inputBufferSize = strlen(inputBuffer);
	DynamicBuffer<uint8_t> bitsArray;

	// Convert the 6-bit Base64 into bits
	for(uint32_t i = 0; i < inputBufferSize; i++)
	{
		if(inputBuffer[i] == '=')
			break;

		for(int j = 5; j >= 0; j--)
		{
			uint8_t b64Char = (uint8_t) (strchr(b64Map, inputBuffer[i]) - b64Map);

			uint8_t bit = (b64Char & (1 << j)) >> j;
			bitsArray.append(bit);
		}
	}

	uint32_t outBufferIndex = 0;

	// Convert our bits into 8-bit bytes
	for(uint32_t i = 0; i < bitsArray.length(); i += 8)
	{
		uint8_t byte = 0;

		for(int32_t j = 7; j >= 0; j--)
		{
			if(i + (7 - j) >= bitsArray.length())
				break;

			uint8_t bit = bitsArray[(i) + (7 - j)];
			byte |= (bit << j);
		}

		outBuffer[outBufferIndex] = byte;
		outBufferIndex++;
	}
}