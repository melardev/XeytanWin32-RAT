#include "Buffer.h"
#include <cstring>

Buffer::Buffer(unsigned long bufferLength)
{
	data = new char[bufferLength];
	capacity = bufferLength;
	limit = 0;
}

Buffer::Buffer(char* buffer, size_t bufferLength, bool shouldManageBuffer)
{
	data = buffer;
	capacity = bufferLength;
	limit = bufferLength; // I assume this buffer is filled so limit is pointing to the end
	this->shouldManageBuffer = shouldManageBuffer;
}

Buffer::~Buffer()
{
	if (!shouldManageBuffer)
		return;

	// Very important, do not delete the buffer if
	// this buffer is not managing it
	capacity = 0;
	limit = 0;
	delete[] data;
}

char* Buffer::getData() const
{
	return data;
}

size_t Buffer::getLimit() const
{
	return limit;
}

void Buffer::resize(unsigned bufferLen, bool required /*=false*/)
{
	// If we already have a buffer greater than bufferLen then return, unless required is set to true
	if (capacity > bufferLen && !required)
		return;

	// Allocate the new buffer
	void* newBuffer = malloc(bufferLen);

	// Move the previous content to new buffer, does not make sense to copy
	// this->capacity, we copy only the interesting data, which has a length of this->limit
	memcpy_s(newBuffer, limit, data, limit);
	// Delete old buffer
	delete[] data;

	// Point the buffer to the new allocated space
	data = (char*)newBuffer;
	capacity = bufferLen; // Update the capacity
}

void Buffer::writeCheck(size_t size, unsigned offset)
{
	// Overflow check, + 1 because offset is given as 0 based index
	if ((offset + 1 + size) > capacity)
	{
		// Our buffer can not hold the data

		if (shouldManageBuffer)
		{
			// This class manages the buffer
			// Reallocate
			resize(capacity * 2);
		}
		else
		{
			// This class does not manage the buffer
			// TODO: Throw BufferOverflow exception
		}
	}
}

void Buffer::updateLimit(unsigned offset, unsigned size)
{
	if (offset + size > limit)
		limit += (offset + size - limit);
}

void Buffer::invalidate()
{
	// Reset buffer state
	memset(data, 0x00, capacity);
	limit = 0;
}

// ================================================================================
//				Read Operations
// The reasoning behind the ugly casting code is: data is just a char* pointer
// that may hold anything (bytes at the end of the day), we make it
// point(through casting) to some data type, for example to read a bool we cast
// the char* to bool*, since we don't want the address, but the value itself, we
// dereference the pointer so *(bool*) and now we get the byte at that location, as
// a boolean. data is a pointer that points to the beginning of the buffer, to read
// 2 bytes ahead we do data + 2. data + offset for dynamic offset value.
// ================================================================================

bool Buffer::readBool(unsigned offset)
{
	return *(bool*)(data + offset);
}

int8_t Buffer::readInt8(unsigned offset)
{
	return *(int8_t*)(data + offset);
}

int16_t Buffer::readInt16(unsigned offset)
{
	return *(int16_t*)(data + offset);
}

int64_t Buffer::readInt64(unsigned offset)
{
	return *(int64_t*)(data + offset);
}

int32_t Buffer::readInt32(unsigned offset)
{
	return *(int32_t*)(data + offset);
}

bool Buffer::readString(std::string& outString, unsigned int size, unsigned int offset)
{
	// Method 1, quick and easy
	outString = std::string(data, offset, size);

	/* Method 2.
	char* result = new char [size + 1];
	strcpy_s(result, size, data + offset);
	result[size] = '\0';
	outString = result;
	delete[] result;
	*/
	return true;
}

bool Buffer::readBytes(void* buffer, unsigned int bufferLen, unsigned int offset, bool failOnIncomplete) const
{
	// If the consumer wants everything but the provided out buffer can not hold
	// all of it, then we may return false, it depends on failOnIncomplete

	if (failOnIncomplete && limit > bufferLen)
		return false;

	memcpy_s(buffer, bufferLen, data + offset, bufferLen);

	return true;
}

// ================================================================================
//				Write Operations
// ================================================================================

bool Buffer::writeString(const std::string& inString, unsigned int offset)
{
	writeCheck(inString.size(), offset);
	strcpy_s(data + offset, inString.size(), inString.c_str());
	limit += (offset >= limit) ? inString.size() : 0;
	return true;
}

bool Buffer::writeBytes(void* input, unsigned int size, unsigned int offset)
{
	writeCheck(size, offset);
	memcpy_s(data + offset, size, input, size);
	updateLimit(offset, size);
	return true;
}

bool Buffer::writeBool(bool value, unsigned int offset)
{
	writeCheck(1, offset);
	memcpy_s(data + offset, 1, &value, 1);
	updateLimit(offset, 1);
	return true;
}

void Buffer::writeInt8(int8_t number, unsigned int offset)
{
	writeCheck(sizeof int8_t, offset);
	memcpy_s(data + offset, sizeof int8_t, (void*)& number, sizeof int8_t);
	updateLimit(offset, sizeof int8_t);
}

void Buffer::writeInt16(int16_t number, unsigned int offset)
{
	writeCheck(sizeof int16_t, offset);
	memcpy_s(data + offset, sizeof int16_t, (void*)&number, sizeof int16_t);
	updateLimit(offset, sizeof int16_t);
}

void Buffer::writeInt32(int32_t number, unsigned int offset)
{
	writeCheck(sizeof int32_t, offset);
	memcpy_s(data + offset, sizeof int32_t, (void*)&number, sizeof int32_t);
	updateLimit(offset, sizeof int32_t);
}

void Buffer::writeInt64(int64_t number, unsigned offset)
{
	writeCheck(sizeof int64_t, offset);
	memcpy_s(data + offset, sizeof int64_t, (void*)&number, sizeof int64_t);
	updateLimit(offset, sizeof int64_t);
}
