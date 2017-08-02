#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>


#include "utility.h"
#include "b64.h"
#include "hex.h"
#include "xor.h"


int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	char hexStr[] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
	uint8_t rawBuffer[34] = {0};
	Hex::decode(hexStr, rawBuffer);

	for(uint32_t i = 0; i < 256; i++)
	{
		uint8_t copyBuffer[35] = {0};
		memcpy(copyBuffer, rawBuffer, 34);
		copyBuffer[34] = 0; // Forced null termination

		uint8_t key = i;
		Xor::vigenere(copyBuffer, 34, &key, 1);

		maskNonPrintableChars(copyBuffer, 34);
		uint32_t engScore = scoreEnglish(copyBuffer, 34);

		if(engScore > 1)
			printf("%lu : <%lu> : %s\n", i, engScore, copyBuffer);
	}

	printf("\n\n");
	return 0;
}