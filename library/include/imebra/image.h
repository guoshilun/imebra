/*
$fileHeader$
*/

/*! \file image.h
    \brief Declaration of the class Image.

*/

#if !defined(imebraImage__INCLUDED_)
#define imebraImage__INCLUDED_

#ifndef SWIG

#include <memory>
#include <string>
#include "definitions.h"
#include "readingDataHandlerNumeric.h"
#include "writingDataHandlerNumeric.h"

namespace imebra
{
namespace implementation
{
class image;
}
}
#endif

namespace imebra
{

class CodecFactory;
class Transform;
class VOILUT;
class DataSet;
class DrawBitmap;

class IMEBRA_API Image
{

#ifndef SWIG
    friend class CodecFactory;
    friend class Transform;
    friend class VOILUT;
    friend class DataSet;
    friend class DrawBitmap;

private:
    Image(std::shared_ptr<imebra::implementation::image> pImage);
#endif

public:

    /// \brief Construct an empty image.
    ///
    /// You can assign an image to the object by using the
    /// operator=() or by calling create() to allocate an
    /// image with the desidered size.
    ///
	///////////////////////////////////////////////////////////
	Image();

    Image(
        std::uint32_t width,
        std::uint32_t height,
        bitDepth_t depth,
        const std::string& colorSpace,
        std::uint32_t highBit);

    /// \brief Construct an image that references the image
    ///        passed in the parameter.
    ///
    /// \param right the Image object referencing the image to
    ///              reference in this object
    ///
    ///////////////////////////////////////////////////////////
	Image(const Image& right);

    virtual ~Image();

    /// \brief Copy into the object the reference to the image
    ///        referenced by the parameter.
    ///
    /// \param right the Image object referencing the image to
    ///              reference in this object
    ///
    ///////////////////////////////////////////////////////////
    Image& operator=(const Image& right);



    double getSizeMmY() const;
    double getSizeMmX() const;

    void setSizeMm(const double width, const double height);

    std::uint32_t getWidth() const;
    std::uint32_t getHeight() const;

    ReadingDataHandlerNumeric getReadingDataHandler() const;
    WritingDataHandlerNumeric getWritingDataHandler();

    std::string getColorSpace() const;

    std::uint32_t getChannelsNumber() const;

    bitDepth_t getDepth() const;

    std::uint32_t getHighBit() const;

#ifndef SWIG
protected:
    std::shared_ptr<implementation::image> m_pImage;
#endif

};

}

#endif // !defined(imebraImage__INCLUDED_)
