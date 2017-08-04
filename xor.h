#ifndef XOR_H
#define XOR_H

class Xor {

	public:

		// Simply XOR's two equal-length buffers
		// 'xor' values end up in buf1
		static void xorBuffers(uint8_t *buf1, uint8_t *buf2, uint32_t bufLength)
		{
			#pragma omp parallel for
			for(uint32_t i = 0; i < bufLength; i++)
				buf1[i] ^= buf2[i];
		}

		static void vigenere(uint8_t *buffer, uint32_t bufferLength, uint8_t *key, uint32_t keyLength)
		{
			for(uint32_t i = 0; i < bufferLength; i++)
			{
			//	printf("%.2X ^= %.2x ", buffer[i], key[i % keyLength]);
				buffer[i] ^= key[i % keyLength];
			}

			//printf("\n\n");
		}
};

#endif