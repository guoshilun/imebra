#include "../../library/implementation/imebraImpl.h"
#include "testsSettings.h"
#include <dirent.h>
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{


// A buffer initialized to a default data type should use the data type OB
TEST(corruptedFilesTest, corruptedFilesTest)
{
    ::tests::settings& settings = ::tests::settings::getSettings();

    std::string folder = settings.get("--corruptedFilesFolder");
    if(folder.empty())
    {
        return;
    }

    DIR * dir = opendir(folder.c_str());
    if(dir == 0)
    {
        std::cout << folder << " not found" << std::endl;
        ASSERT_FALSE(true);
    }

    for(dirent* dirEntry = readdir(dir); dirEntry != 0; dirEntry = readdir(dir))
    {
        const std::string fileName(dirEntry->d_name);
        if(fileName.at(0) == '.')
        {
            continue;
        }
        std::ostringstream fullName;
        fullName << folder << "/" << fileName;

        std::cout << "Processing corrupted file " << fullName.str() << std::endl;

        std::shared_ptr<puntoexe::imebra::codecs::codecFactory> factory = puntoexe::imebra::codecs::codecFactory::getCodecFactory();

        std::shared_ptr<puntoexe::stream> inputStream(new puntoexe::stream());
        inputStream->openFile(fullName.str(), std::ios::in);

        std::shared_ptr<puntoexe::streamReader> reader = std::make_shared<puntoexe::streamReader>(inputStream);

        std::shared_ptr<puntoexe::imebra::dataSet> dataset = factory->load(reader, 2048);

        ASSERT_THROW(dataset->getImage(0), puntoexe::imebra::codecs::codecException);
    }
}

}

}

}
