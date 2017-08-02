#ifndef HEX_H
#define HEX_H

#include <cstring>
#include <cstdio>

class Hex {
	
	public:

		static void encode(char *data, char *outBuffer)
		{
			Hex::encode((uint8_t*) data, strlen(data), outBuffer);
		}

		static void encode(uint8_t *data, uint32_t dataSize, char *outBuffer)
		{
			for(uint32_t i = 0; i < dataSize; i++)
				sprintf((outBuffer + (i * 2)), "%.2X", data[i]);
		}

		static void decode(char *data, uint8_t *outBuffer)
		{
			// Convert lower-case letters to upper-case
			for(int i = 0; data[i] != 0; i++)
			{
				if(data[i] >= 'a' && data[i] <= 'f')
					data[i] -= 32;
			}

			char hexMap[] = "0123456789ABCDEF";

			uint8_t byte = 0;
			for(uint32_t i = 0; i < strlen(data); i++)
			{
				uint8_t hexPos = 0;

				for(uint32_t j = 0; j < 16; j++)
				{
					if(data[i] == hexMap[j])
					{
						hexPos = j;
						break;
					}
				}

				if(i % 2 == 0)
				{
					byte = 16 * hexPos;
				} else {

					byte += hexPos;
					outBuffer[i / 2] = byte;
				}
			}
		}
};


#endif