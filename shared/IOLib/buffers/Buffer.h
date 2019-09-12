#pragma once
#include <string>

/**
 * A class that reads/writes into a buffer.
 * It can also manage the buffer(increase its size, delete it)
 * The idea is taken from ByteBuffer java Class in NIO package
 * It has the concept of position, limit an capacity, although not exactly the same
 * because this class does not take them into account, but subclasses may use them
 */
class Buffer
{
public:
	Buffer(unsigned long bufferLength);
	/**
	 * Does not create a buffer, it just uses it for any read/write.
	 * If shouldManageBuffer is true then it will take care of deleting it.
	 */
	Buffer(char* buffer, size_t bufferLength, bool shouldManageBuffer);
	virtual ~Buffer();

	char* getData() const;
	size_t getLimit() const;

	bool readBool(unsigned offset);

	int8_t readInt8(unsigned offset);
	int16_t readInt16(unsigned offset);
	int32_t readInt32(unsigned offset);
	int64_t readInt64(unsigned offset);

	bool readString(std::string& outString, unsigned size, unsigned offset = 0);

	bool readBytes(void* buffer, unsigned bufferLen, unsigned offset = 0, bool failOnIncomplete = false) const;

	void updateLimit(unsigned offset, unsigned size);

	void writeCheck(size_t size, unsigned offset);

	bool writeBool(bool value, unsigned int offset = 0);

	void writeInt8(int8_t number, unsigned offset);
	void writeInt16(int16_t number, unsigned int offset = 0);
	void writeInt32(int32_t number, unsigned int offset = 0);
	void writeInt64(int64_t number, unsigned offset = 0);

	bool writeString(const std::string& inString, unsigned int offset = 0);

	bool writeBytes(void* input, unsigned int size, unsigned int offset = 0);

	void resize(unsigned int bufferLen, bool required = false);

	virtual void invalidate();

protected:
	unsigned int capacity;
	unsigned int limit;
	char* data;
	bool shouldManageBuffer;
};
