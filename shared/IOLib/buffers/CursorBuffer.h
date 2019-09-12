#pragma once
#include "Buffer.h"


class CursorBuffer : public Buffer
{
public:

	CursorBuffer(int bufferLen);
	
	CursorBuffer(char* buffer, size_t buffer_length, bool shouldDeleteBuffer);
	~CursorBuffer();

	bool readBytes(void* buffer, unsigned bufferLen, bool failOnIncomplete = false);
	void invalidate();
	bool readString(unsigned size, std::string& outString);
	
	bool writeString(const std::string& inString);
	bool writeBytes(void* input, unsigned size);
	void writeBool(bool value);
	unsigned int getWritingCursorPosition();
	void setWritingCursorPosition(unsigned int cursorPos);
	void setReadingCursorPosition(unsigned cursorPos);
	void resetCursors();
	void resetReadingCursor();
	void resetWritingCursor();
	
	void writeInt8(int8_t action);
	void writeInt16(int16_t number);
	void writeInt32(int32_t number);
	void writeInt64(int64_t number);
	

	int8_t readInt8();
	int16_t readInt16();
	int32_t readInt32();
	int64_t readInt64();

	bool readBool();
	unsigned getReadingCursorPosition() const;
	

private:
	unsigned int writingCursor;
	unsigned int readingCursor;
};
