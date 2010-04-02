#include <cppunit/extensions/HelperMacros.h>
#include "bitTransformTest.h"

#include "../../imebra/include/imebra.h"

namespace puntoexe
{

namespace imebra
{

namespace tests
{

CPPUNIT_TEST_SUITE_REGISTRATION(puntoexe::imebra::tests::bitTransformTest);


// A buffer initialized to a default data type should use the data type OB
void bitTransformTest::testBitTransform()
{
	imbxUint32 sizeX = 41;
	imbxUint32 sizeY = 13;
	ptr<image> bits8Image(new image);
	ptr<image> bits16Image(new image);
	ptr<image> bits4Image(new image);
	bits8Image->create(sizeX, sizeY, image::depthU8, L"RGB", 7);
	bits16Image->create(sizeX, sizeY, image::depthU16, L"RGB", 15);
	bits4Image->create(sizeX, sizeY, image::depthU8, L"RGB", 3);

	imbxUint32 rowSize, channelsPixelSize, channelsNumber;
	ptr<handlers::dataHandlerNumericBase> imageHandler = bits8Image->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);

	// Make 3 bands (RGB
	imbxUint32 elementNumber(0);
	for(imbxUint32 y=0; y<sizeY; ++y)
	{
		for(imbxUint32 x=0; x<sizeX; ++x)
		{
			imbxInt32 r, g, b;
			imbxUint32 value = y * 255 / sizeY;
			r = g = 0;
			b = value;
			if(x < sizeX - sizeX/3)
			{
				r = 0;
				g = value;
				b = 0;
			}
			if(x < sizeX / 3)
			{
				r = value;
				g = 0;
				b = 0;
			}
			imageHandler->setUnsignedLong(elementNumber++, r);
			imageHandler->setUnsignedLong(elementNumber++, g);
			imageHandler->setUnsignedLong(elementNumber++, b);
		}
	}
	imageHandler.release();

	ptr<transforms::transformHighBit> highBit(new transforms::transformHighBit);
	highBit->declareInputImage(0, bits8Image);
	highBit->declareOutputImage(0, bits16Image);
	highBit->doTransform();

	highBit->declareInputImage(0, bits8Image);
	highBit->declareOutputImage(0, bits4Image);
	highBit->doTransform();

	ptr<handlers::dataHandlerNumericBase> bits8Handler = bits8Image->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);
	ptr<handlers::dataHandlerNumericBase> bits16Handler = bits16Image->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);
	ptr<handlers::dataHandlerNumericBase> bits4Handler = bits4Image->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);
	elementNumber = 0;
	for(imbxUint32 checkY = 0; checkY < sizeY; ++checkY)
	{
		for(imbxUint32 checkX = 0; checkX < sizeX; ++checkX)
		{
			imbxInt32 r, g, b;
			imbxUint32 value = checkY * 255 / sizeY;
			r = g = 0;
			b = value;
			if(checkX < sizeX - sizeX/3)
			{
				r = 0;
				g = value;
				b = 0;
			}
			if(checkX < sizeX / 3)
			{
				r = value;
				g = 0;
				b = 0;
			}

			imbxInt32 value0r = bits8Handler->getUnsignedLong(elementNumber);
			imbxInt32 value1r = bits16Handler->getUnsignedLong(elementNumber);
			imbxInt32 value2r = bits4Handler->getUnsignedLong(elementNumber++);
			
			imbxInt32 value0g = bits8Handler->getUnsignedLong(elementNumber);
			imbxInt32 value1g = bits16Handler->getUnsignedLong(elementNumber);
			imbxInt32 value2g = bits4Handler->getUnsignedLong(elementNumber++);
			
			imbxInt32 value0b = bits8Handler->getUnsignedLong(elementNumber);
			imbxInt32 value1b = bits16Handler->getUnsignedLong(elementNumber);
			imbxInt32 value2b = bits4Handler->getUnsignedLong(elementNumber++);
			
			CPPUNIT_ASSERT(value0r == r);
			CPPUNIT_ASSERT(value0g == g);
			CPPUNIT_ASSERT(value0b == b);

			CPPUNIT_ASSERT(value0r == (value1r>>8));
			CPPUNIT_ASSERT(value0g == (value1g>>8));
			CPPUNIT_ASSERT(value0b == (value1b>>8));

			CPPUNIT_ASSERT((value0r >> 4) == value2r);
			CPPUNIT_ASSERT((value0g >> 4) == value2g);
			CPPUNIT_ASSERT((value0b >> 4) == value2b);

		}
	}
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe