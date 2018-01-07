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
#import "imebra_nserror.h"
#import "imebra_strings.h"
#import "../include/imebra/objectivec/imebra_tagId.h"
#import "../include/imebra/objectivec/imebra_dataset.h"
#import "../include/imebra/objectivec/imebra_dateAge.h"
#import "../include/imebra/objectivec/imebra_image.h"
#import "../include/imebra/objectivec/imebra_readingDataHandler.h"
#import "../include/imebra/objectivec/imebra_writingDataHandler.h"
#import <Foundation/NSString.h>

@implementation ImebraDataSet

-(id)initWithImebraDataSet:(imebra::DataSet*)pDataSet
{
    self = [super init];
    if(self)
    {
        m_pDataSet = pDataSet;
    }
    else
    {
        delete pDataSet;
    }
    return self;
}

-(id)init
{
    self = [super init];
    if(self)
    {
        m_pDataSet = new imebra::DataSet();
    }
    return self;
}

-(id)initWithTransferSyntax:(NSString*)transferSyntax
{
    self = [super init];
    if(self)
    {
        m_pDataSet = new imebra::DataSet(imebra::NSStringToString(transferSyntax));
    }
    return self;
}

-(void)dealloc
{
    delete m_pDataSet;
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

-(ImebraImage*) getImage:(unsigned int) frameNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return [[ImebraImage alloc] initWithImebraImage:m_pDataSet->getImage(frameNumber)];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(ImebraImage*) getImageApplyModalityTransform:(unsigned int) frameNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return [[ImebraImage alloc] initWithImebraImage:m_pDataSet->getImageApplyModalityTransform(frameNumber)];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(void) setImage:(unsigned int)frameNumber image:(ImebraImage*)image quality:(ImebraImageQuality_t)quality error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setImage(frameNumber, *(image->m_pImage), (imebra::imageQuality_t)quality);

    OBJC_IMEBRA_FUNCTION_END();
}


-(ImebraReadingDataHandler*) getReadingDataHandler:(ImebraTagId*)tagId bufferId:(unsigned int)bufferId error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return [[ImebraReadingDataHandler alloc] initWithImebraReadingDataHandler:m_pDataSet->getReadingDataHandler(*(tagId->m_pTagId), bufferId)];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(ImebraWritingDataHandler*) getWritingDataHandler:(ImebraTagId*)tagId bufferId:(unsigned int)bufferId tagVR:(ImebraTagVR_t)tagVR error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return [[ImebraWritingDataHandler alloc] initWithImebraWritingDataHandler:m_pDataSet->getWritingDataHandler(*(tagId->m_pTagId), bufferId, (imebra::tagVR_t)tagVR)];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(ImebraWritingDataHandler*) getWritingDataHandler:(ImebraTagId*)tagId bufferId:(unsigned int)bufferId error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return [[ImebraWritingDataHandler alloc] initWithImebraWritingDataHandler:m_pDataSet->getWritingDataHandler(*(tagId->m_pTagId), bufferId)];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(signed int)getSignedLong:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return m_pDataSet->getSignedLong(*(tagId->m_pTagId), elementNumber);

    OBJC_IMEBRA_FUNCTION_END_RETURN(0);
}

-(signed int)getSignedLong:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber defaultValue:(signed int)defaultValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return m_pDataSet->getSignedLong(*(tagId->m_pTagId), elementNumber, defaultValue);

    OBJC_IMEBRA_FUNCTION_END_RETURN(0);
}

-(void)setSignedLong:(ImebraTagId*)tagId newValue:(signed int)newValue tagVR:(ImebraTagVR_t)tagVR error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setSignedLong(*(tagId->m_pTagId), newValue, (imebra::tagVR_t)tagVR);

    OBJC_IMEBRA_FUNCTION_END();
}

-(void)setSignedLong:(ImebraTagId*)tagId newValue:(signed int)newValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setSignedLong(*(tagId->m_pTagId), newValue);

    OBJC_IMEBRA_FUNCTION_END();
}


-(unsigned int)getUnsignedLong:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return m_pDataSet->getUnsignedLong(*(tagId->m_pTagId), elementNumber);

    OBJC_IMEBRA_FUNCTION_END_RETURN(0);
}

-(unsigned int)getUnsignedLong:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber defaultValue:(unsigned int)defaultValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return m_pDataSet->getUnsignedLong(*(tagId->m_pTagId), elementNumber, defaultValue);

    OBJC_IMEBRA_FUNCTION_END_RETURN(0);
}

-(void)setUnsignedLong:(ImebraTagId*)tagId newValue:(unsigned int)newValue tagVR:(ImebraTagVR_t)tagVR error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setUnsignedLong(*(tagId->m_pTagId), newValue, (imebra::tagVR_t)tagVR);

    OBJC_IMEBRA_FUNCTION_END();
}

-(void)setUnsignedLong:(ImebraTagId*)tagId newValue:(unsigned int)newValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setUnsignedLong(*(tagId->m_pTagId), newValue);

    OBJC_IMEBRA_FUNCTION_END();
}


-(double)getDouble:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return m_pDataSet->getDouble(*(tagId->m_pTagId), elementNumber);

    OBJC_IMEBRA_FUNCTION_END_RETURN(0.0f);
}

-(double)getDouble:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber defaultValue:(double)defaultValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return m_pDataSet->getDouble(*(tagId->m_pTagId), elementNumber, defaultValue);

    OBJC_IMEBRA_FUNCTION_END_RETURN(0.0f);
}

-(void)setDouble:(ImebraTagId*)tagId newValue:(double)newValue tagVR:(ImebraTagVR_t)tagVR error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setDouble(*(tagId->m_pTagId), newValue, (imebra::tagVR_t)tagVR);

    OBJC_IMEBRA_FUNCTION_END();
}

-(void)setDouble:(ImebraTagId*)tagId newValue:(double)newValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setDouble(*(tagId->m_pTagId), newValue);

    OBJC_IMEBRA_FUNCTION_END();
}


-(NSString*)getString:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return imebra::stringToNSString(m_pDataSet->getString(*(tagId->m_pTagId), elementNumber));

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(NSString*)getString:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber defaultValue:(NSString*)defaultValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return imebra::stringToNSString(m_pDataSet->getString(*(tagId->m_pTagId), elementNumber, imebra::NSStringToString(defaultValue)));

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(void)setString:(ImebraTagId*)tagId newValue:(NSString*)newValue tagVR:(ImebraTagVR_t)tagVR error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setString(*(tagId->m_pTagId), imebra::NSStringToString(newValue), (imebra::tagVR_t)tagVR);

    OBJC_IMEBRA_FUNCTION_END();
}

-(void)setString:(ImebraTagId*)tagId newValue:(NSString*)newValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setString(*(tagId->m_pTagId), imebra::NSStringToString(newValue));

    OBJC_IMEBRA_FUNCTION_END();
}


-(ImebraAge*)getAge:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    std::unique_ptr<imebra::Age> pAge(m_pDataSet->getAge(*(tagId->m_pTagId), elementNumber));
    return [[ImebraAge alloc] initWithAge:pAge->age units:(ImebraAgeUnit_t)pAge->units];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(ImebraAge*)getAge:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber defaultValue:(ImebraAge*)defaultValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    std::unique_ptr<imebra::Age> pAge(m_pDataSet->getAge(*(tagId->m_pTagId), elementNumber, *(imebra::Age*)(defaultValue->m_pAge)));
    return [[ImebraAge alloc] initWithAge:pAge->age units:(ImebraAgeUnit_t)pAge->units];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(void)setAge:(ImebraTagId*)tagId newValue:(ImebraAge*)newValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setAge(*(tagId->m_pTagId), *(imebra::Age*)(newValue->m_pAge));

    OBJC_IMEBRA_FUNCTION_END();
}


-(ImebraDate*)getDate:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    std::unique_ptr<imebra::Date> pDate(m_pDataSet->getDate(*(tagId->m_pTagId), elementNumber));
    return [[ImebraDate alloc] initWithDate:pDate->year
            month:pDate->month
            day:pDate->day
            hour:pDate->hour
            minutes:pDate->minutes
            seconds:pDate->seconds
            nanoseconds:pDate->nanoseconds
            offsetHours:pDate->offsetHours
            offsetMinutes:pDate->offsetMinutes];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(ImebraDate*)getDate:(ImebraTagId*)tagId elementNumber:(unsigned int)elementNumber defaultValue:(ImebraDate*)defaultValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    std::unique_ptr<imebra::Date> pDate(m_pDataSet->getDate(*(tagId->m_pTagId), elementNumber, *(imebra::Date*)(defaultValue->m_pDate)));
    return [[ImebraDate alloc] initWithDate:pDate->year
            month:pDate->month
            day:pDate->day
            hour:pDate->hour
            minutes:pDate->minutes
            seconds:pDate->seconds
            nanoseconds:pDate->nanoseconds
            offsetHours:pDate->offsetHours
            offsetMinutes:pDate->offsetMinutes];

    OBJC_IMEBRA_FUNCTION_END_RETURN(nil);
}

-(void)setDate:(ImebraTagId*)tagId newValue:(ImebraDate*)newValue tagVR:(ImebraTagVR_t)tagVR error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setDate(*(tagId->m_pTagId), *(imebra::Date*)(newValue->m_pDate), (imebra::tagVR_t)tagVR);

    OBJC_IMEBRA_FUNCTION_END();
}

-(void)setDate:(ImebraTagId*)tagId newValue:(ImebraDate*)newValue error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    m_pDataSet->setDate(*(tagId->m_pTagId), *(imebra::Date*)(newValue->m_pDate));

    OBJC_IMEBRA_FUNCTION_END();
}

-(ImebraTagVR_t)getDataType:(ImebraTagId*)tagId error:(NSError**)pError
{
    OBJC_IMEBRA_FUNCTION_START();

    return (ImebraTagVR_t)(m_pDataSet->getDataType(*(tagId->m_pTagId)));

    OBJC_IMEBRA_FUNCTION_END_RETURN(ImebraTagVR_t::ImebraAE);
}

@end




