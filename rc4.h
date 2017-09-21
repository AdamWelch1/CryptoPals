#ifndef RC4_H
#define RC4_H

#include <cstdint>

class RC4 {
	
	public:

		// Key-schedule
		RC4(uint8_t *key, uint32_t length)
		{
			doKeySchedule(key, length);
		}

		void doKeySchedule(uint8_t *key, uint32_t length)
		{
			for(uint32_t i = 0; i < 256; i++)
				iState[i] = i;

			uint16_t j;

			for(uint32_t i = 0; i < 256; i++)
			{
				j = (j + iState[i] + key[i % length]) % 256;

				uint8_t tmp = iState[i];
				iState[i] = iState[j];
				iState[j] = tmp;
			}
		}

		void getBytes(uint8_t *outBuffer, uint32_t count)
		{
			uint16_t i = 0, j = 0;

			for(uint32_t c = 0; c < count; c++)
			{
				i = (i + 1) % 256;
				j = (j + iState[i]) % 256;

				uint8_t tmp = iState[i];
				iState[i] = iState[j];
				iState[j] = tmp;

				outBuffer[c] = iState[(iState[i] + iState[j]) % 256];
			}
		}

	private:

		uint8_t iState[256];
};

#endif