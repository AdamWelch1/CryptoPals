#ifndef FILEIO_H
#define FILEIO_H

#include <cstdio>
#include <cstdint>
#include <cassert>

class FileIO
{
	public:

		FileIO(char *fileName, char *fileMode)
		{
			fileHandle = fopen(fileName, fileMode);
			fileBuffer = 0;
			fileSize = -1;

			#ifdef DEBUG_MODE
				assert(fileHandle != 0); // Abort if fopen fails
			#endif
		}

		~FileIO()
		{
			if(fileHandle != 0)
				fclose(fileHandle);

			if(fileBuffer != 0)
				delete[] fileBuffer;
		}

		int64_t getSize()
		{
			if(fileSize == -1)
			{
				fseek(fileHandle, 0, SEEK_END);
				fileSize = ftell(fileHandle);
				rewind(fileHandle);
			}

			return fileSize;
		}

		const uint8_t* getContents()
		{
			if(fileSize == -1)
				getSize();

			if(fileBuffer != 0)
				delete[] fileBuffer;

			fileBuffer = new uint8_t[fileSize];
			fread(fileBuffer, fileSize, 1, fileHandle);

			fclose(fileHandle);
			fileHandle = 0;

			return fileBuffer;
		}

		
	private:

		FILE *fileHandle;

		// Contains the data read from the file
		uint8_t *fileBuffer;
		int64_t fileSize;

};


#endif