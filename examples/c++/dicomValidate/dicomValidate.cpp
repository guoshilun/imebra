//
// Created by dhz on 2021/11/4.
//

/*
$fileHeader$
*/

#include <iostream>


#include <imebra/imebra.h>
#include <sstream>

#if defined(WIN32) || defined(WIN64)
#include <process.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#endif
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <memory>
#include <list>

using namespace imebra;

int findArgument(const char* argument, int argc, char* argv[])
{
    for(int scanArg(0); scanArg != argc; ++scanArg)
    {
        if(std::string(argv[scanArg]) == argument)
        {
            return scanArg;
        }
    }
    return -1;
}
void  printTagInfo(const std::wstring& prefix ,uint16_t groupId, uint16_t tagId, std::wstring& tagName  ){
    char tagGrp[7]={0};
    snprintf(tagGrp, 7 , "0x%04X", groupId);
     char tagTag[7]={0};
    snprintf(tagTag, 7 , "0x%04X", tagId);
    std::wcout << prefix << L"Tag " << tagGrp << L"," <<  tagTag <<  L" (" << tagName << L")" << std::endl;
}

void outputDatasetTags(const DataSet& dataset, const std::wstring& prefix)
{
    tagsIds_t tags = dataset.getTags();

    // Output all the tags
    for(tagsIds_t::const_iterator scanTags(tags.begin()), endTags(tags.end());
        scanTags != endTags;
        ++scanTags)
    {
        try
        {
            std::wstring tagName = DicomDictionary::getUnicodeTagDescription(*scanTags);
         //   std::wcout << prefix << L"Tag " << (*scanTags).getGroupId() << L"," << (*scanTags).getTagId() << L" (" << tagName << L")" << std::endl;

            printTagInfo(prefix , (*scanTags).getGroupId()  ,  (*scanTags).getTagId()  , tagName);
        }
        catch(const DictionaryUnknownTagError&)
        {
            std::wcout << prefix << L"Tag " << (*scanTags).getGroupId() << L"," << (*scanTags).getTagId() << L" (Unknown tag)" << std::endl;
        }

        Tag tag(dataset.getTag(*scanTags));

        for(size_t itemId(0); ; ++itemId)
        {
            try
            {
                DataSet sequence = tag.getSequenceItem(itemId);
                std::wcout << prefix << L"  SEQUENCE " << itemId << std::endl;
                outputDatasetTags(sequence, prefix + L"    ");
            }
            catch(const MissingDataElementError&)
            {
                break;
            }
        }

        for(size_t bufferId(0); bufferId != tag.getBuffersCount(); ++bufferId)
        {
            ReadingDataHandler handler = tag.getReadingDataHandler(bufferId);
            if(handler.getDataType() != imebra::tagVR_t::OW && handler.getDataType() != imebra::tagVR_t::OB)
            {
                for(size_t scanHandler(0); scanHandler != handler.getSize(); ++scanHandler)
                {
                    std::wcout << prefix << L"  buffer " << bufferId << L", position "<< scanHandler << ":" << handler.getUnicodeString(scanHandler) << std::endl;
                }
            }
            else
            {
                std::wcout << prefix << L"  Not shown: size " << handler.getSize() << " elements" << std::endl;
            }

        }
    }
}

#define  MAX_PATH_LEN  1024
static void trave_dir(char* path,std::list<std::string>& result) {
    DIR *d = NULL;
    struct dirent *dp = NULL; /* readdir函数的返回值就存放在这个结构体中 */
    struct stat st;
    char p[MAX_PATH_LEN] = {0};

    if(stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
        return;
    }

    if(!(d = opendir(path))) {

        return;
    }

    while((dp = readdir(d)) != NULL) {
        /* 把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录 */
        if((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2)))
            continue;
        snprintf(p, sizeof(p) - 1, "%s/%s", path, dp->d_name);
        stat(p, &st);
        if(!S_ISDIR(st.st_mode)) {
            std::stringstream oss;
            oss << path << "/" <<  dp->d_name ;
            result.push_back( oss.str());
            oss.clear();

        } else {
            trave_dir(p, result);
        }
    }
    closedir(d);

    return;
}

int main(int argc, char* argv[])
{
    setlocale( LC_ALL, "zh_CN.UTF-8" );
//    std::wcout <<  L"中国" << std::endl;
//    wprintf( L"中国2\n" );
    std::wstring version(L"1.0.0.1");
    std::wcout << L"dicomValidate version " << version << std::endl;

    imebra::CodecFactory::setMaximumImageSize(8000, 8000);

    try
    {

        if(argc < 2)
        {
            std::wcout << L"Usage: dicomValidate  dicomFileName " << std::endl;

            return 1;
        }
        std::list<std::string> allFiles ;
        trave_dir(argv[1], allFiles);

        std::wcout << "共计多少个文件："<< allFiles.size() << std::endl;

        imebra::CodecFactory::setMaximumImageSize(8000, 8000);


        size_t counter =0;



        for(const  auto cx :allFiles){
            std::wcout <<  cx.c_str() <<std::endl;
            DataSet loadedDataSet = CodecFactory::load(cx, 2048);
            outputDatasetTags(loadedDataSet, L"");
            counter = counter + 1 ;
        }

        std::wcout <<L"预计读取："<< allFiles.size() <<  L",实际读取："<< counter <<L"个文件" << std::endl;
        return  0 ;

    }
    catch(const std::exception& e)
    {
        MutableDataSet convertMessage;
        convertMessage.setString(TagId(10, 10), imebra::ExceptionsManager::getExceptionTrace(), imebra::tagVR_t::LT);
        std::wcout << std::endl << "Error: " << std::endl << convertMessage.getUnicodeString(TagId(10, 10), 0) << std::endl;
        return 1;
    }
}

