/*
$fileHeader$
*/

/*! \file drawBitmap.cpp
    \brief Implementation of the class DrawBitmap.

*/

#include "../include/imebra/drawBitmap.h"
#include "../implementation/drawBitmapImpl.h"
#include "../include/imebra/image.h"
#include "../include/imebra/transform.h"

namespace imebra
{

DrawBitmap::DrawBitmap():
    m_pDrawBitmap(std::make_shared<implementation::drawBitmap>(std::shared_ptr<implementation::transforms::transform>()))
{
}

DrawBitmap::DrawBitmap(const Transform& transform):
    m_pDrawBitmap(std::make_shared<implementation::drawBitmap>(transform.m_pTransform))
{
}

DrawBitmap::~DrawBitmap()
{
}

size_t DrawBitmap::getBitmap(const Image& image, drawBitmapType_t drawBitmapType, std::uint32_t rowAlignBytes, char* buffer, size_t bufferSize)
{
    return m_pDrawBitmap->getBitmap(image.m_pImage, drawBitmapType, rowAlignBytes, (std::uint8_t*)buffer, bufferSize);
}

ReadWriteMemory* DrawBitmap::getBitmap(const Image& image, drawBitmapType_t drawBitmapType, std::uint32_t rowAlignBytes)
{
    return new ReadWriteMemory(m_pDrawBitmap->getBitmap(image.m_pImage, drawBitmapType, rowAlignBytes));
}

}
