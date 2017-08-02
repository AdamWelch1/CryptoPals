#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "b64.h"
#include "hex.h"

int main(int argc, char *argv[])
{
	#ifdef DEBUG_MODE
		printf("\n***DEBUG MODE***\n\n");
	#endif

	char hexBytes[] = "49276D206B696C6C696E6720796F757220627261696E206C696B65206120706F69736F6E6F7573206D757368726F6F6D";
	uint8_t rawBytes[49] = {0};

	Hex::decode(hexBytes, rawBytes);
	printf("%s\n", rawBytes);

	char b64Encoded[200] = {0};
	Base64::encode(rawBytes, 48, b64Encoded);

	printf("Base64: %s\n", b64Encoded);
	

	printf("\n\n");
	return 0;
}