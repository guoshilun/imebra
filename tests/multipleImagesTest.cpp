#include <imebra/imebra.h>
#include "buildImageForTest.h"
#include <gtest/gtest.h>

namespace imebra
{

namespace tests
{

TEST(multipleImagesTest, testLosslessImages)
{
    const size_t numImages(64);

    for(int transferSyntaxId(0); transferSyntaxId != 6; ++transferSyntaxId)
    {
        const std::uint32_t highBit(7);

        const std::string colorSpace("MONOCHROME2");

        const bitDepth_t depth(bitDepth_t::depthU8);

        std::unique_ptr<Image> dicomImage;

        dicomImage.reset(buildImageForTest(
                1024,
                1024,
                depth,
                highBit,
                1024,
                1024,
                colorSpace,
                1));

        std::string transferSyntax;
        switch(transferSyntaxId)
        {
        case 0:
            transferSyntax = "1.2.840.10008.1.2.4.57";
            break;
        case 1:
            transferSyntax = "1.2.840.10008.1.2.4.70";
            break;
        case 2:
            transferSyntax = "1.2.840.10008.1.2";
            break;
        case 3:
            transferSyntax = "1.2.840.10008.1.2.1";
            break;
        case 4:
            transferSyntax = "1.2.840.10008.1.2.2";
            break;
        case 5:
            transferSyntax = "1.2.840.10008.1.2.5";
            break;
        }

        std::cout << "Multiple images dicom test. Transfer syntax: " << transferSyntax << std::endl;

        const imageQuality_t quality = imageQuality_t::veryHigh;

        ReadWriteMemory streamMemory;
        {
            DataSet testDataSet(transferSyntax);
            testDataSet.setDouble(TagId(tagId_t::TimeRange_0008_1163), 50.6);
            if(ColorTransformsFactory::getNumberOfChannels(colorSpace) > 1)
            {
                testDataSet.setUnsignedLong(TagId(imebra::tagId_t::PlanarConfiguration_0028_0006), 1);
            }

            for(size_t imageNumber(0); imageNumber != numImages; ++imageNumber)
            {
                std::cout << "Adding image: " << imageNumber << std::endl;
                testDataSet.setImage(imageNumber, *dicomImage, quality);
            }

            MemoryStreamOutput writeStream(streamMemory);
            StreamWriter writer(writeStream);
            CodecFactory::save(testDataSet, writer, codecType_t::dicom);
        }

        for(unsigned int lazyLoad(0); lazyLoad != 2; ++lazyLoad)
        {
            MemoryStreamInput readStream(streamMemory);
            StreamReader reader(readStream);
            std::unique_ptr<DataSet> testDataSet(CodecFactory::load(reader, lazyLoad == 0 ? std::numeric_limits<size_t>::max() : 1));

            for(size_t imageNumber(0); imageNumber != numImages; ++imageNumber)
            {
                std::unique_ptr<Image> checkImage(testDataSet->getImage(imageNumber));
                ASSERT_TRUE(identicalImages(*checkImage, *dicomImage));
            }
        }
    } // transferSyntaxId
}

}

}
