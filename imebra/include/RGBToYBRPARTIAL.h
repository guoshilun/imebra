/*
$fileHeader$
*/

/*! \file RGBToYBRPARTIAL.h
    \brief Declaration of the class RGBToYBRPARTIAL.

*/

#if !defined(imebraRGBToYBRPARTIAL_E27C63E7_A907_4899_9BD3_8026AD7D110C__INCLUDED_)
#define imebraRGBToYBRPARTIAL_E27C63E7_A907_4899_9BD3_8026AD7D110C__INCLUDED_

#include "colorTransform.h"


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace transforms
{

namespace colorTransforms
{

///////////////////////////////////////////////////////////
/// \brief Transforms an image from the colorspace 
///         RGB into the color space YBR_PARTIAL.
///
/// The input image has to have the colorspace RGB,
///  while the output image is created by the transform
///  and will have the colorspace YBR_PARTIAL.
///
///////////////////////////////////////////////////////////
class RGBToYBRPARTIAL: public colorTransform
{
public:
	virtual std::wstring getInitialColorSpace();
	virtual std::wstring getFinalColorSpace();
	virtual ptr<colorTransform> createColorTransform();

protected:
	virtual void doColorTransform(imbxInt32* pSourceMem, imbxInt32* pDestMem, imbxUint32 pixelsNumber, imbxInt32 inputMinValue, imbxInt32 inputMaxValue, imbxInt32 outputMinValue, imbxInt32 outputMaxValue);
};

} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraRGBToYBRPARTIAL_E27C63E7_A907_4899_9BD3_8026AD7D110C__INCLUDED_)
