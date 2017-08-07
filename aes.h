#ifndef AES_H
#define AES_H

#include <tomcrypt.h>

class AES {

	public:

		static void AES128_ecb_decrypt(char *key, uint8_t *inBuffer, uint8_t *outBuffer, uint32_t blockCount)
		{
			symmetric_key keyStruct;
			aes_setup((uint8_t*) key, 16, 0, &keyStruct);

			for(uint32_t i = 0; i < blockCount; i++)
				aes_ecb_decrypt(inBuffer+(i * 16), outBuffer+(i * 16), &keyStruct);
		}	

};


#endif