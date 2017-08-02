#ifndef UTILITY_H
#define UTILITY_H

#include <cstdio>
#include <cstring>

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

uint32_t scoreEnglish(uint8_t *buffer, uint32_t length)
{
	char *commonEnglish[20] = {"the", "be", "to", "of", "and", "a", "in", "that", "have", "i",
								"it", "for", "not", "on", "with", "he", "as", "you", "do", "at"};

	uint32_t score = 0;
	for(uint32_t i = 0; i < 20; i++)
		score += (strinstr(commonEnglish[i], (char*) buffer) == true) ? 1 : 0;
	
	return score;
}


#endif