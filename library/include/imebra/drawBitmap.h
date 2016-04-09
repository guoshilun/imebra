/*
$fileHeader$
*/

/*! \file drawBitmap.h
    \brief Declaration of the class DrawBitmap.

*/

#if !defined(imebraDrawBitmap__INCLUDED_)
#define imebraDrawBitmap__INCLUDED_

#include <memory>
#include "definitions.h"
#include "readWriteMemory.h"

#ifndef SWIG
namespace imebra
{
namespace implementation
{
class drawBitmap;
}

}
#endif

namespace imebra
{

class TransformsChain;
class Image;

class IMEBRA_API DrawBitmap
{
    DrawBitmap(const DrawBitmap&) = delete;
    DrawBitmap& operator=(const DrawBitmap&) = delete;

public:
    DrawBitmap();

    DrawBitmap(const TransformsChain& transformsChain);

    virtual ~DrawBitmap();

    size_t getBitmap(const Image& image, drawBitmapType_t drawBitmapType, std::uint32_t rowAlignBytes, char* buffer, size_t bufferSize);

    ReadWriteMemory* getBitmap(const Image& image, drawBitmapType_t drawBitmapType, std::uint32_t rowAlignBytes);

#ifndef SWIG
protected:
    std::shared_ptr<implementation::drawBitmap> m_pDrawBitmap;
#endif

};

}

#endif // !defined(imebraDrawBitmap__INCLUDED_)
