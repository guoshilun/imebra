/*
$fileHeader$
*/

/*! \file memoryStream.cpp
    \brief Implementation of the memoryStream class.

*/

#include "exceptionImpl.h"
#include "memoryStreamImpl.h"
#include <string.h>

namespace puntoexe
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// memoryStream
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Constructor
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
memoryStreamInput::memoryStreamInput(std::shared_ptr<const memory> memoryStream): m_memory(memoryStream)
{
}


memoryStreamOutput::memoryStreamOutput(std::shared_ptr<memory> memoryStream): m_memory(memoryStream)
{
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Write raw data into the stream
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void memoryStreamOutput::write(std::uint32_t startPosition, const std::uint8_t* pBuffer, std::uint32_t bufferLength)
{
	PUNTOEXE_FUNCTION_START(L"memoryStream::write");

	// Nothing happens if we have nothing to write
	///////////////////////////////////////////////////////////
	if(bufferLength == 0)
	{
		return;
	}

	// Copy the buffer into the memory
	///////////////////////////////////////////////////////////
	if(startPosition + bufferLength > m_memory->size())
	{
		std::uint32_t newSize = startPosition + bufferLength;
		std::uint32_t reserveSize = ((newSize + 1023) >> 10) << 10; // preallocate blocks of 1024 bytes
		m_memory->reserve(reserveSize);
		m_memory->resize(startPosition + bufferLength);
	}

	::memcpy(m_memory->data() + startPosition, pBuffer, bufferLength);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Read raw data from the stream
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::uint32_t memoryStreamInput::read(std::uint32_t startPosition, std::uint8_t* pBuffer, std::uint32_t bufferLength)
{
	PUNTOEXE_FUNCTION_START(L"memoryStream::read");

	if(bufferLength == 0)
	{
		return 0;
	}

	// Don't read if the requested position isn't valid
	///////////////////////////////////////////////////////////
	std::uint32_t memorySize = m_memory->size();
	if(startPosition >= memorySize)
	{
		return 0;
	}

	// Check if all the bytes are available
	///////////////////////////////////////////////////////////
	std::uint32_t copySize = bufferLength;
	if(startPosition + bufferLength > memorySize)
	{
		copySize = memorySize - startPosition;
	}

	if(copySize == 0)
	{
		return 0;
	}

	::memcpy(pBuffer, m_memory->data() + startPosition, copySize);

	return copySize;

	PUNTOEXE_FUNCTION_END();
}


} // namespace puntoexe
