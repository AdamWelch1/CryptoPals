#ifndef B64_H
#define B64_H

#include <cstdint>
#include <cstdio>
#include <omp.h>
#include <cmath>
#include <cstring>

class Base64 {

	public:

		// Returns the size of the buffer needed to hold the
		// Base64 encoded input buffer
		static uint32_t encodedSize(uint64_t inputBufferSize);

		// Base64 encodes inputBuffer and writes the resulting
		// string to outBuffer
		static void encode(uint8_t *inputBuffer, uint64_t inputBufferSize, char *outBuffer);

		// Returns the size of the buffer needed to hold the
		// decoded Base64 string
		static uint32_t decodedSize(char *inputBuffer);

		// Decodes a Base64-encoded string and writes the resulting
		// data to outBuffer
		static void decode(char *inputBuffer, uint8_t *outBuffer);

	private:

};


#endif