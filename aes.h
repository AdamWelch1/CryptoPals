#ifndef AES_H
#define AES_H

#include <tomcrypt.h>
#include <cassert>
#include "xor.h"
#include <cstring>

class AES {

	public:

		static void AES128_ecb_decrypt(char *key, uint8_t *inBuffer, uint8_t *outBuffer, uint32_t blockCount)
		{
			symmetric_key keyStruct;
			aes_setup((uint8_t*) key, 16, 0, &keyStruct);

			for(uint32_t i = 0; i < blockCount; i++)
				aes_ecb_decrypt(inBuffer+(i * 16), outBuffer+(i * 16), &keyStruct);
		}

		static void AES128_ecb_encrypt(char *key, uint8_t *inBuffer, uint8_t *outBuffer, uint32_t blockCount)
		{
			symmetric_key keyStruct;
			aes_setup((uint8_t*) key, 16, 0, &keyStruct);

			for(uint32_t i = 0; i < blockCount; i++)
				aes_ecb_encrypt(inBuffer+(i * 16), outBuffer+(i * 16), &keyStruct);
		}

		static bool detect_ecb(uint8_t *byteBuffer, uint32_t bbSize)
		{
			#ifdef DEBUG_MODE
				assert(bbSize % 16 == 0);
			#endif

			// byteBuffer length is not divisible by 16, so clearly not ECB encrypted data...
			if(bbSize % 16 != 0)
				return false;

			uint32_t blockCount = bbSize / 16;

			for(uint32_t i = 0; i < (blockCount-1); i++)
			{
				for(uint32_t j = (i+1); j < blockCount; j++)
				{
					if(memcmp(byteBuffer+(i * 16), byteBuffer+(j * 16), 16) == 0)
						return true;
				}
			}

			return false;
		}

		// Encrypts data using AES 128-bit, CBC method
		// Pads data using PKCS#7 to a multiple of 16 bytes.
		// Returns the size of the encrypted data, including padding
		static uint32_t AES128_cbc_encrypt(char *key, uint8_t *inBuffer, uint8_t *outBuffer, uint32_t inBuffSize)
		{
			uint8_t initVector[16] = {0};
			uint32_t blockCount = 0;

			uint8_t *inBuff = new uint8_t[inBuffSize + 16]; // This is for making sure we can pad if necessary
			memcpy(inBuff, inBuffer, inBuffSize);

			if(inBuffSize % 16 == 0)
				blockCount = inBuffSize / 16;
			else
				blockCount = AES::pkcs7_pad(inBuff, inBuffSize) / 16;

			for(uint32_t i = 0; i < blockCount; i++)
			{
				uint8_t block[16] = {0};
				memcpy(block, inBuff+(i * 16), 16);
				
				Xor::xorBuffers(block, initVector, 16);

				AES::AES128_ecb_encrypt(key, block, block, 1);
				memcpy(initVector, block, 16);
				memcpy(outBuffer+(i * 16), block, 16);
			}

			return (blockCount * 16);
		}

		// Decrypts AES 128-bit CBC encrypted data.
		// Removes any PKCS#7 padding from the end of the decrypted data
		// and returns the size of the data with the padding removed
		static uint32_t AES128_cbc_decrypt(char *key, uint8_t *inBuffer, uint8_t *outBuffer, uint32_t inBuffSize)
		{
			uint32_t blockCount = inBuffSize / 16;
			uint8_t lastBlock[16] = {0};

			for(uint32_t i = 0; i < blockCount; i++)
			{
				if(i > 0)
					memcpy(lastBlock, inBuffer+((i - 1) * 16), 16);

				AES128_ecb_decrypt(key, inBuffer+(i * 16), outBuffer+(i * 16), 1);
				Xor::xorBuffers(outBuffer+(i * 16), lastBlock, 16);
			}

			uint8_t lastByte = outBuffer[inBuffSize-1];
			uint32_t retSize = inBuffSize;

			// Remove any padding
			bool padded = true;
			for(uint32_t i = (inBuffSize - lastByte); i < inBuffSize; i++)
			{
				if(outBuffer[i] != lastByte)
				{
					padded = false;
					break;
				}
			}

			if(padded)
			{
				retSize = inBuffSize - lastByte;
				outBuffer[retSize] = 0;
			}

			return retSize;
		}

		// Pads the buffer to a 16-byte block size.
		// The buffer must have enough space for the padding.
		// dataSize is the amount of actual data in the buffer.
		// Returns dataSize + padCount, to give the new buffer size.
		static uint32_t pkcs7_pad(uint8_t *buffer, uint32_t dataSize)
		{
			if(dataSize % 16 == 0)
				return 0; // No padding necessary

			uint32_t padCount = 16 - (dataSize % 16);
			uint8_t padByte = padCount;

			for(uint32_t i = 0; i < padCount; i++)
				buffer[dataSize+i] = padByte;

			return (dataSize + padCount);
		}
};


#endif