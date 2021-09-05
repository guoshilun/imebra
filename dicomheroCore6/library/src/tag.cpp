/*
Copyright 2005 - 2021 by Paolo Brandoli/Binarno s.p.

DICOMHero/Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for DICOMHero/Imebra from the DICOMHero’s website (https://dicomhero.com).
*/

#include "../implementation/dataImpl.h"
#include "../implementation/dataHandlerNumericImpl.h"
#include "../implementation/fileStreamImpl.h"

#include "../include/dicomhero6/tag.h"
#include "../include/dicomhero6/readingDataHandlerNumeric.h"
#include "../include/dicomhero6/writingDataHandlerNumeric.h"
#include "../include/dicomhero6/definitions.h"
#include "../include/dicomhero6/streamReader.h"
#include "../include/dicomhero6/streamWriter.h"
#include "../include/dicomhero6/dataSet.h"
#include "../include/dicomhero6/baseStreamInput.h"
#include "../include/dicomhero6/fileStreamInput.h"

namespace dicomhero
{

Tag::Tag(const Tag& source): m_pData(getTagImplementation(source))
{
}

const std::shared_ptr<dicomhero::implementation::data>& getTagImplementation(const Tag& tag)
{
    return tag.m_pData;
}

Tag::~Tag()
{
}

Tag::Tag(const std::shared_ptr<dicomhero::implementation::data>& pData): m_pData(pData)
{
}

size_t Tag::getBuffersCount() const
{
    DICOMHERO_FUNCTION_START();

    return m_pData->getBuffersCount();

    DICOMHERO_FUNCTION_END_LOG();
}

bool Tag::bufferExists(size_t bufferId) const
{
    DICOMHERO_FUNCTION_START();

    return m_pData->bufferExists(bufferId);

    DICOMHERO_FUNCTION_END_LOG();
}

size_t Tag::getBufferSize(size_t bufferId) const
{
    DICOMHERO_FUNCTION_START();

    return m_pData->getBufferSize(bufferId);

    DICOMHERO_FUNCTION_END_LOG();
}

ReadingDataHandler Tag::getReadingDataHandler(size_t bufferId) const
{
    DICOMHERO_FUNCTION_START();

    return ReadingDataHandler(m_pData->getReadingDataHandler(bufferId));

    DICOMHERO_FUNCTION_END_LOG();
}

ReadingDataHandlerNumeric Tag::getReadingDataHandlerNumeric(size_t bufferId) const
{
    DICOMHERO_FUNCTION_START();

    std::shared_ptr<implementation::handlers::readingDataHandlerNumericBase> numericHandler =
            std::dynamic_pointer_cast<implementation::handlers::readingDataHandlerNumericBase>(m_pData->getReadingDataHandler(bufferId));
    if(numericHandler.get() == 0)
    {
        throw std::bad_cast();
    }
    return ReadingDataHandlerNumeric(numericHandler);

    DICOMHERO_FUNCTION_END_LOG();
}

ReadingDataHandlerNumeric Tag::getReadingDataHandlerRaw(size_t bufferId) const
{
    DICOMHERO_FUNCTION_START();

    std::shared_ptr<implementation::handlers::readingDataHandlerNumericBase> numericHandler = m_pData->getReadingDataHandlerRaw(bufferId);
    return ReadingDataHandlerNumeric(numericHandler);

    DICOMHERO_FUNCTION_END_LOG();
}

StreamReader Tag::getStreamReader(size_t bufferId) const
{
    DICOMHERO_FUNCTION_START();

    return StreamReader(m_pData->getStreamReader(bufferId));

    DICOMHERO_FUNCTION_END_LOG();
}

const DataSet Tag::getSequenceItem(size_t dataSetId) const
{
    DICOMHERO_FUNCTION_START();

    return DataSet(m_pData->getSequenceItem(dataSetId));

    DICOMHERO_FUNCTION_END_LOG();
}

bool Tag::sequenceItemExists(size_t dataSetId) const
{
    DICOMHERO_FUNCTION_START();

    return m_pData->dataSetExists(dataSetId);

    DICOMHERO_FUNCTION_END_LOG();
}

tagVR_t Tag::getDataType() const
{
    DICOMHERO_FUNCTION_START();

    return m_pData->getDataType();

    DICOMHERO_FUNCTION_END_LOG();
}

MutableTag::MutableTag(const std::shared_ptr<dicomhero::implementation::data>& pData): Tag(pData)
{
}

MutableTag::MutableTag(const MutableTag& source): Tag(source)
{
}

MutableTag::~MutableTag()
{
}

WritingDataHandler MutableTag::getWritingDataHandler(size_t bufferId)
{
    DICOMHERO_FUNCTION_START();

    return WritingDataHandler(getTagImplementation(*this)->getWritingDataHandler(bufferId));

    DICOMHERO_FUNCTION_END_LOG();
}

WritingDataHandlerNumeric MutableTag::getWritingDataHandlerNumeric(size_t bufferId)
{
    DICOMHERO_FUNCTION_START();

    std::shared_ptr<implementation::handlers::writingDataHandlerNumericBase> numericHandler =
            std::dynamic_pointer_cast<implementation::handlers::writingDataHandlerNumericBase>(getTagImplementation(*this)->getWritingDataHandler(bufferId));
    if(numericHandler.get() == 0)
    {
        throw std::bad_cast();
    }
    return WritingDataHandlerNumeric(numericHandler);

    DICOMHERO_FUNCTION_END_LOG();
}

WritingDataHandlerNumeric MutableTag::getWritingDataHandlerRaw(size_t bufferId)
{
    DICOMHERO_FUNCTION_START();

    std::shared_ptr<implementation::handlers::writingDataHandlerNumericBase> numericHandler = getTagImplementation(*this)->getWritingDataHandlerRaw(bufferId);
    return WritingDataHandlerNumeric(numericHandler);

    DICOMHERO_FUNCTION_END_LOG();
}

StreamWriter MutableTag::getStreamWriter(size_t bufferId)
{
    DICOMHERO_FUNCTION_START();

    return StreamWriter(getTagImplementation(*this)->getStreamWriter(bufferId));

    DICOMHERO_FUNCTION_END_LOG();
}

MutableDataSet MutableTag::appendSequenceItem()
{
    DICOMHERO_FUNCTION_START();

    return MutableDataSet(getTagImplementation(*this)->appendSequenceItem());

    DICOMHERO_FUNCTION_END_LOG();
}

void MutableTag::setStream(size_t bufferId, FileStreamInput& streamInput)
{
    DICOMHERO_FUNCTION_START();

    getTagImplementation(*this)->setExternalStream(bufferId, std::dynamic_pointer_cast<implementation::fileStreamInput>(getBaseStreamInputImplementation(streamInput)));

    DICOMHERO_FUNCTION_END_LOG();
}

}
