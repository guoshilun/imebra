/*
$fileHeader$
*/

/*! \file data.cpp
    \brief Implementation of the data class.

*/

#include "exceptionImpl.h"
#include "streamReaderImpl.h"
#include "streamWriterImpl.h"
#include "dataImpl.h"
#include "dataSetImpl.h"
#include "bufferImpl.h"
#include "dataHandlerImpl.h"
#include "dataHandlerNumericImpl.h"
#include <iostream>

namespace puntoexe
{

namespace imebra
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// data
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
// Set a buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void data::setBuffer(std::uint32_t bufferId, const std::shared_ptr<buffer>& newBuffer)
{
	PUNTOEXE_FUNCTION_START(L"data::setBuffer");

	// Assign the new buffer
	///////////////////////////////////////////////////////////
	m_buffers[bufferId] = newBuffer;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Remove a buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void data::deleteBuffer(std::uint32_t bufferId)
{
	PUNTOEXE_FUNCTION_START(L"data::deleteBuffer");

	// Remove the buffer
	///////////////////////////////////////////////////////////
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	if(findBuffer != m_buffers.end())
	{
		m_buffers.erase(findBuffer);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the buffer's data type
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string data::getDataType()
{
	PUNTOEXE_FUNCTION_START(L"data::getDataType");

	tBuffersMap::iterator findBuffer = m_buffers.find(0);
	if(findBuffer != m_buffers.end())
	{
		return findBuffer->second->getDataType();
	}
	
	return "";

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the number of buffers in the tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::uint32_t data::getBuffersCount()
{
	PUNTOEXE_FUNCTION_START(L"data::getBuffersCount");

	// Returns the number of buffers
	///////////////////////////////////////////////////////////
	return m_buffers.size();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return true if the specified buffer exists
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool data::bufferExists(std::uint32_t bufferId)
{
	PUNTOEXE_FUNCTION_START(L"data::bufferExists");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	return (findBuffer != m_buffers.end());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the size of a buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::uint32_t data::getBufferSize(std::uint32_t bufferId)
{
	PUNTOEXE_FUNCTION_START(L"data::getBufferSize");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	if(findBuffer == m_buffers.end())
	{
		return 0;
	}

	// Retrieve the buffer's size
	///////////////////////////////////////////////////////////
	return findBuffer->second->getBufferSizeBytes();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get an handler (normal or raw) for the buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::shared_ptr<handlers::dataHandler> data::getDataHandler(std::uint32_t bufferId, bool bWrite, const std::string& defaultType)
{
	PUNTOEXE_FUNCTION_START(L"data::getDataHandler");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	std::shared_ptr<buffer> pTempBuffer;
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	if(findBuffer != m_buffers.end())
	{
		pTempBuffer = findBuffer->second;
	}

	// If the buffer doesn't exist, then create a new one
	///////////////////////////////////////////////////////////
	if(pTempBuffer == 0 && bWrite)
	{
        // If a buffer already exists, then use the already defined
        //  datatype
        ///////////////////////////////////////////////////////////
        if( !m_buffers.empty() && !m_buffers.begin()->second->getDataType().empty() )
        {
            pTempBuffer = std::make_shared<buffer>(m_buffers.begin()->second->getDataType());
        }
        else
        {
            pTempBuffer = std::make_shared<buffer>(defaultType);
        }

		pTempBuffer->setCharsetsList(&m_charsetsList);
		m_buffers[bufferId]=pTempBuffer;
	}

	// Retrieve the data handler
	///////////////////////////////////////////////////////////
	if(pTempBuffer == 0)
	{
		std::shared_ptr<handlers::dataHandler> emptyDataHandler;
		return emptyDataHandler;
	}
	
	return pTempBuffer->getDataHandler(bWrite);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a raw data handler
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::shared_ptr<handlers::dataHandlerRaw> data::getDataHandlerRaw(std::uint32_t bufferId, bool bWrite, const std::string& defaultType)
{
	PUNTOEXE_FUNCTION_START(L"data::getDataHandlerRaw");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	std::shared_ptr<buffer> pTempBuffer;
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	if(findBuffer != m_buffers.end() )
	{
		pTempBuffer = findBuffer->second;
	}

	// If the buffer doesn't exist, then create a new one
	///////////////////////////////////////////////////////////
	if( pTempBuffer == 0 && bWrite )
	{
        // If a buffer already exists, then use the already defined
        //  datatype
        ///////////////////////////////////////////////////////////
        if( !m_buffers.empty() )
        {
            pTempBuffer = std::make_shared<buffer>(m_buffers.begin()->second->getDataType());
        }
        else
        {
            pTempBuffer = std::make_shared<buffer>(defaultType);
        }

		pTempBuffer->setCharsetsList(&m_charsetsList);
		m_buffers[bufferId]=pTempBuffer;
	}

	// Retrieve the data handler
	///////////////////////////////////////////////////////////
	if( pTempBuffer == 0 )
	{
		std::shared_ptr<handlers::dataHandlerRaw> emptyDataHandler;
		return emptyDataHandler;
	}

	return pTempBuffer->getDataHandlerRaw(bWrite);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a stream reader that works on the buffer's data
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::shared_ptr<streamReader> data::getStreamReader(std::uint32_t bufferId)
{
	PUNTOEXE_FUNCTION_START(L"data::getStreamReader");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	std::shared_ptr<buffer> pTempBuffer;
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	if(findBuffer != m_buffers.end())
	{
		return findBuffer->second->getStreamReader();
	}

	std::shared_ptr<streamReader> emptyStream;
	return emptyStream;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a stream writer that works on the buffer's data
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::shared_ptr<streamWriter> data::getStreamWriter(std::uint32_t bufferId, const std::string& dataType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"data::getStream");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	std::shared_ptr<buffer> pTempBuffer;
	tBuffersMap::iterator findBuffer = m_buffers.find(bufferId);
	if(findBuffer != m_buffers.end())
	{
		pTempBuffer = findBuffer->second;
	}

	// If the buffer doesn't exist, then create a new one
	///////////////////////////////////////////////////////////
	if(pTempBuffer == 0)
	{
        // If a buffer already exists, then use the already defined
        //  datatype
        ///////////////////////////////////////////////////////////
        if( !m_buffers.empty() )
        {
            pTempBuffer = std::make_shared<buffer>(m_buffers.begin()->second->getDataType());
        }
        else
        {
            pTempBuffer = std::make_shared<buffer>(dataType);
        }

        m_buffers[bufferId]=pTempBuffer;
	}

	return pTempBuffer->getStreamWriter();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve an embedded data set.
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::shared_ptr<dataSet> data::getDataSet(std::uint32_t dataSetId)
{
	PUNTOEXE_FUNCTION_START(L"data::getDataSet");

	// Retrieve the buffer
	///////////////////////////////////////////////////////////
	if(m_embeddedDataSets.size() <= dataSetId)
	{
		return 0;
	}

	return m_embeddedDataSets[dataSetId];

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set a data set
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void data::setDataSet(std::uint32_t dataSetId, std::shared_ptr<dataSet> pDataSet)
{
	PUNTOEXE_FUNCTION_START(L"data::setDataSet");

	if(dataSetId >= m_embeddedDataSets.size())
	{
		m_embeddedDataSets.resize(dataSetId + 1);
	}
	m_embeddedDataSets[dataSetId] = pDataSet;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Append a data set
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void data::appendDataSet(std::shared_ptr<dataSet> pDataSet)
{
	PUNTOEXE_FUNCTION_START(L"data::appendDataSet");

	m_embeddedDataSets.push_back(pDataSet);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Define the charset to use in the buffers and embedded
//  datasets
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void data::setCharsetsList(charsetsList::tCharsetsList* pCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"data::setCharsetsList");

	m_charsetsList.clear();
	charsetsList::updateCharsets(pCharsetsList, &m_charsetsList);

	for(tEmbeddedDatasetsMap::iterator scanEmbeddedDataSets = m_embeddedDataSets.begin(); scanEmbeddedDataSets != m_embeddedDataSets.end(); ++scanEmbeddedDataSets)
	{
		(*scanEmbeddedDataSets)->setCharsetsList(pCharsetsList);
	}

	for(tBuffersMap::iterator scanBuffers = m_buffers.begin(); scanBuffers != m_buffers.end(); ++scanBuffers)
	{
		scanBuffers->second->setCharsetsList(pCharsetsList);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the charset used by the buffers and the embedded
//  datasets
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void data::getCharsetsList(charsetsList::tCharsetsList* pCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"data::getCharsetsList");

	m_charsetsList.clear();

	for(tEmbeddedDatasetsMap::iterator scanEmbeddedDataSets = m_embeddedDataSets.begin(); scanEmbeddedDataSets != m_embeddedDataSets.end(); ++scanEmbeddedDataSets)
	{
		charsetsList::tCharsetsList charsets;
		(*scanEmbeddedDataSets)->getCharsetsList(&charsets);
		charsetsList::updateCharsets(&charsets, &m_charsetsList);
	}

	for(tBuffersMap::iterator scanBuffers = m_buffers.begin(); scanBuffers != m_buffers.end(); ++scanBuffers)
	{
		charsetsList::tCharsetsList charsets;
		scanBuffers->second->getCharsetsList(&charsets);
		charsetsList::updateCharsets(&charsets, &m_charsetsList);
	}

    pCharsetsList->insert(pCharsetsList->end(), m_charsetsList.begin(), m_charsetsList.end());

	PUNTOEXE_FUNCTION_END();
}


} // namespace imebra

} // namespace puntoexe