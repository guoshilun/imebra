#include <imebra/imebra.h>
#include "buildImageForTest.h"
#include <gtest/gtest.h>
#include <limits>

namespace imebra
{

namespace tests
{

// A buffer initialized to a default data type should use the data type OB
TEST(dicomCodecTest, testDicom)
{
    char* colorSpaces[] = {"RGB", "YBR_FULL", "YBR_FULL_422", "YBR_FULL_420", "MONOCHROME2"};

    for(int transferSyntaxId(0); transferSyntaxId != 4; ++transferSyntaxId)
	{
        for(int interleaved(0); interleaved != 2; ++interleaved)
		{
			for(unsigned int sign=0; sign != 2; ++sign)
			{
                for(std::uint32_t highBit(0); highBit != 32; ++highBit)
				{
                    for(unsigned int colorSpaceIndex(0); colorSpaceIndex != sizeof(colorSpaces)/sizeof(colorSpaces[0]); ++colorSpaceIndex)
                    {
                        std::string colorSpace(colorSpaces[colorSpaceIndex]);
                        if((highBit > 24 || transferSyntaxId == 3 || interleaved == 0) &&
                                (ColorTransformsFactory::isSubsampledX(colorSpace) || ColorTransformsFactory::isSubsampledX(colorSpace)))
                        {
                            continue;
                        }

                        bitDepth_t depth(sign == 0 ? bitDepth_t::depthU8 : bitDepth_t::depthS8);
                        if(highBit > 7)
                        {
                            depth = (sign == 0 ? bitDepth_t::depthU16 : bitDepth_t::depthS16);
                        }
                        if(highBit > 15)
                        {
                            depth = (sign == 0 ? bitDepth_t::depthU32 : bitDepth_t::depthS32);
                        }

                        std::unique_ptr<Image> dicomImage0, dicomImage1, dicomImage2;

                        if(ColorTransformsFactory::isSubsampledY(colorSpace) || ColorTransformsFactory::isSubsampledX(colorSpace))
                        {
                            dicomImage0.reset(buildSubsampledImage(
                                    301,
                                    201,
                                    depth,
                                    highBit,
                                    30,
                                    20,
                                    colorSpace));
                            dicomImage1.reset(buildSubsampledImage(
                                    301,
                                    201,
                                    depth,
                                    highBit,
                                    30,
                                    20,
                                    colorSpace));
                            dicomImage2.reset(buildSubsampledImage(
                                    301,
                                    201,
                                    depth,
                                    highBit,
                                    30,
                                    20,
                                    colorSpace));
                        }
                        else
                        {
                            dicomImage0.reset(buildImageForTest(
                                    301,
                                    201,
                                    depth,
                                    highBit,
                                    30,
                                    20,
                                    colorSpace,
                                    1));
                            dicomImage1.reset(buildImageForTest(
                                    301,
                                    201,
                                    depth,
                                    highBit,
                                    30,
                                    20,
                                    colorSpace,
                                    100));
                            dicomImage2.reset(buildImageForTest(
                                    301,
                                    201,
                                    depth,
                                    highBit,
                                    30,
                                    20,
                                    colorSpace,
                                    150));

                        }

                        std::string transferSyntax;

                        switch(transferSyntaxId)
                        {
                        case 0:
                            transferSyntax = "1.2.840.10008.1.2";
                            break;
                        case 1:
                            transferSyntax = "1.2.840.10008.1.2.1";
                            break;
                        case 2:
                            transferSyntax = "1.2.840.10008.1.2.2";
                            break;
                        case 3:
                            transferSyntax = "1.2.840.10008.1.2.5";
                            break;
                        }

                        std::cout << "Dicom test. Transfer syntax: " << transferSyntax;
                        std::cout << " interleaved: " << interleaved;
                        std::cout << " sign: " << sign;
                        std::cout << " highBit: " << highBit << std::endl;
                        std::cout << " colorSpace: " << colorSpace << std::endl;

                        imageQuality_t quality = imageQuality_t::veryHigh;
                        if(ColorTransformsFactory::isSubsampledY(colorSpace))
                        {
                            quality = imageQuality_t::belowMedium;
                        }
                        else if(ColorTransformsFactory::isSubsampledX(colorSpace))
                        {
                            quality = imageQuality_t::medium;
                        }

                        ReadWriteMemory streamMemory;
                        {
                            DataSet testDataSet;
                            std::unique_ptr<WritingDataHandler> writingDataHandler(testDataSet.getWritingDataHandler(TagId(0x0010, 0x0010), 0));
                            writingDataHandler->setString(0, "AAAaa");
                            writingDataHandler->setString(1, "BBBbbb");
                            writingDataHandler->setString(2, "");
                            writingDataHandler.reset();
                            testDataSet.setDouble(TagId(tagId_t::TimeRange_0008_1163), 50.6);
                            testDataSet.setUnsignedLong(TagId(imebra::tagId_t::PlanarConfiguration_0028_0006), 1 - interleaved);
                            testDataSet.setImage(0, *dicomImage0, transferSyntax, quality);
                            testDataSet.setImage(1, *dicomImage1, transferSyntax, quality);
                            testDataSet.setImage(2, *dicomImage2, transferSyntax, quality);

                            MemoryStreamOutput writeStream(streamMemory);
                            StreamWriter writer(writeStream);
                            CodecFactory::save(testDataSet, writer, codecType_t::dicom);
                        }

                        for(unsigned int lazyLoad(0); lazyLoad != 2; ++lazyLoad)
                        {
                            MemoryStreamInput readStream(streamMemory);
                            StreamReader reader(readStream);
                            std::unique_ptr<DataSet> testDataSet(CodecFactory::load(reader, lazyLoad == 0 ? std::numeric_limits<size_t>::max() : 1));

                            EXPECT_EQ(std::string("AAAaa"), testDataSet->getString(TagId(imebra::tagId_t::PatientName_0010_0010), 0));
                            EXPECT_EQ(std::string("BBBbbb"), testDataSet->getString(TagId(imebra::tagId_t::PatientName_0010_0010), 1));
                            EXPECT_EQ(std::string(""), testDataSet->getString(TagId(imebra::tagId_t::PatientName_0010_0010), 2));
                            EXPECT_FLOAT_EQ(50.6, testDataSet->getDouble(TagId(tagId_t::TimeRange_0008_1163), 0));
                            EXPECT_EQ(1 - interleaved, testDataSet->getSignedLong(TagId(imebra::tagId_t::PlanarConfiguration_0028_0006), 0));

                            std::unique_ptr<Image> checkImage0(testDataSet->getImage(0));
                            std::unique_ptr<Image> checkImage1(testDataSet->getImage(1));
                            std::unique_ptr<Image> checkImage2(testDataSet->getImage(2));

                            std::cout << " DIFF: " << compareImages(*checkImage0, *dicomImage0) << std::endl;
                            std::cout << " DIFF: " << compareImages(*checkImage1, *dicomImage1) << std::endl;
                            std::cout << " DIFF: " << compareImages(*checkImage2, *dicomImage2) << std::endl;

                            double maxDiff(0);
                            if(quality == imageQuality_t::medium)
                            {
                                maxDiff = 10;
                            }
                            else if(quality == imageQuality_t::belowMedium)
                            {
                                maxDiff = 100;
                            }
                            ASSERT_EQ(0, compareImages(*checkImage0, *dicomImage0));
                            ASSERT_EQ(0, compareImages(*checkImage1, *dicomImage1));
                            ASSERT_EQ(0, compareImages(*checkImage2, *dicomImage2));
                        }
                    }
				}
			}
		}
	} // transferSyntaxId
}


} // namespace tests

} // namespace imebra

