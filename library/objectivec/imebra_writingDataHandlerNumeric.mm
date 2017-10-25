/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

#include "../include/imebra/imebra.h"
#import "../include/imebra/objectivec/imebra_nserror.h"
#import "../include/imebra/objectivec/imebra_writingDataHandlerNumeric.h"
#import "../include/imebra/objectivec/imebra_readingDataHandlerNumeric.h"
#import "../include/imebra/objectivec/imebra_readWriteMemory.h"

@implementation ImebraWritingDataHandlerNumeric

-(id)initWithImebraWritingDataHandler:(imebra::WritingDataHandlerNumeric*)pWritingDataHandler
{
    self = [super initWithImebraWritingDataHandler:pWritingDataHandler];
    return self;
}

-(void)dealloc
{
}

-(ImebraReadWriteMemory*)getMemory:(NSError**)pError
{
    try
    {
        return [[ImebraReadWriteMemory alloc] initWithImebraReadWriteMemory:((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->getMemory()];
    }
    catch(const std::runtime_error& e)
    {
        imebra::setNSError(e, pError);
        return nil;
    }
}

-(void)assign:(const char*)data withSize:(unsigned int)size error:(NSError**)pError
{
    try
    {
        ((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->assign((char*)data, size);
    }
    catch(const std::runtime_error& e)
    {
        imebra::setNSError(e, pError);
    }
}

-(void)copyFrom:(ImebraReadingDataHandlerNumeric*)source error:(NSError**)pError
{
    try
    {
        ((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->copyFrom(*((imebra::ReadingDataHandlerNumeric*)source->m_pDataHandler));
    }
    catch(const std::runtime_error& e)
    {
        imebra::setNSError(e, pError);
    }
}

-(char*) data
{
    size_t dummy;
    return ((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->data(&dummy);
}

-(unsigned int) dataSizeBytes
{
    size_t size;
    ((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->data(&size);
    return (unsigned int)size;
}

-(unsigned int) unitSize
{
    return (unsigned int)((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->getUnitSize();
}

-(bool) isSigned
{
    return ((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->isSigned();
}

-(bool) isFloat
{
    return ((imebra::WritingDataHandlerNumeric*)m_pDataHandler)->isFloat();
}

@end
