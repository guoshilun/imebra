/*
$fileHeader$
*/

/*! \file dataSet.cpp
    \brief Implementation of the class dataSet.

*/

#include "../../base/include/exception.h"
#include "../../base/include/streamReader.h"
#include "../../base/include/streamWriter.h"
#include "../../base/include/memoryStream.h"
#include "../include/dataSet.h"
#include "../include/dataGroup.h"
#include "../include/dataHandlerNumeric.h"
#include "../include/dicomDict.h"
#include "../include/codecFactory.h"
#include "../include/codec.h"
#include "../include/image.h"
#include "../include/LUT.h"
#include "../include/colorTransformsFactory.h"
#include "../include/transformsChain.h"
#include "../include/transformHighBit.h"
#include "../include/transaction.h"
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
// dataSet
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
// Retrieve the requested tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<data> dataSet::getTag(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, bool bCreate /* =false */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getTag");

	lockObject lockAccess(this);

	ptr<data> pData;

	ptr<dataGroup>	group=getGroup(groupId, order, bCreate);
	if(group != 0)
	{	
		pData=group->getTag(tagId, bCreate);
	}

	return pData;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the requested group
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<dataGroup> dataSet::getGroup(imbxUint16 groupId, imbxUint16 order, bool bCreate /* =false */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getGroup");

	lockObject lockAccess(this);
	
	ptr<dataGroup> pData=getData(groupId, order);

	if(pData == 0 && bCreate)
	{
		ptr<dataGroup> tempGroup(new dataGroup(m_externalLock));
		pData = tempGroup;
		setGroup(groupId, order, pData);
	}

	return pData;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the requested group
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setGroup(imbxUint16 groupId, imbxUint16 order, ptr<dataGroup> pGroup)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setGroup");

	setData(groupId, order, pGroup);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the image from the structure
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<image> dataSet::getImage(imbxUint32 frameNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getImage");

	// Lock this object
	///////////////////////////////////////////////////////////
	lockObject lockAccess(this);

	// Shared pointer used to reference this dataset
	///////////////////////////////////////////////////////////
	ptr<dataSet> thisDataSet(this);

	// Retrieve the transfer syntax
	///////////////////////////////////////////////////////////
	std::wstring transferSyntax=getUnicodeString(0x0002, 0x0, 0x0010, 0x0);

	// Get the right codec
	///////////////////////////////////////////////////////////
	ptr<codecs::codec> pCodec=codecs::codecFactory::getCodec(transferSyntax);

	// Return if the codec has not been found
	///////////////////////////////////////////////////////////
	if(pCodec == 0)
	{
		PUNTOEXE_THROW(dataSetExceptionUnknownTransferSyntax, "None of the codecs support the specified transfer syntax");
	}

	ptr<imebra::data> imageTag = getTag(0x7fe0, 0x0, 0x0010, false);
	if(imageTag == 0)
	{
		PUNTOEXE_THROW(dataSetImageDoesntExist, "The requested image doesn't exist");
	}
	std::string imageStreamDataType = imageTag->getDataType();

	// Placeholder for the stream containing the image
	///////////////////////////////////////////////////////////
	ptr<streamReader> imageStream;

	// Retrieve the second item in the image's tag.
	// If the second item is present, then a multiframe
	//  image is present.
	///////////////////////////////////////////////////////////
	bool bDontNeedImagesPositions = false;
	{
		if(imageTag->getBufferSize(1) != 0)
		{
			imbxUint32 firstBufferId(0), endBufferId(0), totalLength(0);
			if(imageTag->getBufferSize(0) == 0)
			{
				firstBufferId = frameNumber + 1;
				endBufferId = firstBufferId + 1;
				totalLength = imageTag->getBufferSize(firstBufferId);
			}
			else
			{
				totalLength = getFrameBufferIds(frameNumber, &firstBufferId, &endBufferId);
			}
			if(firstBufferId == endBufferId - 1)
			{
				imageStream = imageTag->getStreamReader(firstBufferId);
				if(imageStream == 0)
				{
					PUNTOEXE_THROW(dataSetImageDoesntExist, "The requested image doesn't exist");
				}
			}
			else
			{
				ptr<memory> temporaryMemory(memoryPool::getMemoryPool()->getMemory(totalLength));
				const imbxUint8* pDest = temporaryMemory->getStringPointer()->data();
				for(imbxUint32 scanBuffers = firstBufferId; scanBuffers != endBufferId; ++scanBuffers)
				{
					ptr<handlers::dataHandlerRaw> bufferHandler = imageTag->getDataHandlerRaw(scanBuffers, false, "");
					imbxUint8* pSource = bufferHandler->getMemoryBuffer();
					::memcpy((void*)pDest, (void*)pSource, bufferHandler->getSize());
					pDest += bufferHandler->getSize();
				}
				ptr<baseStream> compositeStream(new memoryStream(temporaryMemory));
				imageStream = ptr<streamReader>(new streamReader(compositeStream));
			}
			bDontNeedImagesPositions = true;
		}
	}

	// If the image cannot be found, then probably we are
	//  handling an old dicom format.
	// Then try to read the image from the next group with
	//  id=0x7fe
	///////////////////////////////////////////////////////////
	if(imageStream == 0)
	{
		imageStream = getStreamReader(0x7fe0, (imbxUint16)frameNumber, 0x0010, 0x0);
		bDontNeedImagesPositions = true;
	}

	// We are dealing with an old dicom format that doesn't
	//  include the image offsets and stores all the images
	//  in one buffer
	///////////////////////////////////////////////////////////
	if(imageStream == 0)
	{
		imageStream = imageTag->getStreamReader(0x0);
		if(imageStream == 0)
		{
			PUNTOEXE_THROW(dataSetImageDoesntExist, "The requested image doesn't exist");
		}

		imbxUint32 numberOfFrames = 1;
		if(!getDataType(0x0028, 0, 0x0008).empty())
		{
			numberOfFrames = getUnsignedLong(0x0028, 0, 0x0008, 0);
		}
		if(frameNumber >= numberOfFrames)
		{
			PUNTOEXE_THROW(dataSetImageDoesntExist, "The requested image doesn't exist");
		}

		// Reset an internal array that keeps track of the
		//  images position
		///////////////////////////////////////////////////////////
		if(m_imagesPositions.size() != numberOfFrames)
		{
			m_imagesPositions.resize(numberOfFrames);
			
			for(imbxUint32 resetImagesPositions = 0; resetImagesPositions < numberOfFrames; m_imagesPositions[resetImagesPositions++] = 0)
			{}// empty loop

		}

		// Read all the images before the desidered one so we set
		//  reading position in the stream
		///////////////////////////////////////////////////////////
		for(imbxUint32 readImages = 0; readImages < frameNumber; readImages++)
		{
			imbxUint32 offsetPosition = m_imagesPositions[readImages];
			if(offsetPosition == 0)
			{
				ptr<image> tempImage = pCodec->getImage(thisDataSet, imageStream, imageStreamDataType);
				m_imagesPositions[readImages] = imageStream->position();
				continue;
			}
			if((m_imagesPositions[readImages + 1] == 0) || (readImages == (frameNumber - 1)))
			{
				imageStream->seek(offsetPosition);
			}
		}
	}

	double pixelDistanceX=getDouble(0x0028, 0x0, 0x0030, 0);
	double pixelDistanceY=getDouble(0x0028, 0x0, 0x0030, 1);
	if(bDontNeedImagesPositions)
	{
		lockAccess.unlock();
	}

	ptr<image> pImage;
	pImage = pCodec->getImage(thisDataSet, imageStream, imageStreamDataType);
	
	if(!bDontNeedImagesPositions && m_imagesPositions.size() > frameNumber)
	{
		m_imagesPositions[frameNumber] = imageStream->position();
	}
	
	// If the image has been returned correctly, then set
	//  the image's size
	///////////////////////////////////////////////////////////
	if(pImage != 0)
	{
		imbxUint32 sizeX, sizeY;
		pImage->getSize(&sizeX, &sizeY);
		pImage->setSizeMm(pixelDistanceX*(double)sizeX, pixelDistanceY*(double)sizeY);
	}

	return pImage;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Insert an image into the dataset
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setImage(imbxUint32 frameNumber, ptr<image> pImage, std::wstring transferSyntax, codecs::codec::quality quality)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setImage");

	// Lock the entire dataset
	///////////////////////////////////////////////////////////
	lockObject lockAccess(this);

	// All the commit are in one transaction
	///////////////////////////////////////////////////////////
	transaction localTransaction(true);

	// Shared pointer used to reference this dataset
	///////////////////////////////////////////////////////////
	ptr<dataSet> thisDataSet(this);

	// The group, order, tag and buffer where the image must
	//  be stored
	///////////////////////////////////////////////////////////
	imbxUint16 groupId(0x7fe0), orderId(0), tagId(0x0010);
	imbxUint32 firstBufferId(0), endBufferId(0);

	// Will store the current frame offsets
	///////////////////////////////////////////////////////////
	tFrameFirstBufferList frameFirstBufferList;

	// This flag will become true if some images already exist
	//  in the dataset and we must save the new image using
	//  the attributes already stored in the dataset
	///////////////////////////////////////////////////////////
	bool bDontChangeAttributes = (frameNumber != 0);

	// Check if we are dealing with an old Dicom format...
	///////////////////////////////////////////////////////////
	std::string dataHandlerType = getDataType(0x7fe0, 0x1, 0x0010);
	if( !dataHandlerType.empty() )
	{
		orderId=(imbxUint16)frameNumber;
		bDontChangeAttributes = true;
	}
	
	// ...this is a new Dicom format.
	// The first buffer is reserverd for the images offsets
	///////////////////////////////////////////////////////////
	else
	{
		// We have to add the offsets buffer
		///////////////////////////////////////////////////////////
		ptr<handlers::dataHandlerRaw> imageHandler0 = getDataHandlerRaw(groupId, 0x0, tagId, 0x0, false);
		ptr<handlers::dataHandlerRaw> imageHandler1 = getDataHandlerRaw(groupId, 0x0, tagId, 0x1, false);
		if(imageHandler0 != 0L && imageHandler0->getSize() != 0 && imageHandler1 == 0L)
		{
			// The first image must be moved forward, in order to
			//  make some room for the offsets buffer
			///////////////////////////////////////////////////////////
			dataHandlerType = imageHandler0->getDataType();
			ptr<handlers::dataHandlerRaw> moveFirstImage = getDataHandlerRaw(groupId, 0x0, tagId, 0x1, true, dataHandlerType);

			if(moveFirstImage == 0L)
			{
				PUNTOEXE_THROW(dataSetExceptionOldFormat, "Cannot move the first image");
			}
			imbxUint32 bufferSize=imageHandler0->getSize();
			moveFirstImage->setSize(bufferSize);
			::memcpy(moveFirstImage->getMemoryBuffer(), imageHandler0->getMemoryBuffer(), bufferSize);
			bDontChangeAttributes = true;
		}

		// An image in the first buffer already exists.
		///////////////////////////////////////////////////////////
		if(imageHandler1 != 0)
		{
			bDontChangeAttributes = true;
			dataHandlerType = imageHandler1->getDataType();
		}

		// Calculate the buffer ID to use
		///////////////////////////////////////////////////////////
		for(imbxUint32 scanFrames = 0; ; ++scanFrames)
		{
			try
			{
				imbxUint32 frameFirstBuffer, frameEndBuffer;
				getFrameBufferIds(scanFrames, &frameFirstBuffer, &frameEndBuffer);
				frameFirstBufferList.push_back(frameFirstBuffer);
				if(scanFrames == frameNumber)
				{
					firstBufferId = frameFirstBuffer;
					endBufferId = frameEndBuffer;
				}
			}
			catch(dataSetImageDoesntExist)
			{
				break;
			}
		}
		if(firstBufferId == 0)
		{
			firstBufferId = getFirstAvailFrameBufferId();
			endBufferId = firstBufferId + 1;
			frameFirstBufferList.push_back(firstBufferId);
		}
	}

	// Find the number of frames. We cannot skip frames while
	//  saving images.
	///////////////////////////////////////////////////////////
	imbxUint32 numberOfFrames = getUnsignedLong(0x0028, 0, 0x0008, 0);
	if(numberOfFrames == 0 && bDontChangeAttributes)
	{
		++numberOfFrames;
	}
	if(frameNumber > numberOfFrames)
	{
		PUNTOEXE_THROW(dataSetExceptionWrongFrame, "The frames must be inserted in sequence");
	}

	// If the attributes cannot be changed, then load the 
	//  transfer syntax from the dataset
	///////////////////////////////////////////////////////////
	if(bDontChangeAttributes)
	{
		transferSyntax = getUnicodeString(0x0002, 0x0, 0x0010, 0x0);
	}

	// Select the right codec
	///////////////////////////////////////////////////////////
	ptr<codecs::codec> saveCodec=codecs::codecFactory::getCodec(transferSyntax);
	if(saveCodec == 0L)
	{
		PUNTOEXE_THROW(dataSetExceptionUnknownTransferSyntax, "None of the codec support the requested transfer syntax");
	}

	// Set the subsampling flags
	///////////////////////////////////////////////////////////
	bool bSubSampledX = quality < codecs::codec::high;
	bool bSubSampledY = quality < codecs::codec::medium;
	if( !transforms::colorTransforms::colorTransformsFactory::canSubsample(pImage->getColorSpace()) )
	{
		bSubSampledX = bSubSampledY = false;
	}
	bool bInterleaved = false;
	bool b2complement = false;
	imbxUint32 channelsNumber = pImage->getChannelsNumber();
	imbxUint8 allocatedBits = (imbxUint8)(saveCodec->suggestAllocatedBits(transferSyntax, pImage->getHighBit()));

	// If the attributes cannot be changed, then check the
	//  attributes already stored in the dataset
	///////////////////////////////////////////////////////////
	if(bDontChangeAttributes)
	{
		pImage = convertImageForDataSet(pImage);
		std::wstring currentColorSpace = getUnicodeString(0x0028, 0x0, 0x0004, 0x0);
		bSubSampledX = transforms::colorTransforms::colorTransformsFactory::isSubsampledX(currentColorSpace);
		bSubSampledY = transforms::colorTransforms::colorTransformsFactory::isSubsampledY(currentColorSpace);
		bInterleaved = (getUnsignedLong(0x0028, 0x0, 0x0006, 0x0) == 0x0);
		b2complement = (getUnsignedLong(0x0028, 0, 0x0103, 0) != 0);
		allocatedBits = (imbxUint8)getUnsignedLong(0x0028, 0x0, 0x0100, 0x0);
		channelsNumber = getUnsignedLong(0x0028, 0x0, 0x0002, 0x0);
	}

	// Select the data type OB if not already set in the
	//  dataset
	///////////////////////////////////////////////////////////
	if(dataHandlerType.empty())
	{
		dataHandlerType = "OB";
	}

	// Get a stream to save the image
	///////////////////////////////////////////////////////////
	ptr<streamWriter> outputStream = getStreamWriter(groupId, orderId, tagId, firstBufferId, dataHandlerType);
	
	// Save the image in the stream
	///////////////////////////////////////////////////////////
	saveCodec->setImage(
		outputStream,
		pImage, 
		transferSyntax, 
		quality, 
		dataHandlerType,
		allocatedBits,
		bSubSampledX, bSubSampledY,
		bInterleaved,
		b2complement);

	// The images' positions calculated by getImage are not
	//  valid now. They must be recalculated.
	///////////////////////////////////////////////////////////
	m_imagesPositions.clear();

	// Write the attributes in the dataset
	///////////////////////////////////////////////////////////
	if(!bDontChangeAttributes)
	{
		ptr<handlers::dataHandler> dataHandlerTransferSyntax = getDataHandler(0x0002, 0x0, 0x0010, 0x0, true);
		dataHandlerTransferSyntax->setUnicodeString(transferSyntax);

		std::wstring colorSpace = pImage->getColorSpace();
		setUnicodeString(0x0028, 0x0, 0x0004, 0x0, transforms::colorTransforms::colorTransformsFactory::makeSubsampled(colorSpace, bSubSampledX, bSubSampledY));
		setUnsignedLong(0x0028, 0x0, 0x0006, 0x0, bInterleaved ? 0 : 1);
		setUnsignedLong(0x0028, 0x0, 0x0100, 0x0, allocatedBits);            // allocated bits
		setUnsignedLong(0x0028, 0x0, 0x0101, 0x0, pImage->getHighBit() + 1); // stored bits
		setUnsignedLong(0x0028, 0x0, 0x0102, 0x0, pImage->getHighBit());     // high bit
		setUnsignedLong(0x0028, 0x0, 0x0002, 0x0, channelsNumber);
		imbxUint32 imageSizeX, imageSizeY;
		pImage->getSize(&imageSizeX, &imageSizeY);
		setUnsignedLong(0x0028, 0x0, 0x0011, 0x0, imageSizeX);
		setUnsignedLong(0x0028, 0x0, 0x0010, 0x0, imageSizeY);

		double imageSizeMmX, imageSizeMmY;
		pImage->getSizeMm(&imageSizeMmX, &imageSizeMmY);

	}

	// Update the number of frames
	///////////////////////////////////////////////////////////
	if(frameNumber >= numberOfFrames)
	{
		numberOfFrames = frameNumber + 1;
		ptr<handlers::dataHandler> framesNumberHandler = getDataHandler(0x0028, 0, 0x0008, 0, true, "IS");
		framesNumberHandler->setSize(1);
		framesNumberHandler->setUnsignedLong(numberOfFrames);
	}

	// Update the offsets tag with the image's offsets
	///////////////////////////////////////////////////////////
	if(firstBufferId == 0)
	{
		return;
	}

	for(imbxUint32 resetBuffers = firstBufferId + 1; resetBuffers < endBufferId; ++resetBuffers)
	{
		ptr<handlers::dataHandlerRaw> resetBufferHandler = getDataHandlerRaw(groupId, 0L, tagId, resetBuffers, true);
		resetBufferHandler->setSize(0);
	}
	updateFrameBufferTable(&frameFirstBufferList, firstBufferId, endBufferId, outputStream->position());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
// Get the offset, in bytes, of the specified frame
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataSet::getFrameOffset(imbxUint32 frameNumber)
{
	// Retrieve the buffer containing the offsets
	///////////////////////////////////////////////////////////
	ptr<handlers::dataHandlerRaw> framesPointer = getDataHandlerRaw(0x7fe0, 0x0, 0x0010, 0, false);
	if(framesPointer == 0)
	{
		return 0xffffffff;
	}
	
	// Get the offset table's size, in number of offsets
	///////////////////////////////////////////////////////////
	imbxUint32 offsetsCount = framesPointer->getSize() / sizeof(imbxUint32);
	
	// If the requested frame doesn't exist then return 
	//  0xffffffff (the maximum value)
	///////////////////////////////////////////////////////////
	if(frameNumber >= offsetsCount && frameNumber != 0)
	{
		return 0xffffffff;
	}

	// Return the requested offset. If the requested frame is
	//  the first and is offset is not specified, then return
	//  0 (the first position)
	///////////////////////////////////////////////////////////
	imbxUint8* pOffsets = framesPointer->getMemoryBuffer();
	std::auto_ptr<imbxUint32> pAdjustedOffsets((imbxUint32*)new imbxUint8[framesPointer->getSize()]);
	::memcpy(pAdjustedOffsets.get(), pOffsets, framesPointer->getSize());
	streamController::adjustEndian((imbxUint8*)(pAdjustedOffsets.get()), 4, streamController::lowByteEndian, offsetsCount);
	if(frameNumber < offsetsCount)
	{
		return pAdjustedOffsets.get()[frameNumber];
	}
	return 0;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
// Return the buffer that starts at the specified offset
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataSet::getFrameBufferId(imbxUint32 offset, imbxUint32* pLengthToBuffer)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getFrameBufferId");

	*pLengthToBuffer = 0;

	ptr<data> imageTag = getTag(0x7fe0, 0, 0x0010, false);
	if(imageTag == 0)
	{
		return 0;
	}

	// Start from the buffer n. 1 (the buffer 0 contains
	//  the offset table
	///////////////////////////////////////////////////////////
	imbxUint32 scanBuffers(1);
	while(offset != 0)
	{
		// If the handler isn't connected to any buffer, then
		//  the buffer doesn't exist: return
		///////////////////////////////////////////////////////////
		if(!imageTag->bufferExists(scanBuffers))
		{
			break;
		}

		// Calculate the total size of the buffer, including
		//  its descriptor (tag group and id and length)
		///////////////////////////////////////////////////////////
		imbxUint32 bufferSize = imageTag->getBufferSize(scanBuffers);;
		(*pLengthToBuffer) += bufferSize; // Increase the total size
		bufferSize += 4; // one WORD for the group id, one WORD for the tag id
		bufferSize += 4; // one DWORD for the tag length
		if(bufferSize > offset)
		{
			PUNTOEXE_THROW(dataSetImageDoesntExist, "Image not in the offset table");
		}
		offset -= bufferSize;
		++scanBuffers;
	}

	return scanBuffers;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the first buffer and the end buffer occupied by an
//  image
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataSet::getFrameBufferIds(imbxUint32 frameNumber, imbxUint32* pFirstBuffer, imbxUint32* pEndBuffer)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getFrameBufferIds");

	imbxUint32 startOffset = getFrameOffset(frameNumber);
	imbxUint32 endOffset = getFrameOffset(frameNumber + 1);

	if(startOffset == 0xffffffff)
	{
		PUNTOEXE_THROW(dataSetImageDoesntExist, "Image not in the table offset");
	}

	imbxUint32 startLength, endLength;
	*pFirstBuffer = getFrameBufferId(startOffset, &startLength);
	*pEndBuffer = getFrameBufferId(endOffset, &endLength);

	return endLength - startLength;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the id of the first available buffer that can
//  be used to store a new frame
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataSet::getFirstAvailFrameBufferId()
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getFirstAvailFrameBufferId");

	ptr<data> imageTag = getTag(0x7fe0, 0, 0x0010, false);
	if(imageTag == 0)
	{
		return 1;
	}

	imbxUint32 availableId(1);
	while(imageTag->bufferExists(availableId))
	{
		++availableId;
	}

	return availableId;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Update the frame offset table
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::updateFrameBufferTable(
	tFrameFirstBufferList* pFrameFirstBufferList,
	imbxUint32 currentFrameFirstBufferId,
	imbxUint32 currentFrameEndBufferId,
	imbxUint32 currentFrameFirstBufferSize)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::updateFrameBufferTable");

	// The size must be a multiple of 2
	///////////////////////////////////////////////////////////
	if((currentFrameFirstBufferSize & 0x00000001) != 0)
	{
		++currentFrameFirstBufferSize;
	}

	// Retrieve the buffer containing the offsets
	///////////////////////////////////////////////////////////
	ptr<handlers::dataHandlerRaw> framesPointer = getDataHandlerRaw(0x7fe0, 0x0, 0x0010, 0, true);

	framesPointer->setSize((imbxUint32)(pFrameFirstBufferList->size()) * sizeof(imbxUint32));

	// Update all the offsets
	///////////////////////////////////////////////////////////
	imbxUint32 currentBuffer(1);
	imbxUint32 currentOffset(0);

	imbxUint32* pOffsets = (imbxUint32*)framesPointer->getMemoryBuffer();
	for(tFrameFirstBufferList::const_iterator scanFrames = pFrameFirstBufferList->begin(); scanFrames != pFrameFirstBufferList->end(); ++scanFrames)
	{
		while(currentBuffer != *scanFrames)
		{
			if(currentBuffer == currentFrameFirstBufferId)
			{
				currentOffset += currentFrameFirstBufferSize;
			}
			else
			{
				if(currentBuffer <= currentFrameFirstBufferId || currentBuffer >= currentFrameEndBufferId)
				{
					ptr<handlers::dataHandlerRaw> bufferHandler = getDataHandlerRaw(0x7fe0, 0x0, 0x0010, currentBuffer, false);
					currentOffset += bufferHandler->getSize();
				}
			}
			currentOffset += 4; // 1 word for the group id, 1 word for the tag id
			currentOffset += 4; // tag's length
			++currentBuffer;
		}
		*(pOffsets++) = currentOffset;
	}

	streamController::adjustEndian((imbxUint8*)pOffsets, sizeof(imbxUint32), streamController::lowByteEndian, (imbxUint32)(pFrameFirstBufferList->size()));

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Converts an image using the attributes specified in
//  the dataset.
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<image> dataSet::convertImageForDataSet(ptr<image> sourceImage)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::convertImageForDataSet");

	// Shared pointer used to reference this dataset
	///////////////////////////////////////////////////////////
	ptr<dataSet> thisDataSet(this);

	imbxUint32 imageWidth, imageHeight;
	sourceImage->getSize(&imageWidth, &imageHeight);

	std::wstring colorSpace = sourceImage->getColorSpace();
	imbxUint32 highBit = sourceImage->getHighBit();

	transforms::transformsChain chain;
	chain.declareDataSet(thisDataSet);
	chain.declareInputImage(0, sourceImage);

	imbxUint32 currentWidth  = getUnsignedLong(0x0028, 0x0, 0x0011, 0x0);
	imbxUint32 currentHeight = getUnsignedLong(0x0028, 0x0, 0x0010, 0x0);
	imbxUint32 currentHighBit = getUnsignedLong(0x0028, 0x0, 0x0102, 0x0);
	std::wstring currentColorSpace = transforms::colorTransforms::colorTransformsFactory::normalizeColorSpace(getUnicodeString(0x0028, 0x0, 0x0004, 0x0));

	if(currentWidth != imageWidth || currentHeight != imageHeight)
	{
		PUNTOEXE_THROW(dataSetExceptionDifferentFormat, "The dataset already contains an image with a different size");
	}

	if(currentHighBit < highBit)
	{
		PUNTOEXE_THROW(dataSetExceptionDifferentFormat, "The high bit in the dataset is smaller than the requested one");
	}

	if( !transforms::colorTransforms::colorTransformsFactory::isMonochrome(colorSpace) && colorSpace != currentColorSpace)
	{
		PUNTOEXE_THROW(dataSetExceptionDifferentFormat, "The requested color space doesn't match the one already stored in the dataset");
	}

	if(colorSpace != currentColorSpace)
	{
		ptr<transforms::colorTransforms::colorTransformsFactory> pColorFactory(transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory());
		ptr<transforms::transform> colorChain = pColorFactory->getTransform(colorSpace, currentColorSpace);
		if(colorChain->isEmpty())
		{
			PUNTOEXE_THROW(dataSetExceptionDifferentFormat, "The image color space cannot be converted to the dataset color space");
		}
		chain.addTransform(colorChain);
	}

	if(currentHighBit != highBit)
	{
		chain.addTransform(ptr<transforms::transform>(new transforms::transformHighBit));
	}

	if(chain.isEmpty())
	{
		return sourceImage;
	}

	chain.doTransform();

	return chain.getOutputImage(0);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve a sequence item as a dataset
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<dataSet> dataSet::getSequenceItem(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 bufferId)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getSequenceItem");

	ptr<data> tag=getTag(groupId, order, tagId, false);
	ptr<dataSet> pDataSet;
	if(tag != 0)
	{
		pDataSet = tag->getDataSet(bufferId);
	}

	return pDataSet;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve a LUT from the data set
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<lut> dataSet::getLut(imbxUint16 groupId, imbxUint16 tagId, imbxUint32 lutId)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getLut");

	lockObject lockAccess(this);

	ptr<lut> pLUT;
	ptr<dataSet> embeddedLUT=getSequenceItem(groupId, 0, tagId, lutId);
	std::string tagType = getDataType(groupId, 0, tagId);
	if(embeddedLUT != 0)
	{
		ptr<lut> tempLut(new lut);
		pLUT = tempLut;
		ptr<handlers::dataHandler> descriptorHandle=embeddedLUT->getDataHandler(0x0028, 0x0, 0x3002, 0x0, false);
		ptr<handlers::dataHandler> dataHandle=embeddedLUT->getDataHandler(0x0028, 0x0, 0x3006, 0x0, false);
		
		pLUT->setLut(
			descriptorHandle, 
			dataHandle, 
			embeddedLUT->getUnicodeString(0x0028, 0x0, 0x3003, 0x0));
	}
	return pLUT;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a tag as a signed long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 dataSet::getSignedLong(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getSignedLong");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0, false);
	if(dataHandler == 0)
	{
		return 0;
	}

	dataHandler->setPointer(elementNumber);
	return dataHandler->pointerIsValid() ? dataHandler->getSignedLong() : 0;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set a tag as a signed long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setSignedLong(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber, imbxInt32 newValue, std::string defaultType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setSignedLong");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0, true, defaultType);
	if(dataHandler != 0)
	{
		if(dataHandler->getSize() <= elementNumber)
		{
			dataHandler->setSize(elementNumber + 1);
		}
		dataHandler->setPointer(elementNumber);
		dataHandler->setSignedLong(newValue);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the requested tag as an unsigned long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataSet::getUnsignedLong(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getUnignedLong");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0, false);
	if(dataHandler == 0)
	{
		return 0;
	}
	
	dataHandler->setPointer(elementNumber);
	return dataHandler->pointerIsValid() ? dataHandler->getUnsignedLong() : 0;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the requested tag as an unsigned long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setUnsignedLong(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber, imbxUint32 newValue, std::string defaultType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setUnsignedLong");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0, true, defaultType);
	if(dataHandler != 0)
	{
		if(dataHandler->getSize() <= elementNumber)
		{
			dataHandler->setSize(elementNumber + 1);
		}
		dataHandler->setPointer(elementNumber);
		dataHandler->setUnsignedLong(newValue);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the requested tag as a double
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
double dataSet::getDouble(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getDouble");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0, false);
	if(dataHandler == 0)
	{
		return 0.0;
	}

	dataHandler->setPointer(elementNumber);
	return dataHandler->pointerIsValid() ? dataHandler->getDouble() : 0.0;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the requested tag as a double
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setDouble(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber, double newValue, std::string defaultType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setDouble");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0, true, defaultType);
	if(dataHandler != 0)
	{
		if(dataHandler->getSize() <= elementNumber)
		{
			dataHandler->setSize(elementNumber + 1);
		}
		dataHandler->setPointer(elementNumber);
		dataHandler->setDouble(newValue);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the requested tag as a string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataSet::getString(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getString");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0L, false);
	std::string returnValue;
	if(dataHandler != 0)
	{
		dataHandler->setPointer(elementNumber);
		if(dataHandler->pointerIsValid())
		{
			returnValue = dataHandler->getString();
		}
	}

	return returnValue;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the requested tag as an unicode string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring dataSet::getUnicodeString(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getUnicodeString");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0L, false);
	std::wstring returnValue;
	if(dataHandler != 0)
	{
		dataHandler->setPointer(elementNumber);
		if(dataHandler->pointerIsValid())
		{
			returnValue = dataHandler->getUnicodeString();
		}
	}

	return returnValue;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the requested tag as a string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setString(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber, std::string newString, std::string defaultType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setString");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0L, true, defaultType);
	if(dataHandler != 0)
	{
		if(dataHandler->getSize() <= elementNumber)
		{
			dataHandler->setSize(elementNumber + 1);
		}
		dataHandler->setPointer(elementNumber);
		dataHandler->setString(newString);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the requested tag as a string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::setUnicodeString(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 elementNumber, std::wstring newString, std::string defaultType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::setUnicodeString");

	ptr<handlers::dataHandler> dataHandler=getDataHandler(groupId, order, tagId, 0L, true, defaultType);
	if(dataHandler != 0)
	{
		if(dataHandler->getSize() <= elementNumber)
		{
			dataHandler->setSize(elementNumber + 1);
		}
		dataHandler->setPointer(elementNumber);
		dataHandler->setUnicodeString(newString);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a data handler for the requested tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<handlers::dataHandler> dataSet::getDataHandler(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 bufferId, bool bWrite, std::string defaultType /* ="" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getDataHandler");

	lockObject lockAccess(this);

	ptr<dataGroup>	group=getGroup(groupId, order, bWrite);

	ptr<handlers::dataHandler> pDataHandler;

	if(group == 0)
	{
		return pDataHandler;
	}

	if(defaultType.length()!=2L)
	{
		defaultType=getDefaultDataType(groupId, tagId);
	}

	pDataHandler = group->getDataHandler(tagId, bufferId, bWrite, defaultType);

	return pDataHandler;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a stream reader that works on the specified tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<streamReader> dataSet::getStreamReader(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 bufferId)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getStream");

	lockObject lockAccess(this);

	ptr<dataGroup>	group=getGroup(groupId, order, false);

	ptr<streamReader> returnStream;

	if(group != 0)
	{
		returnStream = group->getStreamReader(tagId, bufferId);
	}

	return returnStream;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve a stream writer for the specified tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<streamWriter> dataSet::getStreamWriter(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 bufferId, std::string dataType /* = "" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getStream");

	lockObject lockAccess(this);

	ptr<dataGroup>	group=getGroup(groupId, order, true);

	ptr<streamWriter> returnStream;

	if(group != 0)
	{
		returnStream = group->getStreamWriter(tagId, bufferId, dataType);
	}

	return returnStream;

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a raw data handler for the requested tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<handlers::dataHandlerRaw> dataSet::getDataHandlerRaw(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId, imbxUint32 bufferId, bool bWrite, std::string defaultType /* ="" */)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getDataHandlerRaw");

	lockObject lockAccess(this);

	ptr<dataGroup>	group=getGroup(groupId, order, bWrite);

	if(group == 0)
	{
		ptr<handlers::dataHandlerRaw> emptyDataHandler;
		return emptyDataHandler;
	}

	if(defaultType.length()!=2)
	{
		defaultType=getDefaultDataType(groupId, tagId);
	}

	return group->getDataHandlerRaw(tagId, bufferId, bWrite, defaultType);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the requested tag's default data type
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataSet::getDefaultDataType(imbxUint16 groupId, imbxUint16 tagId)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getDefaultDataType");

	return dicomDictionary::getDicomDictionary()->getTagType(groupId, tagId);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the data type of a tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataSet::getDataType(imbxUint16 groupId, imbxUint16 order, imbxUint16 tagId)
{
	PUNTOEXE_FUNCTION_START(L"dataSet::getDataType");

	std::string bufferType;

	ptr<data> tag = getTag(groupId, order, tagId, false);
	if(tag != 0)
	{
		bufferType = tag->getDataType();
	}
	return bufferType;

	PUNTOEXE_FUNCTION_END();
}

void dataSet::updateCharsetTag()
{
	tCharsetsList charsets;
	getCharsetsList(&charsets);
	ptr<handlers::dataHandler> charsetHandler(getDataHandler(0x0008, 0, 0x0005, 0, true));
	charsetHandler->setSize((imbxUint32)(charsets.size()));
	charsetHandler->setPointer(0);
	for(tCharsetsList::iterator scanCharsets = charsets.begin(); scanCharsets != charsets.end(); ++scanCharsets)
	{
		charsetHandler->setUnicodeString(*scanCharsets);
		charsetHandler->incPointer();
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Update the list of the used charsets
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataSet::updateTagsCharset()
{
	tCharsetsList charsets;
	ptr<handlers::dataHandler> charsetHandler(getDataHandler(0x0008, 0, 0x0005, 0, false));
	if(charsetHandler != 0)
	{
		for(charsetHandler->setPointer(0); charsetHandler->pointerIsValid(); charsetHandler->incPointer())
		{
			charsets.push_back(charsetHandler->getUnicodeString());
		}
	}
	setCharsetsList(&charsets);
}


} // namespace imebra

} // namespace puntoexe
