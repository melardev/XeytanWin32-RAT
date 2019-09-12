#include "CursorBuffer.h"

CursorBuffer::CursorBuffer(int bufferLen) : Buffer(bufferLen), writingCursor(0), readingCursor(0)
{
}

CursorBuffer::CursorBuffer(char* buffer, size_t bufferLength, bool shouldDeleteBuffer) :
	Buffer(buffer, bufferLength, shouldDeleteBuffer), writingCursor(0), readingCursor(0)
{
}

CursorBuffer::~CursorBuffer()
{
}

bool CursorBuffer::readBytes(void* buffer, unsigned int bufferLen, bool failOnIncomplete)
{
	Buffer::readBytes(buffer, bufferLen, readingCursor, failOnIncomplete);
	readingCursor += bufferLen;
	return true;
}


bool CursorBuffer::readString(unsigned int size, std::string& outString)
{
	Buffer::readString(outString, size, readingCursor);
	readingCursor += size;
	return true;
}

bool CursorBuffer::writeString(const std::string& inString)
{
	Buffer::writeString(inString, writingCursor);
	writingCursor += inString.size();
	return true;
}

bool CursorBuffer::writeBytes(void* input, unsigned int size)
{
	Buffer::writeBytes(input, size, writingCursor);
	writingCursor += size;
	return true;
}


void CursorBuffer::writeBool(bool value)
{
	Buffer::writeBool(value, writingCursor);
	writingCursor += 1;
}

unsigned CursorBuffer::getReadingCursorPosition() const
{
	return readingCursor;
}

unsigned CursorBuffer::getWritingCursorPosition()
{
	return writingCursor;
}

void CursorBuffer::setWritingCursorPosition(unsigned cursorPos)
{
	writingCursor = cursorPos;
}

void CursorBuffer::setReadingCursorPosition(unsigned cursorPos)
{
	readingCursor = cursorPos;
}

void CursorBuffer::writeInt8(int8_t number)
{
	Buffer::writeInt8(number, writingCursor);
	writingCursor += sizeof int8_t;
}

void CursorBuffer::writeInt16(int16_t number)
{
	Buffer::writeInt16(number, writingCursor);
	writingCursor += sizeof int16_t;
}

void CursorBuffer::writeInt32(int32_t number)
{
	Buffer::writeInt32(number, writingCursor);
	writingCursor += sizeof int32_t;
}

void CursorBuffer::writeInt64(int64_t number)
{
	Buffer::writeInt64(number, writingCursor);
	writingCursor += sizeof int64_t;
}

bool CursorBuffer::readBool()
{
	const int16_t result = Buffer::readBool(readingCursor);
	readingCursor += 1;
	return result;
}

int16_t CursorBuffer::readInt16()
{
	const int16_t result = Buffer::readInt16(readingCursor);
	readingCursor += sizeof int16_t;
	return result;
}

int32_t CursorBuffer::readInt32()
{
	const int32_t result = Buffer::readInt32(readingCursor);
	readingCursor += sizeof int32_t;
	return result;
}

int8_t CursorBuffer::readInt8()
{
	const int8_t result = Buffer::readInt8(readingCursor);
	readingCursor += sizeof int8_t;
	return result;
}

int64_t CursorBuffer::readInt64()
{
	const int64_t result = Buffer::readInt64(readingCursor);
	readingCursor += sizeof int64_t;
	return result;
}

void CursorBuffer::resetCursors()
{
	resetReadingCursor();
	resetWritingCursor();
}

void CursorBuffer::resetReadingCursor()
{
	readingCursor = 0;
}

void CursorBuffer::resetWritingCursor()
{
	writingCursor = 0;
}


void CursorBuffer::invalidate()
{
	Buffer::invalidate();
	resetCursors();
}
