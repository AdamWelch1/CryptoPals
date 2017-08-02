#include "dynbuff.h"

#define DEFAULT_BUFFER_SIZE	4096 // 4KB

template <class T>
DynamicBuffer<T>::DynamicBuffer()
{
	db__buffer = new T[DEFAULT_BUFFER_SIZE];
	db__bufferLength = 0;
	db__bufferSize = DEFAULT_BUFFER_SIZE;
}

template <class T>
DynamicBuffer<T>::~DynamicBuffer()
{
	delete[] db__buffer;
}

template <class T>
void DynamicBuffer<T>::append(T datum)
{
	if((db__bufferLength + 1) >= db__bufferSize)
		db__growBuffer(db__bufferLength + 1);

	db__buffer[db__bufferLength] = datum;
	db__bufferLength++;
}

template <class T>
void DynamicBuffer<T>::append(T *data, uint64_t dataSize)
{
	if((db__bufferLength + dataSize) >= db__bufferSize)
		db__growBuffer(db__bufferLength + dataSize);

	for(uint64_t i = 0; i < dataSize; i++)
		db__buffer[db__bufferLength + i] = data[i];
	
	db__bufferLength += dataSize;
}

template <class T>
void DynamicBuffer<T>::db__growBuffer(uint64_t requiredSize)
{
	uint64_t newSize = db__bufferSize;

	while(newSize <= requiredSize)
		newSize += DEFAULT_BUFFER_SIZE;

	T *newBuffer = new T[newSize];

	for(uint64_t i = 0; i < db__bufferLength; i++)
		newBuffer[i] = db__buffer[i];

	delete[] db__buffer;

	db__buffer = newBuffer;
	db__bufferSize = newSize;
}

template <class T>
uint64_t DynamicBuffer<T>::length()
{
	return db__bufferLength;
}

template <class T>
void DynamicBuffer<T>::clear()
{
	delete[] db__buffer;
	db__bufferSize = DEFAULT_BUFFER_SIZE;
	db__bufferLength = 0;

	db__buffer = new T[db__bufferSize];
}

template <class T>
T& DynamicBuffer<T>::operator[](uint64_t index)
{
	#ifdef DEBUG_MODE
		assert(index < db__bufferLength);
	#endif

	return db__buffer[index];
}
template <class T>
T& DynamicBuffer<T>::operator[](uint32_t index)
{
	#ifdef DEBUG_MODE
		assert(index < db__bufferLength);
	#endif

	return db__buffer[index];
}
template <class T>
T& DynamicBuffer<T>::operator[](uint16_t index)
{
	#ifdef DEBUG_MODE
		assert(index < db__bufferLength);
	#endif

	return db__buffer[index];
}
template <class T>
T& DynamicBuffer<T>::operator[](uint8_t index)
{
	#ifdef DEBUG_MODE
		assert(index < db__bufferLength);
	#endif

	return db__buffer[index];
}

template <class T>
T& DynamicBuffer<T>::operator[](int index)
{
	#ifdef DEBUG_MODE
		assert(index < db__bufferLength && index >= 0);
	#endif

	return db__buffer[index];
}

template <class T>
void DynamicBuffer<T>::printBuffer()
{
	#ifdef DEBUG_MODE
		for(uint64_t i = 0; i < db__bufferLength; i++)
		{
			if(sizeof(T) == 1)
				printf("%.2X ", db__buffer[i]);

			if(sizeof(T) == 2)
				printf("%.4X ", db__buffer[i]);

			if(sizeof(T) == 4)
				printf("%.8X ", db__buffer[i]);

			if(sizeof(T) == 8)
				printf("%.16X ", db__buffer[i]);
		}
		printf("\n");
	#endif
}