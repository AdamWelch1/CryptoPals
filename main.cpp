#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "b64.h"

int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	char encoded[1000] = {0};
	char str[] = "any carnal pleasur";

	Base64::encode((uint8_t*) str, strlen(str), encoded);

	printf(encoded);
	printf("\n");

	int decodedSize = Base64::decodedSize(encoded);
	char decoded[100] = {0};
	Base64::decode(encoded, (uint8_t*) decoded);

	printf("%lu - %s", decodedSize, decoded);

	printf("\n\n");
	return 0;
}