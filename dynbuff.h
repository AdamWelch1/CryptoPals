#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include <cstdio>
#include <cstdint>
#include <cassert>

template <class T>
class DynamicBuffer {

	public:

		DynamicBuffer();
		~DynamicBuffer();

		void append(T datum);
		void append(T *data, uint64_t dataSize);

		uint64_t length();
		void clear();

		T& operator[](uint64_t index);
		T& operator[](uint32_t index);
		T& operator[](uint16_t index);
		T& operator[](uint8_t index);
		T& operator[](int index);

		void printBuffer();

	private:

		T *db__buffer;
		uint64_t db__bufferLength;
		uint64_t db__bufferSize;

		void db__growBuffer(uint64_t requiredSize);

};

#include "dynbuff.cpp"

#endif