#ifndef UTILITY_H
#define UTILITY_H

#include <cstdio>
#include <cstring>
#include "dynbuff.h"
#include "xor.h"
#include "aes.h"
#include "b64.h"

// Returns a single random Byte. NOT CRYPTOGRAPHICALLY SECURE
uint8_t randomByte()
{
	FILE *urandom = fopen("/dev/urandom", "rb");
	uint8_t ret = 0;
	fread(&ret, 1, 1, urandom);
	fclose(urandom);

	return ret;
}

// Fills outBuffer with outBufferSize bytes. Again, this is NOT CRYPTOGRAPHICALLY SECURE
void randomBytes(uint8_t *outBuffer, uint32_t outBufferSize)
{
	FILE *urandom = fopen("/dev/urandom", "rb");
	fread(outBuffer, outBufferSize, 1, urandom);
	fclose(urandom);
}

// Checks for the existence of a string within a larger string
bool strinstr(char *needle, char *haystack)
{
	bool retVal = true;

	for(uint32_t i = 0; i < strlen(haystack); i++)
	{
		retVal = true;
		for(uint32_t j = 0; j < strlen(needle); j++)
		{
			if((i+j-1) >= strlen(haystack))
			{
				retVal = false;
				break;
			}

			if(needle[j] != haystack[i+j])
			{
				retVal = false;
				break;
			}
		}

		if(retVal == true)
			break;
	}

	return retVal;
}

bool byteInBuffer(uint8_t *buffer, uint32_t bufferLength, uint8_t byte)
{
	for(uint32_t i = 0; i < bufferLength; i++)
	{
		if(buffer[i] == byte)
			return true;
	}

	return false;
}

void maskNonPrintableChars(uint8_t *buffer, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		if(buffer[i] < 32 || buffer[i] > 127)
			buffer[i] = '?';
	}
}

void maskNonPrintableChars(char *buffer, uint32_t length)
{
	maskNonPrintableChars((uint8_t*) buffer, length);
}

uint32_t scoreEnglish(uint8_t *buffer)
{
	char *commonEnglish[20] = {"the", "be", "to", "of", "and", "a", "in", "that", "have", "i",
								"it", "for", "not", "on", "with", "he", "as", "you", "do", "at"};

	uint32_t score = 0;
	for(uint32_t i = 0; i < 20; i++)
		score += (strinstr(commonEnglish[i], (char*) buffer) == true) ? 1 : 0;
	
	return score;
}

uint32_t editDistance(uint8_t *buffer1, uint8_t *buffer2, uint32_t length)
{
	uint8_t *tmpBuffer = new uint8_t[length];

	for(uint32_t i = 0; i < length; i++)
		tmpBuffer[i] = buffer1[i] ^ buffer2[i];

	uint32_t retVal = 0;
	for(uint32_t i = 0; i < length; i++)
	{
		for(uint8_t j = 0; j < 8; j++)
		{
			if((tmpBuffer[i] & (1 << j)) != 0)
				retVal++;
		}
	}

	delete[] tmpBuffer;

	return retVal;
}

uint32_t guessKeySize(uint8_t *buffer, uint32_t rangeLow, uint32_t rangeHigh, bool printStats = false)
{
	float lowestEdit = 999.0;
	uint32_t leKeysize = 999;

	for(uint32_t i = rangeLow; i <= rangeHigh; i++)
	{
		float eDist = static_cast<float>(editDistance(buffer, (buffer+i), i));
		eDist += static_cast<float>(editDistance((buffer+i+i), (buffer+i+i+i), i));
		eDist /= 2.0;
		eDist /= static_cast<float>(i);

		if(eDist < lowestEdit)
		{
			lowestEdit = eDist;
			leKeysize = i;
		}

		if(printStats == true)
			printf("KeySize : %lu; Edit distance : %.3f\n", i, eDist);
	}

	return leKeysize;
}

bool containsNonPrintableChars(uint8_t *buffer, uint32_t bufferLength)
{
	//char printableChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890`~!@#$%^&*()-=_+[]\\{}|:;\"'<>?,./ \t\r\n";
	char printableChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890!()-:;\"'?,. \r\n";
	uint8_t nonPrintableChars[256] = {0};
	uint32_t npcCount = 0;

	// Build the array of non-printable characters
	for(uint32_t i = 0; i < 256; i++)
	{
		uint8_t byte = i;

		if(!byteInBuffer((uint8_t*) printableChars, strlen(printableChars), byte))
		{
			nonPrintableChars[npcCount] = byte;
			npcCount++;
		}
	}

	for(uint32_t i = 0; i < npcCount; i++)
	{
		if(byteInBuffer(buffer, bufferLength, nonPrintableChars[i]))
		{
		//	printf("Found non-printable char: %.2X\n", nonPrintableChars[i]);
			return true;
		}
	}

	return false;
}

void getPossibleKeys(DynamicBuffer<char*> &possibleKeys, DynamicBuffer<uint8_t> *keyCandidates, uint32_t index, uint32_t max, char *printBuf = 0)
{
	if(printBuf == 0)
	{
		printBuf = new char[max+1];
	}

	for(uint32_t i = 0; i < keyCandidates[index].length(); i++)
	{
		if(index == 0)
		{
			for(uint32_t x = 0; x < max; x++)
				printBuf[x] = 0;
		}

		printBuf[index] = keyCandidates[index][i];
		printBuf[index+1] = 0;

		if(index == (max - 1))
		{
			// DO NOT FREE - is passed to possibleKeys, and must be deleted in caller function.
			char *tmp = new char[max+1];
			memcpy(tmp, printBuf, max+1);
			possibleKeys.append(tmp);
		}
		else
			getPossibleKeys(possibleKeys, keyCandidates, (index + 1), max, printBuf);
	}

	if(index == 0)
		delete[] printBuf;
}

void breakVigenere(uint8_t *buffer, uint32_t bufferSize)
{
	#define MAX_KEY_SIZE 55

	uint32_t keySize = guessKeySize(buffer, 3, MAX_KEY_SIZE);
	//printf("Key size: %lu\n", keySize);

	for(uint32_t p = 0; p < 5; p++)
	{
		if(p != 0)
		{
			keySize = guessKeySize(buffer, keySize+1, MAX_KEY_SIZE);

			if(keySize > MAX_KEY_SIZE)
				break;

		//	printf("Key size: %lu\n", keySize);
		}

		uint8_t **chunks = new uint8_t*[keySize];
		uint32_t *chunkCounts = new uint32_t[keySize];
		
		for(uint32_t i = 0; i < keySize; i++)
		{
			chunks[i] = new uint8_t[bufferSize / keySize + 1];
			chunkCounts[i] = 0;
		}

		for(uint32_t i = 0; i < bufferSize; i++)
		{
			chunks[i % keySize][chunkCounts[i % keySize]] = buffer[i];
			chunkCounts[i % keySize]++;
		}
	
		DynamicBuffer<uint8_t> *candidateKeys = new DynamicBuffer<uint8_t>[keySize];
	
		#pragma omp parallel for
		for(uint32_t i = 0; i < keySize; i++)
		{
			for(uint32_t j = 32; j < 128; j++)
			{
				uint32_t bufLen = chunkCounts[i];
				uint8_t *bufferCopy = new uint8_t[bufLen];
				memcpy(bufferCopy, chunks[i], bufLen);
	
				uint8_t key = j;
	
				Xor::vigenere(bufferCopy, bufLen, &key, 1);
	
				if(containsNonPrintableChars(bufferCopy, bufLen) == false && containsNonPrintableChars(&key, 1) == false)
					candidateKeys[i].append(key);
			}
		}

		bool checkPossibleKeys = true;
		for(uint32_t i = 0; i < keySize; i++)
		{
			if(candidateKeys[i].length() == 0)
			{
				checkPossibleKeys = false;
				break;
			}
		}
	
		if(checkPossibleKeys)
		{
			DynamicBuffer<char*> possibleKeys;
			getPossibleKeys(possibleKeys, candidateKeys, 0, keySize);
		
			for(uint32_t i = 0; i < possibleKeys.length(); i++)
			{
				printf("Possible key: %s\n", possibleKeys[i]);
				delete[] possibleKeys[i];
			}
		}

		for(uint32_t i = 0; i < keySize; i++)
			delete[] chunks[i];
	
		delete[] candidateKeys;
		delete[] chunkCounts;
		delete[] chunks;
	}
}

/* This function takes inBuffer and bufferSize as inputs.
 * It encrypts the data, randomly appending between 5-10 bytes to the beginning and end of the buffer,
 * and randomly chooses between ECB or CBC mode. It randomly generates a key upon each call.
 * The idea here is that one is blind to the type of encryption being used each time this function is called.
 * outBuffer is and output that is dynamically allocated and MUST BE DELETED BY THE CALLER OF THE FUNCTION.
 * outBuffSize is also an output and is set to the size of the data in outBuffer.
 */
void regurgitateAES(uint8_t *inBuffer, uint32_t bufferSize, uint8_t *&outBuffer, uint32_t &outBuffSize)
{
	uint8_t aesKey[16] = {0};
	randomBytes(aesKey, 16);

	uint32_t appendCount = 5 + (randomByte() % 6);
	uint8_t appendRandom[16] = {0};
	randomBytes(appendRandom, appendCount);

	uint8_t *inBuff = new uint8_t[bufferSize + (appendCount * 2) + 16];
	memcpy(inBuff, appendRandom, appendCount);
	memcpy(inBuff+appendCount, inBuffer, bufferSize);
	randomBytes(appendRandom, appendCount);
	memcpy(inBuff+bufferSize+appendCount, appendRandom, appendCount);

	uint32_t ibSize = (bufferSize + (appendCount * 2));

	if(randomByte() % 2 == 0)
	{ // ECB
		printf("Using ECB\n");

		uint32_t paddedSize = (ibSize % 16 != 0) ? AES::pkcs7_pad(inBuff, ibSize) : ibSize;
		outBuffer = new uint8_t[paddedSize];
		AES::AES128_ecb_encrypt((char*) aesKey, inBuff, outBuffer, (paddedSize / 16));
		outBuffSize = paddedSize;

	} else { // CBC

		printf("Using CBC\n");
		outBuffSize = AES::AES128_cbc_encrypt((char*) aesKey, inBuff, inBuff, ibSize);
		outBuffer = new uint8_t[outBuffSize];
		memcpy(outBuffer, inBuff, outBuffSize);

	}
}

void breakME(uint8_t *prepend, uint32_t ppLength, uint8_t *&outBuffer, uint32_t &outputSize)
{
	uint8_t unknownB64[] = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";

	uint32_t unkStrSize = Base64::decodedSize((char*) unknownB64);
	uint8_t *unkStr = new uint8_t[unkStrSize];
	Base64::decode((char*) unknownB64, unkStr);

	outBuffer = new uint8_t[ppLength + unkStrSize + 16];
	memcpy(outBuffer, prepend, ppLength);
	memcpy(outBuffer+ppLength, unkStr, unkStrSize);

	outputSize = ppLength + unkStrSize;

	uint32_t paddedSize = (outputSize % 16 != 0) ? AES::pkcs7_pad(outBuffer, outputSize) : outputSize;
	outputSize = paddedSize;

	char aesKey[17] = "YELLOW SUBMARINE";
	AES::AES128_ecb_encrypt(aesKey, outBuffer, outBuffer, (outputSize / 16));

	delete[] unkStr;
}

void printHexBlocks(uint8_t *buffer, uint32_t bufferSize)
{
	uint32_t blockNumber = 1;
	for(uint32_t i = 0; i < bufferSize; i++)
	{
		if(i > 0 && i % 16 == 0)
			printf("\n");

		if(i % 16 == 0)
			printf("%3lu: ", blockNumber++);

		printf("%.2x ", buffer[i]);
	}

	printf("\n\n");
}


#endif